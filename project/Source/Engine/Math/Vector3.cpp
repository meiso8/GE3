#include"Add.h"
#include"Subtract.h"
#include"Multiply.h"

//二項演算子 Binary Operator
Vector3 operator+(const Vector3& v1, const Vector3& v2) { return Add(v1, v2); }
Vector3 operator-(const Vector3& v1, const Vector3& v2) { return Subtract(v1, v2); }
Vector3 operator*(float s, const Vector3& v) { return Multiply(s, v); }
Vector3 operator+(const Vector3& v, float s) { return { v.x + s,v.y + s,v.z + s }; };
Vector3 operator+(float s, const Vector3& v) { return { s + v.x, s + v.y, s + v.z }; };
Vector3 operator*(const Vector3& v, float s) { return s * v; }
Vector3 operator/(const Vector3& v, float s) { return Multiply(1.0f / s, v); }

//単項演算子 Unary Operator
Vector3 operator-(const Vector3& v) { return { -v.x,-v.y,-v.z }; }
Vector3 operator+(const Vector3& v) { return v; }

