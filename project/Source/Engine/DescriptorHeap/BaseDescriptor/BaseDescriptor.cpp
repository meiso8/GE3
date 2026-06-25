#include "BaseDescriptor.h"
#include"Log.h"
#include<cassert>
#include"DirectXCommon.h"

const uint32_t BaseDescriptor::kMaxCount_ = 64;
uint32_t BaseDescriptor::useIndex_ = 0;
uint32_t BaseDescriptor::descriptorSize_ = 0;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>BaseDescriptor::descriptorHeap_ = nullptr;

BaseDescriptor::~BaseDescriptor()
{
    descriptorHeap_.Reset();
    LogFile::Log("Reset descriptorHeap\n");
}

uint32_t BaseDescriptor::Allocate()
{
    assert(CanUseIndex());
    int index = useIndex_;
    useIndex_++;
    return index;
}

bool BaseDescriptor::CanUseIndex()
{
    if (useIndex_ < kMaxCount_) {
        return true;
    }

    LogFile::Log("Max Descriptor Count!\n");
    return false;
}

D3D12_CPU_DESCRIPTOR_HANDLE BaseDescriptor::GetCPUDescriptorHandle(uint32_t index)
{
    assert(CanUseIndex());
    return DirectXCommon::GetCPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor::GetGPUDescriptorHandle(uint32_t index)
{
    assert(CanUseIndex());
    return  DirectXCommon::GetGPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, index);
}

void BaseDescriptor::CreateDescriptorHeapAndSize(const D3D12_DESCRIPTOR_HEAP_TYPE& type, bool shaderVisible)
{
    //DescriptorHeapを生成する
    if (descriptorHeap_ == nullptr) {
        descriptorHeap_ = DirectXCommon::CreateDescriptorHeap(type, kMaxCount_, shaderVisible);
    }

    descriptorSize_ = DirectXCommon::GetDevice()->GetDescriptorHandleIncrementSize(type);

    LogFile::Log("Create DescriptorHeap And Size\n");
}

