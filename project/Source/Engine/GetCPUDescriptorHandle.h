#pragma once

#include<d3d12.h>
#include<cstdint>//int32_tを使うため

//インデックスのDescriptorHandleを取得する関数
D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);