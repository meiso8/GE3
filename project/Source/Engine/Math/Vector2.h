#pragma once

struct Vector2 {
    float x, y;

    //複合代入演算子Compound Assignement Operator
    Vector2& operator+=(float s) { x += s; y += s; return *this; }
    Vector2& operator-=(const Vector2& v) { x -= v.x, y -= v.y; return *this; }
    Vector2& operator+=(const Vector2& v) { x += v.x, y += v.y; return *this; }
    Vector2& operator/=(float s) { x /= s, y /= s; return*this; }

};

//二項演算子 Binary Operator
Vector2 operator+(const Vector2& v1, const Vector2& v2);
Vector2 operator-(const Vector2& v1, const Vector2& v2);
Vector2 operator*(float s, const Vector2& v);
Vector2 operator+(const Vector2& v, float s);
Vector2 operator+(float s, const Vector2& v);
Vector2 operator*(const Vector2& v, float s);
Vector2 operator/(const Vector2& v, float s);

//単項演算子 Unary Operator
Vector2 operator-(const Vector2& v);
Vector2 operator+(const Vector2& v);