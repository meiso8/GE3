#pragma once
#include"Vector3.h"

//t（媒介変数）の値によって当たっているか判定　origin+td;
//直線 無限に続く線 -∞<= t >= ∞　
struct Line {
    Vector3 origin;//!<始点
    Vector3 diff;//!<終点への差分ベクトル
};
