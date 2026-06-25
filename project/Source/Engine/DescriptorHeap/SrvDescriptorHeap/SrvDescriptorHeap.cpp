#include "SrvDescriptorHeap.h"
#include"DirectXCommon.h"
#include"Log.h"

uint32_t SrvDescriptorHeap::useIndex_ = 0;
const uint32_t SrvDescriptorHeap::kMaxCount_ = 512;
uint32_t SrvDescriptorHeap::descriptorSize_ = 0;

SrvDescriptorHeap::SrvDescriptorHeap()
{
    CreateDescriptorHeapAndSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);
}

void SrvDescriptorHeap::PreDraw(ID3D12GraphicsCommandList* commandList)
{    //描画用のDescriptorHeapの設定
    ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap_.Get() };
    commandList->SetDescriptorHeaps(1, descriptorHeaps);
}

void SrvDescriptorHeap::CreateSRVforTexture(uint32_t srvIndex, ID3D12Resource* pResource, DirectX::TexMetadata& metadata)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    if (metadata.IsCubemap()) {
        //Cubemapであるかどうかで処理を変更する
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.TextureCube.MostDetailedMip = 0;
        srvDesc.TextureCube.MipLevels = UINT_MAX;
        srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

    } else {
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//texture
        srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
    }

    //SRVの生成
    DirectXCommon::GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

void SrvDescriptorHeap::CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    srvDesc.Buffer.NumElements = numElements;
    srvDesc.Buffer.StructureByteStride = structureByteStride;

    DirectXCommon::GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

void SrvDescriptorHeap::SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex, ID3D12GraphicsCommandList* commandList)
{
    commandList->SetGraphicsRootDescriptorTable(RootParameterIndex, GetGPUDescriptorHandle(srvIndex));
}
