#pragma once
#include<d3d12.h>
#include<stdint.h>

class DescriptorHandle {

public:

  /// @brief 指定したディスクリプタヒープ内の指定インデックスに対応するCPUディスクリプタハンドルを取得。
  /// @param descriptorHeap 有効なID3D12DescriptorHeapへのポインタ。
  /// @param descriptorSize ディスクリプタ間のバイト単位のオフセット（ディスクリプタサイズ）。
  /// @param index ヒープ内の取得対象ディスクリプタのインデックス。
  /// @return 指定したインデックスに対応するD3D12_CPU_DESCRIPTOR_HANDLE。
    static D3D12_CPU_DESCRIPTOR_HANDLE
        GetCPUDescriptorHandle(
            ID3D12DescriptorHeap* descriptorHeap,
            uint32_t descriptorSize,
            uint32_t index
        );

    /// @brief 指定したディスクリプタヒープ内の指定インデックスに対応するGPUディスクリプタハンドルを取得。
    /// @param descriptorHeap 有効なID3D12DescriptorHeapへのポインタ。
    /// @param descriptorSize  ディスクリプタ間のバイト単位のオフセット（ディスクリプタサイズ）。
    /// @param index ヒープ内の取得対象ディスクリプタのインデックス。
    /// @return 指定したインデックスに対応するD3D12_GPU_DESCRIPTOR_HANDLE。
    static D3D12_GPU_DESCRIPTOR_HANDLE
        GetGPUDescriptorHandle(
            ID3D12DescriptorHeap* descriptorHeap,
            uint32_t descriptorSize,
            uint32_t index);

  

};

