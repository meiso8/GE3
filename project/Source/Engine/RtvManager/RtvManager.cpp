#include "RtvManager.h"
#include"DirectXCommon.h"
#include"Log.h"

const uint32_t RtvManager::kMaxRTVCount_ = 64;
uint32_t RtvManager::useIndex_ = 0;
uint32_t RtvManager::descriptorSize_ = 0;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>RtvManager::descriptorHeap_ = nullptr;

bool RtvManager::CanCreateRTV()
{
    if (useIndex_ < kMaxRTVCount_) {
        return true;
    }

    LogFile::Log("RtvManager IsMaxRTVCount!!\n");
    return false;

}

RtvManager::RtvManager()
{    
    
    //DescriptorHeapを生成する
    if (descriptorHeap_ == nullptr) {
        descriptorHeap_ = DirectXCommon::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kMaxRTVCount_, false);
    }

    //DescriptorSizeを取得しておく
    descriptorSize_ = DirectXCommon::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    LogFile::Log("Create　RtvManager\n");
}

RtvManager::~RtvManager()
{
    descriptorHeap_.Reset();

    LogFile::Log("RtvManager　Reset　descriptorHeap\n");
}

uint32_t RtvManager::Allocate()
{
    assert(CanCreateRTV());
    int index = useIndex_;
    useIndex_++;
    return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE RtvManager::GetCPUDescriptorHandle(uint32_t index)
{
    assert(CanCreateRTV());
    return DirectXCommon::GetCPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE RtvManager::GetGPUDescriptorHandle(uint32_t index)
{
    assert(CanCreateRTV());
    return  DirectXCommon::GetGPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, index);
}
