#pragma once
#include"Vector4.h"
#include"Matrix4x4.h"
#include<cstdint>

struct Material {
    Vector4 color;
    int32_t lightType;
    float padding[3];
    Matrix4x4 uvTransform;
};

