#pragma once
#include"Vector3.h"
#include"Vector2.h"

//正規化
Vector3 Normalize(const Vector3& v);
Vector2 Normalize(const Vector2& v);

//template<typename Type> Type Normalize(Type& v);