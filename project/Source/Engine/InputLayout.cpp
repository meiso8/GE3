#include "InputLayout.h"


void InputLayout::Create() {

    inputElementDescs_[0].SemanticName = "POSITION";
    inputElementDescs_[0].SemanticIndex = 0;
    inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//RGBA
    inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs_[1].SemanticName = "TEXCOORD";
    inputElementDescs_[1].SemanticIndex = 0;
    inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;//Vector2のためRG
    inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs_[2].SemanticName = "NORMAL";
    inputElementDescs_[2].SemanticIndex = 0;
    inputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;//RGB
    inputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputLayoutDesc_[0].pInputElementDescs = inputElementDescs_;
    inputLayoutDesc_[0].NumElements = _countof(inputElementDescs_);


    inputElementDescs2_[0].SemanticName = "POSITION";
    inputElementDescs2_[0].SemanticIndex = 0;
    inputElementDescs2_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//RGBA
    inputElementDescs2_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
                      
    inputElementDescs2_[1].SemanticName = "NORMAL";
    inputElementDescs2_[1].SemanticIndex = 0;
    inputElementDescs2_[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;//RGB
    inputElementDescs2_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputLayoutDesc_[1].pInputElementDescs = inputElementDescs2_;
    inputLayoutDesc_[1].NumElements = _countof(inputElementDescs2_);


}