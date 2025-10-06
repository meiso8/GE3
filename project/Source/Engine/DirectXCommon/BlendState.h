#pragma once

#include<d3d12.h>
#include<stdint.h>

enum BlendMode {
    //ブレンドなし
    kBlendModeNone,
    //通常ブレンド
    kBlendModeNormal,
    //加算
    kBlendModeAdd,
    //減算
    kBlendModeSubtract,
    //乗算
    kBlendModeMultiply,
    //スクリーン
    kBlendModeScreen,
    //カウント用　利用禁止
    kCountOfBlendMode,
};


class BlendState {
public:
    void Create(uint32_t blendMode);
    D3D12_BLEND_DESC& GetDesc()
    {
        return blendDesc_;
    }


private:

    void Normal();
    void Add();
    void Subtract();
    void Multiply();
    void Screen();
    //書き込む色要素を決めることなども出来る
    D3D12_BLEND_DESC blendDesc_{};

};


