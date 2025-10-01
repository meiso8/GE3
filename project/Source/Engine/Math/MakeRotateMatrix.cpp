
#include"MakeRotateMatrix.h"
#include"Multiply.h"
#include <cmath>

//1.X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian) {


    Matrix4x4 result;

    result.m[0][0] = 1.0f;
    result.m[0][1] = 0.0f;
    result.m[0][2] = 0.0f;
    result.m[0][3] = 0.0f;

    result.m[1][0] = 0.0f;
    result.m[1][1] = std::cos(radian);
    result.m[1][2] = std::sin(radian);
    result.m[1][3] = 0.0f;

    result.m[2][0] = 0.0f;
    result.m[2][1] = -std::sin(radian);
    result.m[2][2] = std::cos(radian);
    result.m[2][3] = 0.0f;

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
};

//1.Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian) {

    Matrix4x4 result;

    result.m[0][0] = std::cos(radian);
    result.m[0][1] = 0.0f;
    result.m[0][2] = -std::sin(radian);
    result.m[0][3] = 0.0f;

    result.m[1][0] = 0.0f;
    result.m[1][1] = 1.0f;
    result.m[1][2] = 0.0f;
    result.m[1][3] = 0.0f;

    result.m[2][0] = std::sin(radian);
    result.m[2][1] = 0.0f;
    result.m[2][2] = std::cos(radian);
    result.m[2][3] = 0.0f;

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
};

//1.Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {

    Matrix4x4 result;

    result.m[0][0] = std::cos(radian);
    result.m[0][1] = std::sin(radian);
    result.m[0][2] = 0.0f;
    result.m[0][3] = 0.0f;

    result.m[1][0] = -std::sin(radian);
    result.m[1][1] = std::cos(radian);
    result.m[1][2] = 0.0f;
    result.m[1][3] = 0.0f;

    result.m[2][0] = 0.0f;
    result.m[2][1] = 0.0f;
    result.m[2][2] = 1.0f;
    result.m[2][3] = 0.0f;

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
};

//XYZ回転
Matrix4x4 MakeRotateXYZMatrix(Vector3 radian) {
    Matrix4x4 result = Multiply(Multiply(MakeRotateXMatrix(radian.x), MakeRotateYMatrix(radian.y)), MakeRotateZMatrix(radian.z));
    return result;
}