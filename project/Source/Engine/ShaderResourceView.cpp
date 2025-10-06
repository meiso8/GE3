#include "ShaderResourceView.h"
#include"Texture.h"
#include"DirectXCommon.h"
#include<cassert>

void ShaderResourceView::Create(
    Texture& texture,
    uint32_t index) {

    //metaDataを基にSRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format =texture.GetMetadata().format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//texture
    srvDesc.Texture2D.MipLevels = UINT(texture.GetMetadata().mipLevels);

    //SRVを作成するDescriptorHeapの場所の選択
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU =  DirectXCommon::GetSRVCPUDescriptorHandle(index);
    textureSrvHandleGPU_ = DirectXCommon::GetSRVGPUDescriptorHandle(index);

    //SRVの生成
    DirectXCommon::GetDevice()->CreateShaderResourceView(texture.GetTextureResource().Get(), &srvDesc, textureSrvHandleCPU);

}


