#pragma once
#include"../BaseDescriptor/BaseDescriptor.h"
#include"DirectXTex.h"

class SrvDescriptorHeap :public BaseDescriptor
{
public:
    static const uint32_t kMaxSRVCount_ = 512;
public:
    SrvDescriptorHeap();
public:

    void PreDraw(ID3D12GraphicsCommandList* commandList);
    void CreateSRVforTexture(uint32_t srvIndex, ID3D12Resource* pResource, DirectX::TexMetadata& metadata);
    void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);
    void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex, ID3D12GraphicsCommandList* commandList);
};
