#include "RasterizerState.h"

void RasterizerState::Create(uint32_t cullMode, uint32_t fillMode) {
    //三角形の内部をピクセルに分解して、PixelShaderを起動する


    switch (cullMode)
    {
    case kCullModeNone:
        rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
        break;
    case kCullModeFront:
        rasterizerDesc_.CullMode = D3D12_CULL_MODE_FRONT;
        break;
    case kCullModeBack:
        rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
        break;

    }

    if (fillMode == kFillModeSolid) {
        rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

    } else {
        rasterizerDesc_.FillMode = D3D12_FILL_MODE_WIREFRAME;
    }


}

