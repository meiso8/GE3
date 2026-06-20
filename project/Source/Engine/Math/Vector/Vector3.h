#pragma once

struct Vector3 {
    float x, y, z;

    //複合代入演算子Compound Assignement Operator
    Vector3& operator+=(float s) { x += s; y += s, z += s; return *this; }
    Vector3& operator-=(const Vector3& v) { x -= v.x, y -= v.y, z -= v.z; return *this; }
    Vector3& operator+=(const Vector3& v) { x += v.x, y += v.y, z += v.z; return *this; }
    Vector3& operator*=(float s) { x *= s, y *= s, z *= s; return*this; }
    Vector3& operator/=(float s) { x /= s, y /= s, z /= s; return*this; }
    bool operator==(const Vector3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

Vector3 Add(const Vector3& v1, const Vector3& v2);
Vector3 Subtract(const Vector3& v1, const Vector3& v2);
Vector3 Multiply(const float scalar, const Vector3& v);
Vector3 Multiply(const Vector3 v1, const Vector3& v2);

//内積
float Dot(const Vector3& v1, const Vector3& v2);
Vector3 Cross(const Vector3& v1, const Vector3& v2);
//二つ
// のベクトルが向かい合っているかは内積で判断できる
//c・(a×b)<= 0; 上記の外積と内積で判断
//長さ(ノルム)
float Length(const Vector3& v);
float Distance(const Vector3& v0, const Vector3& v1);

Vector3 Normalize(const Vector3& v);
//ターゲットに向かうベクトルを計算する
Vector3 ToTargetVector(const Vector3& target, const  Vector3& ownPos);
Vector3 CalculateLookAtRotate(const Vector3& startPos, const Vector3& endPos);
/// @brief ベクトルがゼロベクトルかどうかを判定する関数
/// @param v ベクトル
/// @return ゼロベクトルであればtrue、そうでなければfalse
bool IsZero(const Vector3 &v);

/// @brief 垂直ベクトルのうちの一つを求める関数
/// @param vector ベクトル
/// @return 垂直ベクトル
Vector3 Perpendicular(const Vector3& vector);

Vector3 Reflect(const Vector3& input, const Vector3& normal);
Vector3 Project(const Vector3& v1, const Vector3& v2);


//二項演算子 Binary Operator
Vector3 operator+(const Vector3& v1, const Vector3& v2);
Vector3 operator-(const Vector3& v1, const Vector3& v2);
Vector3 operator*(const Vector3& v1, const Vector3& v2);

Vector3 operator*(float s, const Vector3& v);
Vector3 operator+(const Vector3& v, float s);
Vector3 operator+(float s, const Vector3& v);
Vector3 operator*(const Vector3& v, float s);

Vector3 operator/(const Vector3& v, float s);

//単項演算子 Unary Operator
Vector3 operator-(const Vector3& v);
Vector3 operator+(const Vector3& v);
