#include"Vector4.h"

#include<algorithm>
Vector4 Add(const Vector4& v1, const Vector4& v2) {
    return { v1.x + v2.x,v1.y + v2.y ,v1.z + v2.z ,v1.w + v2.w };
};


Vector4 Subtract(const Vector4& v1, const Vector4& v2) {
    return { v1.x - v2.x,v1.y - v2.y ,v1.z - v2.z,v1.w - v2.w };
};


Vector4 Multiply(const float scalar, const Vector4& v) {
    return { scalar * v.x,scalar * v.y,scalar * v.z,scalar * v.w };
}
float Dot(const Vector4& v1, const Vector4& v2)
{
    return { v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w };
}

//二項演算子 Binary Operator
Vector4 operator+(const Vector4& v1, const Vector4& v2) { return Add(v1, v2); }
Vector4 operator-(const Vector4& v1, const Vector4& v2) { return Subtract(v1, v2); }
Vector4 operator*(float s, const Vector4& v) { return Multiply(s, v); }
Vector4 operator+(const Vector4& v, float s) { return { v.x + s,v.y + s,v.z + s ,s + v.w }; };
Vector4 operator+(float s, const Vector4& v) { return  v + s; };
Vector4 operator*(const Vector4& v, float s) { return s * v; }
Vector4 operator/(const Vector4& v, float s) { return Multiply(1.0f / s, v); }

//単項演算子 Unary Operator
Vector4 operator-(const Vector4& v) { return { -v.x,-v.y,-v.z ,-v.w }; }
Vector4 operator+(const Vector4& v) { return v; }

Vector4 COLOR::ToShadowColor(const Vector4 color)
{
    Vector4 newColor = { 0.0f };
    newColor = color - Vector4{ 0.5f,0.5f,0.5f,0.0f };
    newColor.x = std::clamp(newColor.x, 0.0f, 1.0f);
    newColor.y = std::clamp(newColor.y, 0.0f, 1.0f);
    newColor.z = std::clamp(newColor.z, 0.0f, 1.0f);
    newColor.w = std::clamp(newColor.w, 0.0f, 1.0f);
    return newColor;
}
