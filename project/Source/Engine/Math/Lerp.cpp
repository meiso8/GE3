#include "Lerp.h"
#include"Normalize.h"
#include"Dot.h"
#include<cmath>

float Lerp(float start, float end, float t) {

	return { start * (1.0f - t) + end * t };

};

//Vector2 Lerp(Vector2 start, Vector2 end, float t) {
//
//	return { start * (1.0f - t) + end * t };
//
//};

Vector3 Lerp(Vector3 start, Vector3 end, float t) {

    return { start * (1.0f - t) + end * t };

};

Vector4 Lerp(Vector4 start, Vector4 end, float t) {

    return { start * (1.0f - t) + end * t };

};

Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t) {

	Vector3 normalizeV1 = Normalize(v1);
	Vector3 normalizeV2 = Normalize(v2);
	float dot = Dot(normalizeV1, normalizeV2);
	//dot = std::min(dot, 1.0f);
	float phi = std::acosf(dot);

	return std::sinf((1.0f - t) * phi) / std::sinf(phi) * normalizeV1 + std::sinf(t * phi) / std::sinf(phi) * normalizeV2;
};