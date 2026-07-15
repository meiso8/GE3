#pragma once
#include"../BaseDescriptor/BaseDescriptor.h"
#include"DirectXTex.h"

class CbvSrvUavDescriptorHeap :public BaseDescriptor
{
public:
    static const uint32_t kMaxCount_ = 512;
public:
    CbvSrvUavDescriptorHeap();
public:

    void PreDraw(ID3D12GraphicsCommandList* commandList);
    void CreateSRVforTexture(uint32_t index, ID3D12Resource* pResource, DirectX::TexMetadata& metadata);
    void CreateSRVforStructuredBuffer(uint32_t index, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);
    void CreateUAV(uint32_t index, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);
    void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t index, ID3D12GraphicsCommandList* commandList);
    void SetComputeRootDescriptorTable(UINT RootParameterIndex, uint32_t index, ID3D12GraphicsCommandList* commandList);
};
