

#include "BlendState.h"


void BlendState::Create(uint32_t blendMode,uint32_t renderTargetIndex) {

    //マルチサンプル時のアンチエイリアス用アルファ処理
    blendDesc_.AlphaToCoverageEnable = FALSE;
    //複数のレンダーターゲット（MRT）で個別にブレンド設定を行うかどうか
    blendDesc_.IndependentBlendEnable = TRUE;

    //ブレンド処理
    blendDesc_.RenderTarget[renderTargetIndex].BlendEnable = (blendMode != kBlendModeNone) ? true : false;

    switch (blendMode)
    {
    case kBlendModeNone:
        break;
    case kBlendModeNormal:
        Normal(renderTargetIndex);
        break;
    case kBlendModeAdd:
        Add(renderTargetIndex);
        break;
    case kBlendModeSubtract:
        Subtract(renderTargetIndex);
        break;
    case kBlendModeMultiply:
        Multiply(renderTargetIndex);
        break;
    case kBlendModeScreen:
        Screen(renderTargetIndex);
        break;
    }

    //アルファ値の合成時、ソース側の値をそのまま使います
    blendDesc_.RenderTarget[renderTargetIndex].SrcBlendAlpha = D3D12_BLEND_ONE;
    //アルファ値の合成時、デスティネーション側は使いません
    blendDesc_.RenderTarget[renderTargetIndex].DestBlendAlpha = D3D12_BLEND_ZERO;
    //アルファ値の合成方法も「加算」にします
    blendDesc_.RenderTarget[renderTargetIndex].BlendOpAlpha = D3D12_BLEND_OP_ADD;

    //全ての色要素を書き込む
    blendDesc_.RenderTarget[renderTargetIndex].RenderTargetWriteMask =
        D3D12_COLOR_WRITE_ENABLE_ALL;
}

void BlendState::Normal(uint32_t renderTargetIndex)
{
    //カラーの合成時、ソース（描画する側）のアルファ値をブレンド係数として使います
    blendDesc_.RenderTarget[renderTargetIndex].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    //ブレンドの合成方法を「加算」にします
    blendDesc_.RenderTarget[renderTargetIndex].BlendOp = D3D12_BLEND_OP_ADD;
    //カラーの合成時、デスティネーション（既に描画済みのピクセル）の係数として「1-ソースアルファ」を使います。
    blendDesc_.RenderTarget[renderTargetIndex].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
}

void BlendState::Add(uint32_t renderTargetIndex)
{
    blendDesc_.RenderTarget[renderTargetIndex].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendDesc_.RenderTarget[renderTargetIndex].BlendOp = D3D12_BLEND_OP_ADD;
    blendDesc_.RenderTarget[renderTargetIndex].DestBlend = D3D12_BLEND_ONE;
}

void BlendState::Subtract(uint32_t renderTargetIndex)
{
    blendDesc_.RenderTarget[renderTargetIndex].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendDesc_.RenderTarget[renderTargetIndex].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
    blendDesc_.RenderTarget[renderTargetIndex].DestBlend = D3D12_BLEND_ONE;
}

void BlendState::Multiply(uint32_t renderTargetIndex)
{
    blendDesc_.RenderTarget[renderTargetIndex].SrcBlend = D3D12_BLEND_ZERO;
    blendDesc_.RenderTarget[renderTargetIndex].BlendOp = D3D12_BLEND_OP_ADD;
    blendDesc_.RenderTarget[renderTargetIndex].DestBlend = D3D12_BLEND_SRC_COLOR;
}

void BlendState::Screen(uint32_t renderTargetIndex)
{
    blendDesc_.RenderTarget[renderTargetIndex].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
    blendDesc_.RenderTarget[renderTargetIndex].BlendOp = D3D12_BLEND_OP_ADD;
    blendDesc_.RenderTarget[renderTargetIndex].DestBlend = D3D12_BLEND_ONE;
}

