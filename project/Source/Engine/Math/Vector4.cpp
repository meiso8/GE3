#include"Add.h"
#include"Subtract.h"
#include"Multiply.h"

//二項演算子 Binary Operator
Vector4 operator+(const Vector4& v1, const Vector4& v2) { return Add(v1, v2); }
Vector4 operator-(const Vector4& v1, const Vector4& v2) { return Subtract(v1, v2); }
Vector4 operator*(float s, const Vector4& v) { return Multiply(s, v); }
Vector4 operator+(const Vector4& v, float s) { return { v.x + s,v.y + s,v.z + s }; };
Vector4 operator+(float s, const Vector4& v) { return { s + v.x, s + v.y, s + v.z }; };
Vector4 operator*(const Vector4& v, float s) { return s * v; }
Vector4 operator/(const Vector4& v, float s) { return Multiply(1.0f / s, v); }

//単項演算子 Unary Operator
Vector4 operator-(const Vector4& v) { return { -v.x,-v.y,-v.z }; }
Vector4 operator+(const Vector4& v) { return v; }