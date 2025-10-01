#pragma once
#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"
#include "Matrix4x4.h"

//スカラー倍
Vector2 Multiply(const float scalar, const Vector2& v);

Vector3 Multiply(const float scalar, const Vector3& v);
Vector4 Multiply(const float scalar, const Vector4& v);

//行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);