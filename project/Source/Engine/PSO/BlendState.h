#pragma once

#include<d3d12.h>
#include<stdint.h>

#include"BlendMode.h"

class BlendState {
public:
    void Create(uint32_t blendMode, uint32_t renderTargetIndex);
    D3D12_BLEND_DESC& GetDesc()
    {
        return blendDesc_;
    }


private:

    void Normal(uint32_t renderTargetIndex);
    void Add(uint32_t renderTargetIndex);
    void Subtract(uint32_t renderTargetIndex);
    void Multiply(uint32_t renderTargetIndex);
    void Screen(uint32_t renderTargetIndex);
    //書き込む色要素を決めることなども出来る
    D3D12_BLEND_DESC blendDesc_{};

};


