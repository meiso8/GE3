#include "PostProcessManager.h"
#include<cassert>
#include"Log.h"
#include"PostEffectMaterial/PostEffectMaterial.h"
#include"Texture.h"
#include"TransitionBarrier.h"
#include"CbvSrvUavDescriptorHeap.h"
#include"Camera.h"
#include"DepthTexture/DepthTexture.h"

ID3D12GraphicsCommandList* PostProcessManager::commandList_ = nullptr;
CbvSrvUavDescriptorHeap* PostProcessManager::srvDescriptorHeap_ = nullptr;
DepthTexture* PostProcessManager::depthTexture_ = nullptr;

void PostProcessManager::Create(ID3D12GraphicsCommandList* commandList,
    CbvSrvUavDescriptorHeap* srvDescriptorHeap, DepthTexture* depthTexture)
{
    commandList_ = commandList;
    assert(commandList_);
    srvDescriptorHeap_ = srvDescriptorHeap;
    assert(srvDescriptorHeap_);
    depthTexture_ = depthTexture;
    assert(depthTexture_);

    for (auto& l : renderLayer_) {
        l.postEffectMaterial_ = std::make_unique<PostEffectMaterial>();
        l.postEffectMaterial_->Create();
    }
}

void PostProcessManager::Update()
{
    for (auto& l : renderLayer_) {
        l.postEffectMaterial_->Update();
    }
}


void PostProcessManager::SetPostEffectMaterialCamera(Camera* camera, const Layer& layer)
{
    renderLayer_[layer].postEffectMaterial_->SetCamera(camera);
}

void PostProcessManager::SetRenderTexture(RenderTexture* renderTexture, const Layer& layer)
{
    renderLayer_[layer].renderTexture_ = renderTexture;
    assert(renderLayer_[layer].renderTexture_);
}

void PostProcessManager::Execute(
    const Layer& layer, 
    const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle, 
    TransitionBarrier* barrier,
    const BlendMode& randomBlendMode)
{

    // 有効なエフェクトが無ければ通常描画して終了
    if (renderLayer_[layer].activeEffects_.empty()) {
        Draw(layer, PSO::kEffectNone, dstRtvHandle, 0);
        return;
    }

    // 登録されているエフェクトの数だけループして、
    // RenderTextureのIndex 0 と Index 1 を交互に切り替えながら描画していく
    uint32_t srcIndex = 0;
    uint32_t dstIndex = 1;

    for (auto effect : renderLayer_[layer].activeEffects_) {
        auto& renderTextureData = renderLayer_[layer].renderTexture_->GetRenderTextureData(static_cast<RenderTexture::RenderTextureType>(dstIndex));
        // srcIndexのテクスチャを読み込んで、dstIndexのRTVに描画する

        //Randomをどうするか
        barrier->SettingBarrierSRVforRTV(renderTextureData.resource.resource);

        if (effect == PSO::EffectType::kEffectDissolve) {
            DrawDissolve(layer, renderTextureData.rtvHandleCPU, srcIndex, TextureFactory::NOIZE0);
        } else if (effect == PSO::EffectType::kEffectDepthBasedOutline) {
            DrawOutLine(layer, renderTextureData.rtvHandleCPU, srcIndex, depthTexture_->GetSRVIndex());
        } else if (effect == PSO::EffectType::kEffectThermography) {
            //サーモグラフィーのターゲットを戻す
            DrawThermo(layer, renderTextureData.rtvHandleCPU);
        } else if (effect == PSO::kEffectRandom) {
            DrawRandom(layer, randomBlendMode, renderTextureData.rtvHandleCPU, srcIndex);
        } else {
            Draw(layer, effect, renderTextureData.rtvHandleCPU, srcIndex);
        }

        barrier->SettingBarrierRTVforSRV(renderTextureData.resource.resource);

        // 読み込みと書き込みの対象を入れ替える（ピンポン）
        std::swap(srcIndex, dstIndex);
    }

    Draw(layer, PSO::kEffectNone, dstRtvHandle, srcIndex);


}

void PostProcessManager::DrawDissolve(const Layer& layer, const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle, const uint32_t index, const TextureFactory::Handle& textureHandle) {

    // 1. 書き込み先（RTV）の設定とクリア
    commandList_->OMSetRenderTargets(1, &dstRtvHandle, false, nullptr);

    commandList_->SetGraphicsRootSignature(PSO::rootSignature->GetRootSignature(RootSignature::DISSOLVE));
    commandList_->SetPipelineState(PSO::GetGraphicsPipelineStateOffScreen(PSO::kEffectDissolve).Get());//PSOを設定
    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //SRVのDescriptorTableの先頭を設定。0はrootParameter[0]である。
    commandList_->SetGraphicsRootConstantBufferView(0, renderLayer_[layer].postEffectMaterial_->GetGPUVirtualAddress(PSO::kEffectDissolve));
    srvDescriptorHeap_->SetGraphicsRootDescriptorTable(1, Texture::GetSRVHandle(textureHandle), commandList_);

    DrawCallforRenderTexture(layer, 2, index);
    commandList_->DrawInstanced(3, 1, 0, 0);


}
void PostProcessManager::DrawRandom(const Layer& layer, const BlendMode& blendMode, const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle, const uint32_t index)
{
    // 1. 書き込み先（RTV）の設定とクリア
    commandList_->OMSetRenderTargets(1, &dstRtvHandle, false, nullptr);
    commandList_->SetGraphicsRootSignature(PSO::rootSignature->GetRootSignature(RootSignature::RANDOM));
    commandList_->SetPipelineState(PSO::GetGraphicsPipelineStateRandom(blendMode).Get());//PSOを設定
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    commandList_->SetGraphicsRootConstantBufferView(0, renderLayer_[layer].postEffectMaterial_->GetGPUVirtualAddress(PSO::kEffectRandom));
    commandList_->DrawInstanced(3, 1, 0, 0);
};

void PostProcessManager::Finalize()
{
    for (auto& renderLayer : renderLayer_) {
        renderLayer.activeEffects_.clear();
        renderLayer.postEffectMaterial_.reset();
    }
}

void PostProcessManager::Draw(const Layer& layer, const PSO::EffectType& effectType, const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle, const uint32_t index)
{
    // 1. 書き込み先（RTV）の設定とクリア
    commandList_->OMSetRenderTargets(1, &dstRtvHandle, false, nullptr);

    commandList_->SetGraphicsRootSignature(PSO::rootSignature->GetRootSignature(RootSignature::OFFSCREEN));
    commandList_->SetPipelineState(PSO::GetGraphicsPipelineStateOffScreen(effectType).Get());//PSOを設定
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //SRVのDescriptorTableの先頭を設定。0はrootParameter[0]である。
    DrawCallforRenderTexture(layer, 0, index);

    commandList_->SetGraphicsRootConstantBufferView(1, renderLayer_[layer].postEffectMaterial_->GetGPUVirtualAddress(effectType));
    commandList_->DrawInstanced(3, 1, 0, 0);
}

void PostProcessManager::DrawOutLine(const Layer& layer, const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle, const uint32_t index, const uint32_t depthSrvIndex)
{
    // 1. 書き込み先（RTV）の設定とクリア
    commandList_->OMSetRenderTargets(1, &dstRtvHandle, false, nullptr);

    commandList_->SetGraphicsRootSignature(PSO::rootSignature->GetRootSignature(RootSignature::DEPTH_BASED_OUTLINE));
    commandList_->SetPipelineState(PSO::GetGraphicsPipelineStateOffScreen(PSO::kEffectDepthBasedOutline).Get());//PSOを設定
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //SRVのDescriptorTableの先頭を設定。0はrootParameter[0]である。
    commandList_->SetGraphicsRootConstantBufferView(0, renderLayer_[layer].postEffectMaterial_->GetGPUVirtualAddress(PSO::kEffectDepthBasedOutline));
    srvDescriptorHeap_->SetGraphicsRootDescriptorTable(1, depthSrvIndex, commandList_);

    DrawCallforRenderTexture(layer, 2, index);
    //サーモグラフィー用のテクスチャを利用してマスク処理をかける
    DrawCallforRenderTexture(layer, 3, RenderTexture::RenderTextureType::kThermography);

    commandList_->DrawInstanced(3, 1, 0, 0);
}
// RenderTexture.cpp にサーモグラフィー用の描画関数を追加する例
void PostProcessManager::DrawThermo(const Layer& layer, const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle)
{
    // 書き込み先はスワップチェーン（画面出力用）など
    commandList_->OMSetRenderTargets(1, &dstRtvHandle, false, nullptr);

    // ★ サーモグラフィー用のPSOやRootSignatureを設定
    commandList_->SetGraphicsRootSignature(PSO::rootSignature->GetRootSignature(RootSignature::THERMOGRAPHY));
    commandList_->SetPipelineState(PSO::GetGraphicsPipelineStateOffScreen(PSO::kEffectThermography).Get());
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 温度テクスチャ(t2)をシェーダーに渡す
    commandList_->SetGraphicsRootConstantBufferView(0, renderLayer_[layer].postEffectMaterial_->GetGPUVirtualAddress(PSO::kEffectThermography));
    // 温度用
    DrawCallforRenderTexture(layer, 1, RenderTexture::RenderTextureType::kThermography);
    commandList_->DrawInstanced(3, 1, 0, 0);
}

void PostProcessManager::DrawCallforRenderTexture(const Layer& layer, UINT rootParameterIndex, const uint32_t index)
{
    srvDescriptorHeap_->SetGraphicsRootDescriptorTable(
        rootParameterIndex,
        renderLayer_[layer].renderTexture_->GetRenderTextureData(static_cast<RenderTexture::RenderTextureType>(index)).resource.srvIndex,
        commandList_
    );
}
