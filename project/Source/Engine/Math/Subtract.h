#pragma once
#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"
#include"Matrix4x4.h"

//ベクトルの減算
//ベクトル減算
Vector2 Subtract(const Vector2& v1, const Vector2& v2);
Vector3 Subtract(const Vector3& v1, const Vector3& v2);
Vector4 Subtract(const Vector4& v1, const Vector4& v2);
//行列の減算
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);