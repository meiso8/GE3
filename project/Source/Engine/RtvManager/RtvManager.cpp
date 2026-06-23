#include "RtvManager.h"
#include"DirectXCommon.h"
#include"Log.h"

const uint32_t RtvManager::kMaxRTVCount_ = 64;
uint32_t RtvManager::useIndex_ = 0;
uint32_t RtvManager::descriptorSize_ = 0;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>RtvManager::descriptorHeap_ = nullptr;

bool RtvManager::IsMaxCount()
{

    if (useIndex_ < kMaxRTVCount_) {
        return true;
    }

    return false;

}

void RtvManager::Initialize()

{
    //DescriptorHeapを生成する
    if (descriptorHeap_ == nullptr) {
        descriptorHeap_ = DirectXCommon::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kMaxRTVCount_, false);
        LogFile::Log("Create RTV DescriptorHeap");
    }

    //DescriptorSizeを取得しておく
    descriptorSize_ = DirectXCommon::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

}

RtvManager::~RtvManager()
{
    descriptorHeap_.Reset();
}

uint32_t RtvManager::Allocate()
{
    assert(useIndex_ < kMaxRTVCount_);
    int index = useIndex_;
    useIndex_++;
    return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE RtvManager::GetCPUDescriptorHandle(uint32_t index)
{
    assert(IsMaxCount());
    return DirectXCommon::GetCPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE RtvManager::GetGPUDescriptorHandle(uint32_t index)
{
    assert(IsMaxCount());
    return  DirectXCommon::GetGPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, index);
}
