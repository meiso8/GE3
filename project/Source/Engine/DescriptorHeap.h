#pragma once

//ComPtr(コムポインタ)
#include<wrl.h>

#include<d3d12.h>
#include<dxgi1_6.h>

//DescriptorHeapの作成関数
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
    const Microsoft::WRL::ComPtr<ID3D12Device>& device,
    D3D12_DESCRIPTOR_HEAP_TYPE heapType,
    UINT numDescriptors,
    bool shaderVisible);