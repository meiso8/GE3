#pragma once
#include"Vector3.h"
#include"Matrix4x4.h"
#include <stdint.h>

//Axis Aligned Bounding Box
struct AABB {
    Vector3 min;
    Vector3 max;
};
//
//void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);