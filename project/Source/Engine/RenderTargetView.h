#pragma once

#include<d3d12.h>
#include<cstdint>//int32_tを使うため
//ComPtr(コムポインタ)
#include<wrl.h>


class RenderTargetView
{
public:
    void Create(
        const Microsoft::WRL::ComPtr<ID3D12Device>& device,
        const Microsoft::WRL::ComPtr <ID3D12Resource>(&swapChainResources)[2],
        const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& rtvDescriptorHeap);
    D3D12_RENDER_TARGET_VIEW_DESC& GetDesc() {return rtvDesc_ ;  }
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetHandle(size_t index) const;
private:
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
    //RTVを2つ作るのでディスクリプタを2つ用意
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2]{};

};

