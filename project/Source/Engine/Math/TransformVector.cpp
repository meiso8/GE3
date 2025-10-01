#include"TransformVector.h"
#include<assert.h>

//座標変換関数
Vector3 TransformVector(const Vector3& vector, const Matrix4x4& matrix) {

    Vector3 result;

    result.x = vector.x * matrix.m[0][0]
        + vector.y * matrix.m[1][0]
        + vector.z * matrix.m[2][0]
        + 1.0f * matrix.m[3][0];

    result.y = vector.x * matrix.m[0][1]
        + vector.y * matrix.m[1][1]
        + vector.z * matrix.m[2][1]
        + 1.0f * matrix.m[3][1];

    result.z = vector.x * matrix.m[0][2]
        + vector.y * matrix.m[1][2]
        + vector.z * matrix.m[2][2]
        + 1.0f * matrix.m[3][2];

    float w = vector.x * matrix.m[0][3]
        + vector.y * matrix.m[1][3]
        + vector.z * matrix.m[2][3]
        + 1.0f * matrix.m[3][3];

    assert(w != 0.0f);
    result.x /= w;
    result.y /= w;
    result.z /= w;

    return result;

};
