

#include "BlendState.h"


void BlendState::Create(uint32_t blendMode) {

    //マルチサンプル時のアンチエイリアス用アルファ処理
    blendDesc_.AlphaToCoverageEnable = FALSE;
    //複数のレンダーターゲット（MRT）で個別にブレンド設定を行うかどうか
    blendDesc_.IndependentBlendEnable = FALSE;

    //ブレンド処理
    blendDesc_.RenderTarget[0].BlendEnable = (blendMode != kBlendModeNone) ? true : false;

    switch (blendMode)
    {
    case kBlendModeNone:
        break;
    case kBlendModeNormal:
        Normal();
        break;
    case kBlendModeAdd:
        Add();
        break;
    case kBlendModeSubtract:
        Subtract();
        break;
    case kBlendModeMultiply:
        Multiply();
        break;
    case kBlendModeScreen:
        Screen();
        break;
    }


    //アルファ値の合成時、ソース側の値をそのまま使います
    blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    //アルファ値の合成時、デスティネーション側は使いません
    blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    //アルファ値の合成方法も「加算」にします
    blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

    //全ての色要素を書き込む
    blendDesc_.RenderTarget[0].RenderTargetWriteMask =
        D3D12_COLOR_WRITE_ENABLE_ALL;




}

void BlendState::Normal()
{
    //カラーの合成時、ソース（描画する側）のアルファ値をブレンド係数として使います
    blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    //ブレンドの合成方法を「加算」にします
    blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    //カラーの合成時、デスティネーション（既に描画済みのピクセル）の係数として「1-ソースアルファ」を使います。
    blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
}

void BlendState::Add()
{
    blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
}

void BlendState::Subtract()
{
    blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
    blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
}

void BlendState::Multiply()
{
    blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
    blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
}

void BlendState::Screen()
{
    blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
    blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
}

