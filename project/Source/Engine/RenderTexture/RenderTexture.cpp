#include "RenderTexture.h"
#include"DirectXCommon.h"
#include"Engine/SRVManager/SRVManager.h"
#include"Engine/RtvManager/RtvManager.h"
#include"PSO.h"
#ifdef _DEVELOP
#include "DebugUI.h"
#endif
#include"Log.h"
#include"MakeMatrix.h"


Microsoft::WRL::ComPtr<ID3D12Resource>& RenderTexture::GetMaterialResouce(const PSO::EffectType& effectType)
{
    assert(effectType < PSO::kCountOfEffect);
    return materialResource_[effectType];
}


void RenderTexture::Create(RtvManager& rtvManager)
{
    kRenderTargetClearValue_ = { 1.0f,0.0f,0.0f,1.0f };

    CreateResource(kNormal0,rtvManager, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, true);
    CreateResource(kNormal1,rtvManager, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, true);
    //サーモグラフィー用テクスチャ
    CreateResource(kThermography, rtvManager,DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, true);
    //ObjectID用テクスチャ　
    CreateResource(kObjectID, rtvManager,DXGI_FORMAT_R32_UINT, false);
    //ObjectID用リソース
    idReadbackResource_ = DirectXCommon::CreateReadbackBufferResource(sizeof(uint32_t));

    CreateMaterialBUfferForFullScreen();
    CreateMaterialBufferForGrayScale();
    CreateMaterialBufferForVignette();
    CreateMaterialBufferForBoxFilter();
    CreateMaterialBufferForGaussianFilter();
    CreateMaterialLuminanceBasedOutline();
    CreateMaterialDepthBasedOutline();
    CreateMaterialRadialBlur();
    CreateMaterialDissolve();
    CreateMaterialRandom();
    CreateMaterialThermography();
}

void RenderTexture::SetCommandList(ID3D12GraphicsCommandList* commandList)
{
    commandList_ = commandList;
    assert(commandList_);
}

void RenderTexture::CreateResource(const uint32_t index, RtvManager& rtvManager,DXGI_FORMAT format, bool createSRV)
{
    //rtvの作成
    renderTextureDatas_[index].resource =
        DirectXCommon::CreateRenderTextureResource(
            Window::GetClientWidth(),
            Window::GetClientHeight(),
            format,
            kRenderTargetClearValue_
        );

    LogFile::Log("Rendertexture : CreateRTV\n");

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = format;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    uint32_t rtvIndex = rtvManager.Allocate();
    renderTextureDatas_[index].rtvHandleCPU = rtvManager.GetCPUDescriptorHandle(rtvIndex);
    DirectXCommon::GetDevice()->CreateRenderTargetView(renderTextureDatas_[index].resource.Get(), &rtvDesc, renderTextureDatas_[index].rtvHandleCPU);

    LogFile::Log("Rendertexture : CreateRTVDesc\n");

    // ------------------------------------------

    // 3. SRVは必要に応じて作成する
    if (createSRV) {
        //SRVの作成
        D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
        renderTextureSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        renderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        renderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        renderTextureSrvDesc.Texture2D.MipLevels = 1;
        LogFile::Log("Rendertexture : Create SRV\n");

        renderTextureDatas_[index].srvIndex = SrvManager::Allocate();
        renderTextureDatas_[index].srvHandleCPU = SrvManager::GetCPUDescriptorHandle(renderTextureDatas_[index].srvIndex);
        renderTextureDatas_[index].srvHandleGPU = SrvManager::GetGPUDescriptorHandle(renderTextureDatas_[index].srvIndex);
        LogFile::Log("Rendertexture : GetSRVIndexAndGPUAndCPUHandle\n");

        DirectXCommon::GetDevice()->CreateShaderResourceView(renderTextureDatas_[index].resource.Get(), &renderTextureSrvDesc, renderTextureDatas_[index].srvHandleCPU);
        LogFile::Log("Rendertexture : CreateShaderResourceView\n");
    }

}


void RenderTexture::CopyClickPixelCommand(int mouseX, int mouseY)
{   // 画面外の座標の場合は弾く (バッファオーバーフロー防止)
    if (mouseX < 0 || mouseX >= 1280 || mouseY < 0 || mouseY >= 720) {
        return;
    }

    // kObjectIDのリソースポインタを取得
    ID3D12Resource* idTexture = renderTextureDatas_[kObjectID].resource.Get();
    ID3D12Resource* readbackBuffer = idReadbackResource_.Get();
    if (!idTexture || !readbackBuffer) return;

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = idTexture;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList_->ResourceBarrier(1, &barrier);

    // 2. コピー元のテクスチャ上の1ピクセルの位置（マウス位置）を指定
    D3D12_TEXTURE_COPY_LOCATION srcLocation{};
    srcLocation.pResource = idTexture;
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    srcLocation.SubresourceIndex = 0;

    D3D12_BOX srcBox{};
    srcBox.left = mouseX;
    srcBox.top = mouseY;
    srcBox.front = 0;
    srcBox.right = mouseX + 1;  // 1ピクセル幅
    srcBox.bottom = mouseY + 1; // 1ピクセル高
    srcBox.back = 1;

    // 3. コピー先のReadbackバッファの指定（バッファ構造なのでFOOTPRINT形式）
    D3D12_TEXTURE_COPY_LOCATION dstLocation{};
    dstLocation.pResource = idReadbackResource_.Get();
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    dstLocation.PlacedFootprint.Offset = 0;
    dstLocation.PlacedFootprint.Footprint.Width = 1;
    dstLocation.PlacedFootprint.Footprint.Height = 1;
    dstLocation.PlacedFootprint.Footprint.Depth = 1;
    dstLocation.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R32_UINT; // IDのフォーマット
    dstLocation.PlacedFootprint.Footprint.RowPitch = 256; // アライメントの最低規則(256バイト)に合わせる

    // 4. GPUコマンドリストにコピー命令を発行
    commandList_->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, &srcBox);

    // 3. 元の状態に戻す
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // ★1で指定した元の状態に戻す
    commandList_->ResourceBarrier(1, &barrier);

}

uint32_t RenderTexture::GetClickedObjectID()
{
    ID3D12Resource* readbackBuffer = idReadbackResource_.Get();
    if (!readbackBuffer) return 0;

    // 7. GPUの処理が終わったら、CPU側でバッファをMapして数値を読み出す
    uint32_t clickedID = 0;
    uint32_t* mappedData = nullptr;

    // 読み出し専用なので、Rangeの設定は全体
    D3D12_RANGE readRange{ 0, sizeof(uint32_t) };
    if (SUCCEEDED(readbackBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mappedData)))) {
        clickedID = *mappedData; // ピクセルに書き込まれていたIDを取得！
        readbackBuffer->Unmap(0, nullptr); // すぐにアンマップ
    }


    return clickedID;
}

void RenderTexture::DrawDissolve(const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle, const uint32_t index, const TextureFactory::Handle& textureHandle) {

    // 1. 書き込み先（RTV）の設定とクリア
    commandList_->OMSetRenderTargets(1, &dstRtvHandle, false, nullptr);
    commandList_->SetGraphicsRootSignature(PSO::rootSignature->GetRootSignature(RootSignature::DISSOLVE));
    commandList_->SetPipelineState(PSO::GetGraphicsPipelineStateOffScreen(PSO::kEffectDissolve).Get());//PSOを設定
    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //SRVのDescriptorTableの先頭を設定。0はrootParameter[0]である。
    commandList_->SetGraphicsRootConstantBufferView(0, materialResource_[PSO::kEffectDissolve]->GetGPUVirtualAddress());
    SrvManager::SetGraphicsRootDescriptorTable(1, Texture::GetSRVHandle(textureHandle), commandList_);
    SrvManager::SetGraphicsRootDescriptorTable(2, renderTextureDatas_[index].srvIndex, commandList_);
    commandList_->DrawInstanced(3, 1, 0, 0);


}
void RenderTexture::DrawRandom(const BlendMode& blendMode, const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle, const uint32_t index)
{
    // 1. 書き込み先（RTV）の設定とクリア
    commandList_->OMSetRenderTargets(1, &dstRtvHandle, false, nullptr);
    commandList_->SetGraphicsRootSignature(PSO::rootSignature->GetRootSignature(RootSignature::RANDOM));
    commandList_->SetPipelineState(PSO::GetGraphicsPipelineStateRandom(blendMode).Get());//PSOを設定
    commandList_->SetGraphicsRootConstantBufferView(0, materialResourceRandom_->GetGPUVirtualAddress());
    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList_->DrawInstanced(3, 1, 0, 0);
};

void RenderTexture::Draw( const PSO::EffectType& effectType, const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle, const uint32_t index)
{
    // 1. 書き込み先（RTV）の設定とクリア
    commandList_->OMSetRenderTargets(1, &dstRtvHandle, false, nullptr);
    commandList_->SetGraphicsRootSignature(PSO::rootSignature->GetRootSignature(RootSignature::OFFSCREEN));
    commandList_->SetPipelineState(PSO::GetGraphicsPipelineStateOffScreen(effectType).Get());//PSOを設定
    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //SRVのDescriptorTableの先頭を設定。0はrootParameter[0]である。
    SrvManager::SetGraphicsRootDescriptorTable(0, renderTextureDatas_[index].srvIndex, commandList_);
    commandList_->SetGraphicsRootConstantBufferView(1, materialResource_[effectType]->GetGPUVirtualAddress());
    commandList_->DrawInstanced(3, 1, 0, 0);
}

void RenderTexture::DrawOutLine( const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle, const uint32_t index, const uint32_t depthSrvIndex)
{
    // 1. 書き込み先（RTV）の設定とクリア
    commandList_->OMSetRenderTargets(1, &dstRtvHandle, false, nullptr);

    commandList_->SetGraphicsRootSignature(PSO::rootSignature->GetRootSignature(RootSignature::DEPTH_BASED_OUTLINE));

    commandList_->SetPipelineState(PSO::GetGraphicsPipelineStateOffScreen(PSO::kEffectDepthBasedOutline).Get());//PSOを設定
    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //SRVのDescriptorTableの先頭を設定。0はrootParameter[0]である。
    commandList_->SetGraphicsRootConstantBufferView(0, materialResource_[PSO::kEffectDepthBasedOutline]->GetGPUVirtualAddress());
    SrvManager::SetGraphicsRootDescriptorTable(1, depthSrvIndex, commandList_);
    SrvManager::SetGraphicsRootDescriptorTable(2, renderTextureDatas_[index].srvIndex, commandList_);
    //サーモグラフィー用のテクスチャを利用してマスク処理をかける
    SrvManager::SetGraphicsRootDescriptorTable(3, renderTextureDatas_[kThermography].srvIndex, commandList_);
    commandList_->DrawInstanced(3, 1, 0, 0);
}
// RenderTexture.cpp にサーモグラフィー用の描画関数を追加する例
void RenderTexture::DrawThermo( const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle)
{
    // 書き込み先はスワップチェーン（画面出力用）など
    commandList_->OMSetRenderTargets(1, &dstRtvHandle, false, nullptr);

    // ★ サーモグラフィー用のPSOやRootSignatureを設定
    commandList_->SetGraphicsRootSignature(PSO::rootSignature->GetRootSignature(RootSignature::THERMOGRAPHY));
    commandList_->SetPipelineState(PSO::GetGraphicsPipelineStateOffScreen(PSO::kEffectThermography).Get());
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 温度テクスチャ(t2)をシェーダーに渡す
    commandList_->SetGraphicsRootConstantBufferView(0, materialResource_[PSO::kEffectThermography]->GetGPUVirtualAddress());
    SrvManager::SetGraphicsRootDescriptorTable(1, renderTextureDatas_[kThermography].srvIndex, commandList_); // 温度用

    commandList_->DrawInstanced(3, 1, 0, 0);
}
void RenderTexture::Update()
{
    assert(camera_);
    materialForDepthBasedOutline_->projectionInverse = Inverse(camera_->GetProjectionMatrixForOutline());

#ifdef _DEVELOP

    ImGui::Begin("PosEffect");

    if (ImGui::TreeNode("GrayScale")) {
        DebugUI::CheckColor(materialForGrayScale_->color, "RenderTextureColor");
        ImGui::Checkbox("UseEffect", &materialForGrayScale_->useEffect);
        const char* lights[] = { "Sepia", "Grayscale", "None" };
        int type_current = int(materialForGrayScale_->type);

        if (ImGui::Combo("EffectType", &type_current, lights, IM_ARRAYSIZE(lights))) {
            materialForGrayScale_->type = type_current % 3;
        };
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Vignette")) {
        DebugUI::CheckFloat(materialForVignette_->correctVal, "correctVal");
        DebugUI::CheckFloat(materialForVignette_->viignetteVal, "viignetteVal");
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("BoxFilter")) {

        ImGui::DragFloat("kernel", &materialForBoxFilter_->kernel, 1.0f, 0.0f, 1001.0f);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("GaussianFilter")) {

        ImGui::DragInt("kernel", &materialForGaussianFilter_->kernel, 1, 1);
        ImGui::DragFloat("sigma", &materialForGaussianFilter_->sigma);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("LuminanceBasedOutline")) {

        ImGui::DragFloat("weightVal", &materialForLuminanceBasedOutline_->weightVal, 0.1f);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("DepthBasedOutline")) {
        ImGui::DragFloat("lineWidth", &materialForDepthBasedOutline_->lineWidth, 0.1f);
        DebugUI::ShowMatrix4x4(materialForDepthBasedOutline_->projectionInverse);
        ImGui::ColorEdit3("color", &materialForDepthBasedOutline_->color.x);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("RadialBulr")) {

        ImGui::DragFloat2("center", &materialForRadialBlur_->center.x, 0.01f, 0.0f, 1.0f);
        ImGui::DragInt("numSamples", &materialForRadialBlur_->numSamples, 1.0f, 1);
        ImGui::DragFloat("blurWidth", &materialForRadialBlur_->blurWidth, 0.01f);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Dissolve")) {
        //ImGui::Checkbox("useDissolve", &materialForDissolve_->useDissolve);
        ImGui::DragFloat("maskVal", &materialForDissolve_->maskVal, 0.01f, 0.0f, 1.0f);
        ImGui::ColorEdit3("color", &materialForDissolve_->rgb.x);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Random")) {
        //ImGui::Checkbox("useRandom", &materialForRandom_->useRandom);
        ImGui::DragFloat("time", &materialForRandom_->time, 0.01f);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Thermography")) {
        ImGui::DragFloat("alpha", &materialForThermography_->alpha);
        ImGui::DragInt("kernel", &materialForThermography_->kernel, 1, 1);
        ImGui::DragFloat("sigma", &materialForThermography_->sigma);
        ImGui::TreePop();
    }

    ImGui::End();

#endif
}

void RenderTexture::SetCamera(Camera* camera)
{
    camera_ = camera;
}

void RenderTexture::Clear()
{// すべての ComPtr を Reset して参照を外す
    for (auto& data : renderTextureDatas_) {
        data.resource.Reset();
    }
    thermographyTextureData_.resource.Reset();

    for (auto& mat : materialResource_) {
        // 必要ならここで 各ポインタの Unmap() を行う
        mat.Reset();
    }
    idReadbackResource_.Reset();
    materialResourceRandom_.Reset();
}

void RenderTexture::CreateMaterialBufferForGrayScale()
{
    //マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectGrayScale] = DirectXCommon::CreateBufferResource(sizeof(MaterialForRenderTexture));
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectGrayScale]->Map(0, nullptr, reinterpret_cast<void**>(&materialForGrayScale_));
    materialForGrayScale_->color = sepiaColor_;

    LogFile::Log("Rendertexture : Create : MaterialBuffer : GrayScale\n");
}

void RenderTexture::CreateMaterialBufferForVignette()
{    //マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectVignette] = DirectXCommon::CreateBufferResource(sizeof(MaterialForVignette));
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectVignette]->Map(0, nullptr, reinterpret_cast<void**>(&materialForVignette_));
    materialForVignette_->correctVal = 16.0f;
    materialForVignette_->viignetteVal = 0.8f;

    LogFile::Log("Rendertexture : Create : MaterialBuffer : Vignette\n");
}

void RenderTexture::CreateMaterialBufferForBoxFilter()
{

    materialResource_[PSO::kEffectBoxFilter] = DirectXCommon::CreateBufferResource(sizeof(MaterialForBoxFilter));
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectBoxFilter]->Map(0, nullptr, reinterpret_cast<void**>(&materialForBoxFilter_));
    materialForBoxFilter_->kernel = 1.0f;

    LogFile::Log("Rendertexture : Create : MaterialBuffer : BoxFilter\n");
}

void RenderTexture::CreateMaterialBUfferForFullScreen()
{
    //マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectNone] = DirectXCommon::CreateBufferResource(sizeof(MaterialForRenderTexture));
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectNone]->Map(0, nullptr, reinterpret_cast<void**>(&materialForFullScreen_));
    materialForFullScreen_->color = { 1.0f,1.0f,1.0f,1.0f };

    LogFile::Log("Rendertexture : Create : MaterialBuffer : GrayScale\n");
}

void RenderTexture::CreateMaterialBufferForGaussianFilter()
{

    //マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectGaussianFilter] = DirectXCommon::CreateBufferResource(sizeof(MaterialForGaussianFilter));
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectGaussianFilter]->Map(0, nullptr, reinterpret_cast<void**>(&materialForGaussianFilter_));
    materialForGaussianFilter_->sigma = 1.0f;
    materialForGaussianFilter_->kernel = 1;

    LogFile::Log("Rendertexture : Create : MaterialBuffer : GaussianFilter\n");
}

void RenderTexture::CreateMaterialLuminanceBasedOutline()
{
    //マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectLuminanceBasedOutline] = DirectXCommon::CreateBufferResource(sizeof(MaterialForLuminanceBasedOutline));
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectLuminanceBasedOutline]->Map(0, nullptr, reinterpret_cast<void**>(&materialForLuminanceBasedOutline_));
    materialForLuminanceBasedOutline_->weightVal = 0.0f;

    LogFile::Log("Rendertexture : Create : MaterialBuffer : LuminanceBasedOutline\n");
}

void RenderTexture::CreateMaterialDepthBasedOutline()
{

    //マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectDepthBasedOutline] = DirectXCommon::CreateBufferResource(sizeof(MaterialForDepthBasedOutline));
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectDepthBasedOutline]->Map(0, nullptr, reinterpret_cast<void**>(&materialForDepthBasedOutline_));
    materialForDepthBasedOutline_->projectionInverse = MakeIdentity4x4();
    materialForDepthBasedOutline_->lineWidth = 10000.0f;
    materialForDepthBasedOutline_->color = { 0.0f,0.0f,0.0f };

    LogFile::Log("Rendertexture : Create : MaterialBuffer : DepthBasedOutline\n");
}


void RenderTexture::CreateMaterialRadialBlur()
{

    //マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectRadialBlur] = DirectXCommon::CreateBufferResource(sizeof(MaterialForRadialBlur));
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectRadialBlur]->Map(0, nullptr, reinterpret_cast<void**>(&materialForRadialBlur_));
    materialForRadialBlur_->center = { 0.5f,0.5f };
    materialForRadialBlur_->numSamples = 1;
    materialForRadialBlur_->blurWidth = 0.01f;

    LogFile::Log("Rendertexture : Create : MaterialBuffer : RadialBlur\n");
}

void RenderTexture::CreateMaterialDissolve() {

    //マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectDissolve] = DirectXCommon::CreateBufferResource(sizeof(MaterialForDissolve));
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectDissolve]->Map(0, nullptr, reinterpret_cast<void**>(&materialForDissolve_));
    materialForDissolve_->maskVal = 1.0f;
    materialForDissolve_->rgb = { 8.0f / 255.0f, 16.0f / 255.0f,0.0f };
    LogFile::Log("Rendertexture : Create : MaterialBuffer : Dissolve\n");
}
void RenderTexture::CreateMaterialRandom()
{
    //マテリアル用のリソースを作る。
    materialResourceRandom_ = DirectXCommon::CreateBufferResource(sizeof(MaterialForDissolve));
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResourceRandom_->Map(0, nullptr, reinterpret_cast<void**>(&materialForRandom_));
    materialForRandom_->time = 1.0f;

    LogFile::Log("Rendertexture : Create : MaterialBuffer : Dissolve\n");
}
void RenderTexture::CreateMaterialThermography()
{//マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectThermography] = DirectXCommon::CreateBufferResource(sizeof(MaterialForThermography));
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectThermography]->Map(0, nullptr, reinterpret_cast<void**>(&materialForThermography_));
    materialForThermography_->alpha = { 1.0f };
    materialForThermography_->sigma = 10.0f;
    materialForThermography_->kernel = 14;
    LogFile::Log("Rendertexture : Create : MaterialBuffer : Thermography\n");
};