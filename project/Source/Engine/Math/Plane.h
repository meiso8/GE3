#pragma once
#include"Vector3.h"
#include"Matrix4x4.h"
#include <stdint.h>

//無限遠平面
struct Plane {
    Vector3 normal;//法線
    float distance;//距離
};

//abcが法線　dが距離  ax+by+cz = d
void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);
