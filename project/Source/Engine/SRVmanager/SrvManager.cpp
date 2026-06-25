#include "SrvManager.h"
#include"DirectXCommon.h"
#include"CommandList.h"
#include"Log.h"
#include<cassert>
using namespace Microsoft::WRL;

uint32_t SrvManager::useIndex = 0;
const uint32_t SrvManager::kMaxSRVCount = 512;
uint32_t SrvManager::descriptorSize = 0;

ComPtr<ID3D12DescriptorHeap> SrvManager::descriptorHeap = nullptr;


bool SrvManager::CanCreateSRV()
{

    if (useIndex < kMaxSRVCount) {

        return true;
    }

    LogFile::Log("SrvManager MaxSRVCount!!\n");
    return false;

}

void SrvManager::PreDraw(ID3D12GraphicsCommandList* commandList)
{
    //描画用のDescriptorHeapの設定
    ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap.Get() };
    commandList->SetDescriptorHeaps(1, descriptorHeaps);
}

uint32_t SrvManager::Allocate()
{
    assert(CanCreateSRV());
    int index = useIndex;
    useIndex++;
    return index;
}

SrvManager::SrvManager()
{   
    //SRV　SRVやCBV用のDescriptorHeapは一旦ゲーム中に一つだけ
    if (descriptorHeap == nullptr) {
        descriptorHeap = DirectXCommon::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount, true);
    }

    descriptorSize = DirectXCommon::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    LogFile::Log("Create SrvManager\n");
}

SrvManager::~SrvManager()
{
    descriptorHeap.Reset();
    LogFile::Log("SrvManager Reset descriptorHeap \n");
}

D3D12_CPU_DESCRIPTOR_HANDLE SrvManager::GetCPUDescriptorHandle(uint32_t index)
{
    assert(CanCreateSRV());
    return DirectXCommon::GetCPUDescriptorHandle(descriptorHeap.Get(), descriptorSize, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE SrvManager::GetGPUDescriptorHandle(uint32_t index)
{
    assert(CanCreateSRV());
    return DirectXCommon::GetGPUDescriptorHandle(descriptorHeap.Get(), descriptorSize, index);
}

void SrvManager::CreateSRVforTexture(uint32_t srvIndex, ID3D12Resource* pResource, DirectX::TexMetadata& metadata)
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

void SrvManager::CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride)
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

void SrvManager::SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex, ID3D12GraphicsCommandList* commandList)
{
    commandList->SetGraphicsRootDescriptorTable(RootParameterIndex, GetGPUDescriptorHandle(srvIndex));
}
