#include"MakeAffineMatrix.h"
#include <cmath>

//3次元アフィン変換行列の生成
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {


    //Matrix4x4 rotateMat =  Multiply(MakeRotateXMatrix(rotate.x), Multiply(MakeRotateYMatrix(rotate.y), MakeRotateZMatrix(rotate.z)));

    Matrix4x4 result;

    float sinX = std::sin(rotate.x);
    float sinY = std::sin(rotate.y);
    float sinZ = std::sin(rotate.z);

    float cosX = std::cos(rotate.x);
    float cosY = std::cos(rotate.y);
    float cosZ = std::cos(rotate.z);

    result.m[0][0] = scale.x * cosY * cosZ;
    result.m[0][1] = scale.x * cosY * sinZ;
    result.m[0][2] = -scale.x * sinY;
    result.m[0][3] = 0.0f;

    float A = sinX * sinY;

    result.m[1][0] = scale.y * (A * cosZ - sinZ * cosX);
    result.m[1][1] = scale.y * (A * sinZ + cosX * cosZ);
    result.m[1][2] = scale.y * sinX * cosY;
    result.m[1][3] = 0.0f;

    result.m[2][0] = scale.z * (sinY * cosX * cosZ + sinX * sinZ);
    result.m[2][1] = scale.z * (sinY * cosX * sinZ - sinX * cosZ);
    result.m[2][2] = scale.z * (cosX * cosY);
    result.m[2][3] = 0.0f;

    result.m[3][0] = translate.x;
    result.m[3][1] = translate.y;
    result.m[3][2] = translate.z;
    result.m[3][3] = 1.0f;

    return result;
};
