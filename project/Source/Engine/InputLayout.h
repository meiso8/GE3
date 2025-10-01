#pragma once

#include<d3d12.h>


class InputLayout
{
public:
    void Create();
    D3D12_INPUT_LAYOUT_DESC& GetDesc() {
        return inputLayoutDesc_;
   };
private:
    D3D12_INPUT_ELEMENT_DESC inputElementDescs_[3] = {};
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};
};

