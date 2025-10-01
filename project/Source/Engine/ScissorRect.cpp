#include "ScissorRect.h"

D3D12_RECT CreateScissorRect(const int32_t& width, const int32_t& height) {
    //シザー矩形
    D3D12_RECT scissorRect;

    //基本的にビューポートと同じ矩形が構成されるようにする
    scissorRect.left = 0;
    scissorRect.right = width;
    scissorRect.top = 0;
    scissorRect.bottom = height;

    return scissorRect;
};

