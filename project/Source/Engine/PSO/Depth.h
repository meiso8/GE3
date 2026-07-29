#pragma once
//ComPtr(コムポインタ)
#include<wrl.h>
#include<cstdint>//int32_tを使うため
#include<d3d12.h>

enum MaskMode {
    kZero,
    kAll,
    kNone,
    kZero_Depth_Stencil_Enable,
    kMasks
};




class DepthStencil {
public:

    void Create(MaskMode maskMode);
    D3D12_DEPTH_STENCIL_DESC& GetDesc() {
        return depthStencilDesc_;
    };
private:
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
};

