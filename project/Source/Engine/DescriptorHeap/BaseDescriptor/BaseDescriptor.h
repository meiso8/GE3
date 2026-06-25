#pragma once

#include<d3d12.h>
#include<wrl.h>
#include<stdint.h>

class BaseDescriptor
{
public:
    static const uint32_t kMaxCount_;
protected:
    static  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>descriptorHeap_;
private:
    static uint32_t descriptorSize_;
    static uint32_t useIndex_;
public:
    BaseDescriptor() = default;
    ~BaseDescriptor();
    /// @brief アロケータ
    /// @return 空いているインデックス
    static uint32_t Allocate();
    /// @brief 最大数かどうかを取得する関数
    /// @return 最大数かどうか
    static bool CanUseIndex();
    /// @brief RTVのCPUディスクリプタハンドルの取得関数
    /// @param index 
    /// @return RTVのCPUディスクリプタハンドル
    static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
    /// @brief RTVのGPUディスクリプタハンドルの取得関数
    /// @param index 
    /// @return RTVのGPUディスクリプタハンドル
    static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);
protected:
    virtual void CreateDescriptorHeapAndSize(
        const D3D12_DESCRIPTOR_HEAP_TYPE& type = 
        D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        bool shaderVisible  = true) = 0;
};

