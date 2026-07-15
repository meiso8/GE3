#include "DepthTexture.h"
#include"DirectXCommon.h"
#include"Log.h"
#include"DebugUI.h"
#include"DsvDescriptorHeap.h"
#include"CbvSrvUavDescriptorHeap.h"

void DepthTexture::CreateResource(CbvSrvUavDescriptorHeap* srvDescriptorHeap,int32_t width,int32_t height)
{

    //stencileTextureResourceの作成 
    if (depthTextureData_.resource == nullptr) {
        depthTextureData_.resource = ResourceFactory::CreateDepthStencileTextureResource(width, height);
        LogFile::Log("CreateDepthBuffer\n");
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC depthTextureSrvDesc{};
    depthTextureSrvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    depthTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    depthTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    depthTextureSrvDesc.Texture2D.MipLevels = 1;

    depthTextureData_.srvIndex = srvDescriptorHeap->Allocate();

    DirectXCommon::GetDevice()->CreateShaderResourceView(
        depthTextureData_.Get(),
        &depthTextureSrvDesc,
        srvDescriptorHeap->GetCPUDescriptorHandle(depthTextureData_.srvIndex)
    );

    depthTextureData_.resource->SetName(L"depthTextureData_depthStencilResource");

    LogFile::Log("DepthTextureResource : CreateShaderResourceView\n");

  
}

void DepthTexture::InitializeDepthStencilView(DsvDescriptorHeap* dsvDescriptorHeap)
{
    //DSVの設定 DepthStencilView
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//基本的にはResourceに合わせる。
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2dTexture
    // DSVHeapの先頭にDSVを作る
//Deviceが出ちゃってるので何とかする
    DirectXCommon::GetDevice()->CreateDepthStencilView(depthTextureData_.resource.Get(), &dsvDesc, dsvDescriptorHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

    LogFile::Log("Initialize DepthStencilView\n");
}

void DepthTexture::DebugViewer(CbvSrvUavDescriptorHeap* srvDescriptorHeap)
{
#ifdef USE_IMGUI
    ImGui::Begin("PostEffect Viewer");
    // 例：表示したいSRVのインデックス番号
    DebugUI::CheckSRVTexture(depthTextureData_.srvIndex, srvDescriptorHeap);
    ImGui::End();

#endif
}
