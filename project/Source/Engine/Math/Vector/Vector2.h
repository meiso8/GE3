#pragma once

struct Vector2 {
    float x, y;

    //複合代入演算子Compound Assignement Operator
    Vector2& operator+=(float s) { x += s; y += s; return *this; }
    Vector2& operator-=(const Vector2& v) { x -= v.x, y -= v.y; return *this; }
    Vector2& operator+=(const Vector2& v) { x += v.x, y += v.y; return *this; }
    Vector2& operator/=(float s) { x /= s, y /= s; return*this; }

};

struct Vector2Int {
    int x, y;
};

Vector2 Add(const Vector2& v1, const Vector2& v2);
Vector2 Subtract(const Vector2& v1, const Vector2& v2);
Vector2 Multiply(const float scalar, const Vector2& v);
Vector2 Multiply(const Vector2& v1, const Vector2& v2);

//内積
float Dot(const Vector2& v1, const Vector2& v2);
float Cross(const Vector2& v1, const Vector2& v2);
//長さ(ノルム)
float Length(const Vector2& v);
Vector2 Normalize(const Vector2& v);
float Distance(const Vector2& v0, const Vector2& v1);


//二項演算子 Binary Operator
Vector2 operator+(const Vector2& v1, const Vector2& v2);
Vector2 operator-(const Vector2& v1, const Vector2& v2);
Vector2 operator*(float s, const Vector2& v);
Vector2 operator+(const Vector2& v, float s);
Vector2 operator+(float s, const Vector2& v);
Vector2 operator*(const Vector2& v, float s);
Vector2 operator/(const Vector2& v, float s);
Vector2 operator*(const Vector2& v1, const Vector2& v2);

//単項演算子 Unary Operator
Vector2 operator-(const Vector2& v);
Vector2 operator+(const Vector2& v);

