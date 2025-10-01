#include"Dot.h"

//内積
float Dot(const Vector3& v1, const Vector3& v2) {
    return { v1.x * v2.x + v1.y * v2.y + v1.z * v2.z };
};

float Dot(const Vector2& v1, const Vector2& v2) {
    return { v1.x * v2.x + v1.y * v2.y };
};