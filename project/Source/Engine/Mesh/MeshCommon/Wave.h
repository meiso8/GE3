#pragma once
#include"hlslTypeToCpp.h"

//波の頂点アニメーション用
struct Wave
{
    float3 direction;
    float time; // アニメーション用の時間変数
    float amplitude; //振幅
    float frequency;//周期
};
