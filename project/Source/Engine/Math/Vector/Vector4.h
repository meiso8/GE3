#pragma once

struct Vector4 {
    float x, y, z, w;

    bool operator==(const Vector4& other) const {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }
};

namespace COLOR{
    const Vector4 RED = { 1.0f,0.0f,0.0f,1.0f };
    const Vector4 GREEN = { 0.0f,1.0f,0.0f,1.0f };
    const Vector4 BLUE = { 0.0f,0.0f,1.0f,1.0f };
    const Vector4 CYAN = { 0.0f,1.0f,1.0f,1.0f };

    Vector4 ToShadowColor(const Vector4 color);

}

Vector4 Add(const Vector4& v1, const Vector4& v2);
Vector4 Subtract(const Vector4& v1, const Vector4& v2);
Vector4 Multiply(const float scalar, const Vector4& v);
float Dot(const Vector4& v1, const Vector4& v2);


//二項演算子 Binary Operator
Vector4 operator+(const Vector4& v1, const Vector4& v2);
Vector4 operator-(const Vector4& v1, const Vector4& v2);
Vector4 operator*(float s, const Vector4& v);
Vector4 operator+(const Vector4& v, float s);
Vector4 operator+(float s, const Vector4& v);
Vector4 operator*(const Vector4& v, float s);
Vector4 operator/(const Vector4& v, float s);

//単項演算子 Unary Operator
Vector4 operator-(const Vector4& v);
Vector4 operator+(const Vector4& v);