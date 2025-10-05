#pragma once
#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"
//float Lerp(float start, float end, float t);
//Vector3 Lerp(Vector3 start, Vector3 end, float t);
//Vector4 Lerp(Vector4 start, Vector4 end, float t);
template <typename T> T Lerp(T start, T end, float t) {
	return static_cast<T>(start * (1.0f - t) + end * t);
}
Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);