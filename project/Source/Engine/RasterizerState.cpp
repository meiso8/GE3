#include "RasterizerState.h"


void RasterizerState::Create(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode) {
    //三角形の内部をピクセルに分解して、PixelShaderを起動する

    rasterizerDesc_.CullMode = cullMode;

    rasterizerDesc_.FillMode = fillMode;

}

