#include "SrvDescriptorHeap.h"
#include"DirectXCommon.h"
#include"Log.h"

CbvSrvUavDescriptorHeap::CbvSrvUavDescriptorHeap()
{
    CreateDescriptorHeapAndSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true, kMaxCount_);
}

void CbvSrvUavDescriptorHeap::PreDraw(ID3D12GraphicsCommandList* commandList)
{    //描画用のDescriptorHeapの設定
    ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap_.Get() };
    commandList->SetDescriptorHeaps(1, descriptorHeaps);
}

void CbvSrvUavDescriptorHeap::CreateSRVforTexture(uint32_t index, ID3D12Resource* pResource, DirectX::TexMetadata& metadata)
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
    DirectXCommon::GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(index));
}



void CbvSrvUavDescriptorHeap::CreateSRVforStructuredBuffer(uint32_t index, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    srvDesc.Buffer.NumElements = numElements;
    srvDesc.Buffer.StructureByteStride = structureByteStride;

    DirectXCommon::GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(index));
}

void CbvSrvUavDescriptorHeap::CreateUAV(uint32_t index, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride)
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = numElements;//頂点数
    uavDesc.Buffer.CounterOffsetInBytes = 0;
    uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
    uavDesc.Buffer.StructureByteStride = structureByteStride;
    //UAVの作成
    DirectXCommon::GetDevice()->CreateUnorderedAccessView(pResource, nullptr, &uavDesc, GetCPUDescriptorHandle(index));

}
void CbvSrvUavDescriptorHeap::SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t index, ID3D12GraphicsCommandList* commandList)
{
    commandList->SetGraphicsRootDescriptorTable(RootParameterIndex, GetGPUDescriptorHandle(index));
}
void CbvSrvUavDescriptorHeap::SetComputeRootDescriptorTable(UINT RootParameterIndex, uint32_t index, ID3D12GraphicsCommandList* commandList)
{
    commandList->SetComputeRootDescriptorTable(RootParameterIndex, GetGPUDescriptorHandle(index));
}

