#include "RenderTexture.h"
#include"DirectXCommon.h"

#include"SrvDescriptorHeap.h"
#include"RtvDescriptorHeap.h"

#include"PSO.h"
#ifdef _DEVELOP
#include "DebugUI.h"
#endif
#include"Log.h"

void RenderTexture::Create(RtvDescriptorHeap* rtvDescriptorHeap)
{
    kRenderTargetClearValue_ = { 1.0f,0.0f,0.0f,1.0f };

    CreateResource(kNormal0, rtvDescriptorHeap, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, true);
    CreateResource(kNormal1, rtvDescriptorHeap, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, true);
    //サーモグラフィー用テクスチャ
    CreateResource(kThermography, rtvDescriptorHeap,DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, true);
    //ObjectID用テクスチャ　
    CreateResource(kObjectID, rtvDescriptorHeap,DXGI_FORMAT_R32_UINT, false);
    //ObjectID用リソース
    idReadbackResource_ = DirectXCommon::CreateReadbackBufferResource(sizeof(uint32_t));
    idReadbackResource_->SetName(L"RenderTexture_Id_ReadBackResource");
}

void RenderTexture::SetCommandListAndSrvDescriptorHeap(ID3D12GraphicsCommandList* commandList, SrvDescriptorHeap* srvDescriptorHeap)
{
    commandList_ = commandList;
    assert(commandList_);
    srvDescriptorHeap_ = srvDescriptorHeap;
    assert(srvDescriptorHeap_);
}


void RenderTexture::CreateResource(
    const uint32_t index,
    RtvDescriptorHeap* rtvDescriptorHeap,
    DXGI_FORMAT format, bool createSRV)
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
    uint32_t rtvIndex = rtvDescriptorHeap->Allocate();
    renderTextureDatas_[index].rtvHandleCPU = rtvDescriptorHeap->GetCPUDescriptorHandle(rtvIndex);
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

        renderTextureDatas_[index].srvIndex = srvDescriptorHeap_->Allocate();
        renderTextureDatas_[index].srvHandleCPU = srvDescriptorHeap_->GetCPUDescriptorHandle(renderTextureDatas_[index].srvIndex);
        renderTextureDatas_[index].srvHandleGPU = srvDescriptorHeap_->GetGPUDescriptorHandle(renderTextureDatas_[index].srvIndex);
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

void RenderTexture::Clear()
{
    // すべての ComPtr を Reset して参照を外す
    for (auto& data : renderTextureDatas_) {
        data.resource.Reset();
    }

    thermographyTextureData_.resource.Reset();
    idReadbackResource_.Reset();
}

