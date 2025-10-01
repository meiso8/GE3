#pragma once
#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"
#include"Matrix4x4.h"



Vector2 Add(const Vector2& v1, const Vector2& v2);

//加算
Vector3 Add(const Vector3& v1, const Vector3& v2);
Vector4 Add(const Vector4& v1, const Vector4& v2);



//行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);