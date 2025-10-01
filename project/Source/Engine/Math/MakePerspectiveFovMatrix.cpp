#include"MakePerspectiveFovMatrix.h"

//1.透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {

    Matrix4x4 result;

    result.m[0][0] = 1.0f / aspectRatio * cot(fovY / 2.0f);
    result.m[0][1] = 0.0f;
    result.m[0][2] = 0.0f;
    result.m[0][3] = 0.0f;

    result.m[1][0] = 0.0f;
    result.m[1][1] = cot(fovY / 2.0f);
    result.m[1][2] = 0.0f;
    result.m[1][3] = 0.0f;

    result.m[2][0] = 0.0f;
    result.m[2][1] = 0.0f;
    result.m[2][2] = farClip / (farClip - nearClip);
    result.m[2][3] = 1.0f;

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
    result.m[3][3] = 0.0f;

    return result;

};
