#pragma once
#include"PSO/PSO.h"
#include"RenderTexture/RenderTexture.h"
#include"TransitionBarrier.h"

class PostProcessManager {
public:
    // 適用したいエフェクトを動的に追加する
    void AddEffect(PSO::EffectType type) {
        activeEffects_.push_back(type);
    }

    void ClearEffects() { activeEffects_.clear(); }

    void Execute(RenderTexture* renderTexture,
        const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle,
        TransitionBarrier* barrier,
        const uint32_t depthSrvIndex,
        const BlendMode& randomBlendMode) {
        // 有効なエフェクトが無ければ通常描画して終了
        if (activeEffects_.empty()) {
            renderTexture->Draw(PSO::kEffectNone, dstRtvHandle, 0);
            return;
        }

        // 登録されているエフェクトの数だけループして、
        // RenderTextureのIndex 0 と Index 1 を交互に切り替えながら描画していく
        uint32_t srcIndex = 0;
        uint32_t dstIndex = 1;

        for (auto effect : activeEffects_) {
            auto& renderTextureData = renderTexture->GetRenderTextureData(static_cast<RenderTexture::RenderTextureType>(dstIndex));
            // srcIndexのテクスチャを読み込んで、dstIndexのRTVに描画する

            //Randomをどうするか
            barrier->SettingBarrierSRVforRTV(renderTextureData.resource);

            if (effect == PSO::EffectType::kEffectDissolve) {
                renderTexture->DrawDissolve(renderTextureData.rtvHandleCPU, srcIndex, TextureFactory::NOIZE0);
            } else if (effect == PSO::EffectType::kEffectDepthBasedOutline) {
                renderTexture->DrawOutLine(renderTextureData.rtvHandleCPU, srcIndex, depthSrvIndex);
            } else if (effect == PSO::EffectType::kEffectThermography) {
                //サーモグラフィーのターゲットを戻す
                auto& renderTextureDataForThermography = renderTexture->GetRenderTextureData(RenderTexture::kThermography);
                renderTexture->DrawThermo(renderTextureData.rtvHandleCPU);
                barrier->SettingBarrierRTVforSRV(renderTextureDataForThermography.resource);

            } else if (effect == PSO::kEffectRandom) {
                renderTexture->DrawRandom(randomBlendMode, renderTextureData.rtvHandleCPU, srcIndex);
            } else {
                renderTexture->Draw(effect, renderTextureData.rtvHandleCPU, srcIndex);
            }

            barrier->SettingBarrierRTVforSRV(renderTextureData.resource);

            // 読み込みと書き込みの対象を入れ替える（ピンポン）
            std::swap(srcIndex, dstIndex);
        }

        renderTexture->Draw(PSO::kEffectNone, dstRtvHandle, srcIndex);
    }

private:
    std::vector<PSO::EffectType> activeEffects_; // 現在有効なエフェクトのリスト
};