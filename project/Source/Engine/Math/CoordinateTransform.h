#pragma once

#include "Matrix4x4.h"
#include"Vector3.h"

//座標変換関数
Vector3 CoordinateTransform(const Vector3& vector, const Matrix4x4& matrix);
