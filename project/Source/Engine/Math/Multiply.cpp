#include"Multiply.h"

Vector2 Multiply(const float scalar, const Vector2& v) {
    return { scalar * v.x,scalar * v.y };
};


//スカラー倍
Vector3 Multiply(const float scalar, const Vector3& v) {
    return { scalar * v.x,scalar * v.y,scalar * v.z };
};

Vector4 Multiply(const float scalar, const Vector4& v) {
    return { scalar * v.x,scalar * v.y,scalar * v.z,scalar * v.w };
};


//行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {

    Matrix4x4 result;

    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            result.m[row][column] =
                m1.m[row][0] * m2.m[0][column] +
                m1.m[row][1] * m2.m[1][column] +
                m1.m[row][2] * m2.m[2][column] +
                m1.m[row][3] * m2.m[3][column]
                ;
        }
    }

    return result;
};


