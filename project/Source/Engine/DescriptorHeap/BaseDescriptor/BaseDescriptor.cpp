#include "BaseDescriptor.h"
#include"Log.h"
#include<cassert>
#include"DescriptorHandle/DescriptorHandle.h"
#include"DirectXCommon.h"

BaseDescriptor::BaseDescriptor()
{
}

BaseDescriptor::~BaseDescriptor()
{
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
    if (useIndex_ < maxCount_) {
        return true;
    }

    LogFile::Log("Max Descriptor Count!\n");
    return false;
}

D3D12_CPU_DESCRIPTOR_HANDLE BaseDescriptor::GetCPUDescriptorHandle(uint32_t index)
{
    assert(CanUseIndex());
    return DescriptorHandle::GetCPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor::GetGPUDescriptorHandle(uint32_t index)
{
    assert(CanUseIndex());
    return  DescriptorHandle::GetGPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, index);
}

void BaseDescriptor::CreateDescriptorHeapAndSize(const D3D12_DESCRIPTOR_HEAP_TYPE& type, bool shaderVisible,const uint32_t kMaxCount)
{

    maxCount_ = kMaxCount;

    //DescriptorHeapを生成する
    if (descriptorHeap_ == nullptr) {
        descriptorHeap_ = DirectXCommon::CreateDescriptorHeap(type, maxCount_, shaderVisible);
    }

    descriptorSize_ = DirectXCommon::GetDevice()->GetDescriptorHandleIncrementSize(type);

    LogFile::Log("Create DescriptorHeap And Size\n");
}

