#include"Subtract.h"

//ベクトル減算
Vector2 Subtract(const Vector2& v1, const Vector2& v2) {
    return { v1.x - v2.x,v1.y - v2.y };
};

//ベクトル減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
    return { v1.x - v2.x,v1.y - v2.y ,v1.z - v2.z };
};

Vector4 Subtract(const Vector4& v1, const Vector4& v2) {
    return { v1.x - v2.x,v1.y - v2.y ,v1.z - v2.z,v1.w - v2.w };
};



//行列の減算
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {

    Matrix4x4 result;

    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            result.m[row][column] = m1.m[row][column] - m2.m[row][column];
        }
    }

    return result;

};