#pragma once
#include"Vector3.h"

//波の頂点アニメーション用
struct Wave
{
    Vector3 direction;
    float padding;
    float time; // アニメーション用の時間変数
    float amplitude; //振幅
    float frequency;//周期
};

