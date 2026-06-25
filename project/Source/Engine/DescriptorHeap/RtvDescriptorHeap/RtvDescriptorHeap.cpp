#include "RtvDescriptorHeap.h"
#include"Log.h"

const uint32_t RtvDescriptorHeap::kMaxCount_ = 64;
uint32_t RtvDescriptorHeap::useIndex_ = 0;
uint32_t RtvDescriptorHeap::descriptorSize_ = 0;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>RtvDescriptorHeap::descriptorHeap_ = nullptr;

RtvDescriptorHeap::RtvDescriptorHeap()
{
    CreateDescriptorHeapAndSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, false);
    LogFile::Log("Create　Rtv Descriptor Heap\n");
}
