#pragma once

#include<d3d12.h>


class InputLayout
{
public:
    void Create();
    D3D12_INPUT_LAYOUT_DESC& GetDesc(size_t index) {
        return inputLayoutDesc_[index];
   };
private:
    D3D12_INPUT_ELEMENT_DESC inputElementDescs_[3] = {};
    D3D12_INPUT_ELEMENT_DESC inputElementDescs2_[2] = {};
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_[2]{};
};

