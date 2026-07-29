#include"Depth.h"

void DepthStencil::Create(MaskMode maskMode) {
    
    
    //Depthの機能を有効化する
        if (maskMode == MaskMode::kZero) {
            //書き込みします
            depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
            depthStencilDesc_.DepthEnable = true;
        } else if (maskMode == MaskMode::kAll) {
            //書き込みします
            depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
            depthStencilDesc_.DepthEnable = true;
        } else if (maskMode == MaskMode::kNone) {
            depthStencilDesc_.DepthEnable = false;
        } else if (maskMode == MaskMode::kZero_Depth_Stencil_Enable) {
            depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
            depthStencilDesc_.DepthEnable = FALSE;
            depthStencilDesc_.StencilEnable = FALSE;
        }
        if (depthStencilDesc_.DepthEnable) {
            //比較関数はLessEqual。つまり、近ければ描画される
            depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        }

  
};
