#pragma once

#include<d3d12.h>
#include<wrl.h>
#include<stdint.h>

class BaseDescriptor
{
protected:
   Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>descriptorHeap_ = nullptr;
    uint32_t descriptorSize_ = 0;
    uint32_t useIndex_ = 0;
    uint32_t maxCount_ = 1;
public:
    BaseDescriptor();
    ~BaseDescriptor();
    /// @brief アロケータ
    /// @return 空いているインデックス
    uint32_t Allocate();
    /// @brief 最大数かどうかを取得する関数
    /// @return 最大数かどうか
    bool CanUseIndex();
    /// @brief RTVのCPUディスクリプタハンドルの取得関数
    /// @param index 
    /// @return RTVのCPUディスクリプタハンドル
   D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
    /// @brief RTVのGPUディスクリプタハンドルの取得関数
    /// @param index 
    /// @return RTVのGPUディスクリプタハンドル
   D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);
   ID3D12DescriptorHeap* GetDescriptorHeap() { return descriptorHeap_.Get(); };
protected:
        void CreateDescriptorHeapAndSize(
        const D3D12_DESCRIPTOR_HEAP_TYPE& type =
        D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        bool shaderVisible = true,
        const uint32_t kMaxCount = 1
    );
};

