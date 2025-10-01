#pragma once

struct Vector3 {
    float x, y, z;

    //複合代入演算子Compound Assignement Operator
    Vector3& operator+=(float s) { x += s; y += s, z += s; return *this; }
    Vector3& operator-=(const Vector3& v) { x -= v.x, y -= v.y, z -= v.z; return *this; }
    Vector3& operator+=(const Vector3& v) { x += v.x, y += v.y, z += v.z; return *this; }
    Vector3& operator/=(float s) { x /= s, y /= s, z /= s; return*this; }

};

//二項演算子 Binary Operator
Vector3 operator+(const Vector3& v1, const Vector3& v2);
Vector3 operator-(const Vector3& v1, const Vector3& v2);
Vector3 operator*(float s, const Vector3& v);
Vector3 operator+(const Vector3& v, float s);
Vector3 operator+(float s, const Vector3& v);
Vector3 operator*(const Vector3& v, float s);
Vector3 operator/(const Vector3& v, float s);

//単項演算子 Unary Operator
Vector3 operator-(const Vector3& v);
Vector3 operator+(const Vector3& v);
