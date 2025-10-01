#include "ShaderResourceView.h"
#include"GetCPUDescriptorHandle.h"
#include"GetGPUDescriptorHandle.h"
#include"Texture.h"

void ShaderResourceView::Create(
    Texture& texture,
    uint32_t index,
    const Microsoft::WRL::ComPtr<ID3D12Device>& device,
    const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& srvDescriptorHeap) {

    const uint32_t descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    //metaDataを基にSRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format =texture.GetMetadata().format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//texture
    srvDesc.Texture2D.MipLevels = UINT(texture.GetMetadata().mipLevels);

    //SRVを作成するDescriptorHeapの場所の選択
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap.Get(), descriptorSize, index);
    textureSrvHandleGPU_ = GetGPUDescriptorHandle(srvDescriptorHeap.Get(), descriptorSize, index);

    //SRVの生成
    device->CreateShaderResourceView(texture.GetTextureResource().Get(), &srvDesc, textureSrvHandleCPU);

}


