#pragma once

#include<d3d12.h>
#include<wrl.h>
#include<stdint.h>
#include"DirectXTex.h"


class SrvManager
{
public:
    static const uint32_t kMaxSRVCount;
private:
    static uint32_t useIndex;
    static uint32_t descriptorSize;
    //ゲームに一つだけ
    static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;

public:
    void Initialize();
    ~SrvManager();
    static void PreDraw();
    static uint32_t Allocate();
    static bool IsMaxCount();

    static ID3D12DescriptorHeap* GetDescriptorHeap() { return descriptorHeap.Get(); }
    static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
    static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

    static void CreateSRVforTexture(uint32_t srvIndex, ID3D12Resource* pResource, DirectX::TexMetadata& metadata);
    static void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource,UINT numElements, UINT structureByteStride);
    static void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);
};

