#pragma once
//ComPtr(コムポインタ)
#include<wrl.h>
#include<cstdint>//int32_tを使うため
#include<d3d12.h>

/// @brief StencilTextureの作成関数　奥行き
/// @param device 
/// @param width 
/// @param height 
/// @return 
Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencileTextureResource(
    const Microsoft::WRL::ComPtr<ID3D12Device>& device,
    int32_t width,
    int32_t height);


class DepthStencil {
public:
    void Create();
    D3D12_DEPTH_STENCIL_DESC& GetDesc() {
        return depthStencilDesc_;
    };
private:
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
};

