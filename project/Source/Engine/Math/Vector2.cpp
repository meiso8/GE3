#include"Add.h"
#include"Subtract.h"
#include"Multiply.h"

//二項演算子 Binary Operator
Vector2 operator+(const Vector2& v1, const Vector2& v2) { return Add(v1, v2); }
Vector2 operator-(const Vector2& v1, const Vector2& v2) { return Subtract(v1, v2); }
Vector2 operator*(float s, const Vector2& v) { return Multiply(s, v); }
Vector2 operator+(const Vector2& v, float s) { return { v.x + s,v.y + s }; };
Vector2 operator+(float s, const Vector2& v) { return { s + v.x, s + v.y }; };
Vector2 operator*(const Vector2& v, float s) { return s * v; }
Vector2 operator/(const Vector2& v, float s) { return Multiply(1.0f / s, v); }

//単項演算子 Unary Operator
Vector2 operator-(const Vector2& v) { return { -v.x,-v.y }; }
Vector2 operator+(const Vector2& v) { return v; }


