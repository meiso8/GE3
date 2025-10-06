#include "Viewport.h"

//ビューポート

D3D12_VIEWPORT CreateViewport(const float& width, const float& height) {

    D3D12_VIEWPORT viewport;

    //クライアント領域のサイズと一緒にして画面全体に表示 , windowClass.GetClientHeight()
    viewport.Width = width;
    viewport.Height = height;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    return viewport;
}

