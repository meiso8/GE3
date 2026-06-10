#include"Vector3.h"
#include <cmath>

Vector3 Add(const Vector3& v1, const Vector3& v2) {

    return { v1.x + v2.x,v1.y + v2.y ,v1.z + v2.z };
};
//ベクトル減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
    return { v1.x - v2.x,v1.y - v2.y ,v1.z - v2.z };
};

//スカラー倍
Vector3 Multiply(const float scalar, const Vector3& v) {
    return { scalar * v.x,scalar * v.y,scalar * v.z };
};

Vector3 Multiply(const Vector3 v1, const Vector3& v2) {
    return { v1.x * v2.x,v1.y * v2.y,v1.z * v2.z};
};


//内積
float Dot(const Vector3& v1, const Vector3& v2) {
    return  v1.x * v2.x + v1.y * v2.y + v1.z * v2.z ;
};


Vector3 Cross(const Vector3& v1, const Vector3& v2) {

    //左手でベクトルの順に巻いて親指が向く方向
    Vector3 result;

    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;

    return result;
}
//二つ
// のベクトルが向かい合っているかは内積で判断できる
//c・(a×b)<= 0; 上記の外積と内積で判断
//二項演算子 Binary Operator

//長さ(ノルム)
float Length(const Vector3& v) {
    return { sqrtf(Dot(v,v)) };
}
float Distance(const Vector3& v0, const Vector3& v1)
{
    return fabs(Length(v0 - v1));
}

Vector3 ToTargetVector(const Vector3& target, const  Vector3& ownPos)
{
    Vector3 velocity = target - ownPos;
    velocity = Normalize(velocity);
    return velocity;
}

Vector3 Normalize(const Vector3& v) {
    float length = Length(v);
    if (length != 0.0f) {
        return { v.x / length,v.y / length,v.z / length };
    } else {
        return { 0.0f, 0.0f, 0.0f };
    }

}

bool IsZero(const Vector3 &v) {
	return (v.x == 0.0f) && (v.y == 0.0f) && (v.z == 0.0f);
}

Vector3 Perpendicular(const Vector3& vector) {
    if (vector.x != 0.0f || vector.y != 0.0f) {
        return { -vector.y, vector.x, 0.0f };
    }
    return { 0.0f, -vector.z, vector.y };
}

Vector3 Reflect(const Vector3& input, const Vector3& normal) {
    return input - 2.0f * Dot(input, normal) * normal;
}

Vector3 Project(const Vector3& v1, const Vector3& v2) {
    Vector3 v2n = Normalize(v2);
    float d = Dot(v1, v2n);
    return v2n * d;
}


Vector3 operator+(const Vector3& v1, const Vector3& v2) { return Add(v1, v2); }
Vector3 operator-(const Vector3& v1, const Vector3& v2) { return Subtract(v1, v2); }
Vector3 operator*(const Vector3& v1, const Vector3& v2){  return Multiply(v1,v2);}
Vector3 operator*(float s, const Vector3& v) { return Multiply(s, v); }
Vector3 operator+(const Vector3& v, float s) { return { v.x + s,v.y + s,v.z + s }; };
Vector3 operator+(float s, const Vector3& v) { return { s + v.x, s + v.y, s + v.z }; };
Vector3 operator*(const Vector3& v, float s) { return s * v; }
Vector3 operator/(const Vector3& v, float s) { return Multiply(1.0f / s, v); }

//単項演算子 Unary Operator
Vector3 operator-(const Vector3& v) { return { -v.x,-v.y,-v.z }; }
Vector3 operator+(const Vector3& v) { return v; }

