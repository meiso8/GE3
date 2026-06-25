#pragma once
#include"../BaseDescriptor/BaseDescriptor.h"
#include"DirectXTex.h"

class SrvDescriptorHeap :public BaseDescriptor
{
public:
    SrvDescriptorHeap();
public:
    static void PreDraw(ID3D12GraphicsCommandList* commandList);
    static ID3D12DescriptorHeap* GetDescriptorHeap() { return descriptorHeap_.Get(); }
    static void CreateSRVforTexture(uint32_t srvIndex, ID3D12Resource* pResource, DirectX::TexMetadata& metadata);
    static void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);
    static void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex, ID3D12GraphicsCommandList* commandList);
};
