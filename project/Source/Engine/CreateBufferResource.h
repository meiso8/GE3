#pragma once

#include<d3d12.h>
//ComPtr(コムポインタ)
#include<wrl.h>

//Resource作成の関数化
Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
    const Microsoft::WRL::ComPtr<ID3D12Device>& device,
    size_t sizeInBytes);