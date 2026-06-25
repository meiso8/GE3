#include"MakeMatrix.h"
#include <cmath>
#include"cotangent.h"
#include<numbers>
Matrix3x3 MakeIdentity3x3() {
    Matrix3x3 result;
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            result.m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
    return result;
}

//単位行列
Matrix4x4 MakeIdentity4x4() {

    Matrix4x4 result;

    result = { 1.0f,0.0f,0.0f,0.0f,
               0.0f,1.0f,0.0f,0.0f,
               0.0f,0.0f,1.0f,0.0f,
               0.0f,0.0f,0.0f,1.0f,
    };

    return result;

};

//拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
    Matrix4x4 result;

    result.m[0][0] = scale.x;
    result.m[0][1] = 0.0f;
    result.m[0][2] = 0.0f;
    result.m[0][3] = 0.0f;

    result.m[1][0] = 0.0f;
    result.m[1][1] = scale.y;
    result.m[1][2] = 0.0f;
    result.m[1][3] = 0.0f;

    result.m[2][0] = 0.0f;
    result.m[2][1] = 0.0f;
    result.m[2][2] = scale.z;
    result.m[2][3] = 0.0f;

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
}
Matrix3x3 MakeScaleMatrix(const Vector2& scale)
{

    Matrix3x3 result = {};

    result.m[0][0] = scale.x;
    result.m[1][0] = 0.0f;
    result.m[2][0] = 0.0f;

    result.m[0][1] = 0.0f;
    result.m[1][1] = scale.y;
    result.m[2][1] = 0.0f;

    result.m[0][2] = 0.0f;
    result.m[1][2] = 0.0f;
    result.m[2][2] = 1.0f;

    return (result);
}
;


//1.X軸回転行列
Matrix4x4 MakeRotateXMatrix(const float& radian) {


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
Matrix4x4 MakeRotateYMatrix(const float& radian) {

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
Matrix4x4 MakeRotateZMatrix(const float& radian) {

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
Matrix4x4 MakeRotateXYZMatrix(const Vector3& radian) {
    Matrix4x4 result = Multiply(Multiply(MakeRotateXMatrix(radian.x), MakeRotateYMatrix(radian.y)), MakeRotateZMatrix(radian.z));
    return result;
}

Matrix3x3 MakeRotateMatrix(const float& theta)
{
    Matrix3x3 result = {};

    result.m[0][0] = cosf(theta);
    result.m[1][0] = -sinf(theta);
    result.m[2][0] = 0.0f;

    result.m[0][1] = sinf(theta);
    result.m[1][1] = cosf(theta);
    result.m[2][1] = 0.0f;

    result.m[0][2] = 0.0f;
    result.m[1][2] = 0.0f;
    result.m[2][2] = 1.0f;

    return (result);
}

//平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {

    Matrix4x4 result;
    result.m[0][0] = 1.0f;
    result.m[0][1] = 0.0f;
    result.m[0][2] = 0.0f;
    result.m[0][3] = 0.0f;

    result.m[1][0] = 0.0f;
    result.m[1][1] = 1.0f;
    result.m[1][2] = 0.0f;
    result.m[1][3] = 0.0f;

    result.m[2][0] = 0.0f;
    result.m[2][1] = 0.0f;
    result.m[2][2] = 1.0f;
    result.m[2][3] = 0.0f;

    result.m[3][0] = translate.x;
    result.m[3][1] = translate.y;
    result.m[3][2] = translate.z;
    result.m[3][3] = 1.0f;

    return result;
}

Matrix3x3 MakeTranslateMatrix(const Vector2& rectCenter)
{
    Matrix3x3 result = {};

    result.m[0][0] = 1.0f;
    result.m[1][0] = 0.0f;
    result.m[2][0] = rectCenter.x;

    result.m[0][1] = 0.0f;
    result.m[1][1] = 1.0f;
    result.m[2][1] = rectCenter.y;

    result.m[0][2] = 0.0f;
    result.m[1][2] = 0.0f;
    result.m[2][2] = 1.0f;

    return (result);
}

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
}
Matrix3x3 MakeAffineMatrix(const Vector2& scale, const float& theta, const Vector2& translate)
{
    Matrix3x3 result = {};

    result.m[0][0] = scale.x * cosf(theta);
    result.m[1][0] = scale.y * -sinf(theta);
    result.m[2][0] = translate.x;

    result.m[0][1] = scale.x * sinf(theta);
    result.m[1][1] = scale.y * cosf(theta);
    result.m[2][1] = translate.y;

    result.m[0][2] = 0.0f;
    result.m[1][2] = 0.0f;
    result.m[2][2] = 1.0f;

    return (result);
}
;

//3.ビューポート変換行列　正規化デバイス座標系->スクリーン座標
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {


    Matrix4x4 result;

    result.m[0][0] = width / 2.0f;
    result.m[0][1] = 0.0f;
    result.m[0][2] = 0.0f;
    result.m[0][3] = 0.0f;

    result.m[1][0] = 0.0f;
    result.m[1][1] = -height / 2.0f;
    result.m[1][2] = 0.0f;
    result.m[1][3] = 0.0f;

    result.m[2][0] = 0.0f;
    result.m[2][1] = 0.0f;
    result.m[2][2] = maxDepth - minDepth;
    result.m[2][3] = 0.0f;

    result.m[3][0] = left + width / 2.0f;
    result.m[3][1] = top + height / 2.0f;
    result.m[3][2] = minDepth;
    result.m[3][3] = 1.0f;

    return result;

}
Matrix3x3 MakeViewportMatrix(float left, float top, float width, float height)
{
    //引数はスクリーン空間上でNDCがどこにどこの大きさで見えるかというのを指定する。

    //ビューポート変換
    Matrix3x3 result = {};
    result.m[0][0] = width / 2.0f;
    result.m[1][0] = 0.0f;
    result.m[2][0] = left + (width / 2.0f);

    result.m[0][1] = 0.0f;
    result.m[1][1] = -height / 2.0f;
    result.m[2][1] = top + (height / 2.0f);

    result.m[0][2] = 0.0f;
    result.m[1][2] = 0.0f;
    result.m[2][2] = 1.0f;

    return result;
}
;

//2.正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {

    Matrix4x4 result;

    result.m[0][0] = 2.0f / (right - left);
    result.m[0][1] = 0.0f;
    result.m[0][2] = 0.0f;
    result.m[0][3] = 0.0f;

    result.m[1][0] = 0.0f;
    result.m[1][1] = 2.0f / (top - bottom);
    result.m[1][2] = 0.0f;
    result.m[1][3] = 0.0f;

    result.m[2][0] = 0.0f;
    result.m[2][1] = 0.0f;
    result.m[2][2] = 1.0f / (farClip - nearClip);
    result.m[2][3] = 0.0f;

    result.m[3][0] = (left + right) / (left - right);
    result.m[3][1] = (top + bottom) / (bottom - top);
    result.m[3][2] = nearClip / (nearClip - farClip);
    result.m[3][3] = 1.0f;

    return result;

}
Matrix3x3 MakeOrthographicMatrix(float left, float top, float right, float bottom)
{
    //正規かデバイス座標系
    //NDC

    //引数は左上と右下の座標でカメラ空間上での座標である
    Matrix3x3 result = {};
    result.m[0][0] = 2.0f / (right - left);
    result.m[1][0] = 0.0f;
    result.m[2][0] = (left + right) / (left - right);

    result.m[0][1] = 0.0f;
    result.m[1][1] = 2.0f / (top - bottom);
    result.m[2][1] = (top + bottom) / (bottom - top);

    result.m[0][2] = 0.0f;
    result.m[1][2] = 0.0f;
    result.m[2][2] = 1.0f;

    return result;
}
;

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

//転置行列
Matrix4x4 Transpose(const Matrix4x4& m) {

    Matrix4x4 result;

    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            result.m[row][column] = m.m[column][row];
        }
    }

    return result;
}

Matrix3x3 Inverse(const Matrix3x3& m) {
    float abs =
        m.m[0][0] * m.m[1][1] * m.m[2][2]
        + m.m[0][1] * m.m[1][2] * m.m[2][0]
        + m.m[0][2] * m.m[1][0] * m.m[2][1]
        - m.m[0][2] * m.m[1][1] * m.m[2][0]
        - m.m[0][1] * m.m[1][0] * m.m[2][2]
        - m.m[0][0] * m.m[1][2] * m.m[2][1];

    if (std::abs(abs) < 1e-6f) {
        // 逆行列が作れない！→ 単位行列を返す or エラー処理
        return MakeIdentity3x3(); // または assert(false);
    }

    Matrix3x3 result;
    result.m[0][0] = (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1]) / abs;
    result.m[0][1] = (m.m[0][2] * m.m[2][1] - m.m[0][1] * m.m[2][2]) / abs;
    result.m[0][2] = (m.m[0][1] * m.m[1][2] - m.m[0][2] * m.m[1][1]) / abs;
    result.m[1][0] = (m.m[1][2] * m.m[2][0] - m.m[1][0] * m.m[2][2]) / abs;
    result.m[1][1] = (m.m[0][0] * m.m[2][2] - m.m[0][2] * m.m[2][0]) / abs;
    result.m[1][2] = (m.m[0][2] * m.m[1][0] - m.m[0][0] * m.m[1][2]) / abs;
    result.m[2][0] = (m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0]) / abs;
    result.m[2][1] = (m.m[0][1] * m.m[2][0] - m.m[0][0] * m.m[2][1]) / abs;
    result.m[2][2] = (m.m[0][0] * m.m[1][1] - m.m[0][1] * m.m[1][0]) / abs;
    return result;
}

//逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {
    float abs =
        m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]
        + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]
        + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]

        - m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]
        - m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]
        - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]

        - m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]
        - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]
        - m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]

        + m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]
        + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]
        + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]

        + m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]
        + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]
        + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]

        - m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]
        - m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]
        - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]

        - m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]
        - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]
        - m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]

        + m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]
        + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]
        + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];


    if (std::abs(abs) < 1e-6f) {
        // 逆行列が作れない！→ 単位行列を返す or エラー処理
        return MakeIdentity4x4(); // または assert(false);
    }

    Matrix4x4 result;

    result.m[0][0] = (
        m.m[1][1] * m.m[2][2] * m.m[3][3]
        + m.m[1][2] * m.m[2][3] * m.m[3][1]
        + m.m[1][3] * m.m[2][1] * m.m[3][2]
        - m.m[1][3] * m.m[2][2] * m.m[3][1]
        - m.m[1][2] * m.m[2][1] * m.m[3][3]
        - m.m[1][1] * m.m[2][3] * m.m[3][2]) / abs;

    result.m[0][1] = (
        -m.m[0][1] * m.m[2][2] * m.m[3][3]
        - m.m[0][2] * m.m[2][3] * m.m[3][1]
        - m.m[0][3] * m.m[2][1] * m.m[3][2]
        + m.m[0][3] * m.m[2][2] * m.m[3][1]
        + m.m[0][2] * m.m[2][1] * m.m[3][3]
        + m.m[0][1] * m.m[2][3] * m.m[3][2]) / abs;

    result.m[0][2] = (
        m.m[0][1] * m.m[1][2] * m.m[3][3]
        + m.m[0][2] * m.m[1][3] * m.m[3][1]
        + m.m[0][3] * m.m[1][1] * m.m[3][2]
        - m.m[0][3] * m.m[1][2] * m.m[3][1]
        - m.m[0][2] * m.m[1][1] * m.m[3][3]
        - m.m[0][1] * m.m[1][3] * m.m[3][2]) / abs;

    result.m[0][3] = (
        -m.m[0][1] * m.m[1][2] * m.m[2][3]
        - m.m[0][2] * m.m[1][3] * m.m[2][1]
        - m.m[0][3] * m.m[1][1] * m.m[2][2]
        + m.m[0][3] * m.m[1][2] * m.m[2][1]
        + m.m[0][2] * m.m[1][1] * m.m[2][3]
        + m.m[0][1] * m.m[1][3] * m.m[2][2]
        ) / abs;

    result.m[1][0] = (
        -m.m[1][0] * m.m[2][2] * m.m[3][3]
        - m.m[1][2] * m.m[2][3] * m.m[3][0]
        - m.m[1][3] * m.m[2][0] * m.m[3][2]
        + m.m[1][3] * m.m[2][2] * m.m[3][0]
        + m.m[1][2] * m.m[2][0] * m.m[3][3]
        + m.m[1][0] * m.m[2][3] * m.m[3][2]
        ) / abs;

    result.m[1][1] = (
        m.m[0][0] * m.m[2][2] * m.m[3][3]
        + m.m[0][2] * m.m[2][3] * m.m[3][0]
        + m.m[0][3] * m.m[2][0] * m.m[3][2]
        - m.m[0][3] * m.m[2][2] * m.m[3][0]
        - m.m[0][2] * m.m[2][0] * m.m[3][3]
        - m.m[0][0] * m.m[2][3] * m.m[3][2]
        ) / abs;

    result.m[1][2] = (
        -m.m[0][0] * m.m[1][2] * m.m[3][3]
        - m.m[0][2] * m.m[1][3] * m.m[3][0]
        - m.m[0][3] * m.m[1][0] * m.m[3][2]
        + m.m[0][3] * m.m[1][2] * m.m[3][0]
        + m.m[0][2] * m.m[1][0] * m.m[3][3]
        + m.m[0][0] * m.m[1][3] * m.m[3][2]
        ) / abs;

    result.m[1][3] = (
        m.m[0][0] * m.m[1][2] * m.m[2][3]
        + m.m[0][2] * m.m[1][3] * m.m[2][0]
        + m.m[0][3] * m.m[1][0] * m.m[2][2]
        - m.m[0][3] * m.m[1][2] * m.m[2][0]
        - m.m[0][2] * m.m[1][0] * m.m[2][3]
        - m.m[0][0] * m.m[1][3] * m.m[2][2]
        ) / abs;

    result.m[2][0] = (
        m.m[1][0] * m.m[2][1] * m.m[3][3]
        + m.m[1][1] * m.m[2][3] * m.m[3][0]
        + m.m[1][3] * m.m[2][0] * m.m[3][1]
        - m.m[1][3] * m.m[2][1] * m.m[3][0]
        - m.m[1][1] * m.m[2][0] * m.m[3][3]
        - m.m[1][0] * m.m[2][3] * m.m[3][1]
        ) / abs;

    result.m[2][1] = (
        -m.m[0][0] * m.m[2][1] * m.m[3][3]
        - m.m[0][1] * m.m[2][3] * m.m[3][0]
        - m.m[0][3] * m.m[2][0] * m.m[3][1]
        + m.m[0][3] * m.m[2][1] * m.m[3][0]
        + m.m[0][1] * m.m[2][0] * m.m[3][3]
        + m.m[0][0] * m.m[2][3] * m.m[3][1]
        ) / abs;

    result.m[2][2] = (
        m.m[0][0] * m.m[1][1] * m.m[3][3]
        + m.m[0][1] * m.m[1][3] * m.m[3][0]
        + m.m[0][3] * m.m[1][0] * m.m[3][1]
        - m.m[0][3] * m.m[1][1] * m.m[3][0]
        - m.m[0][1] * m.m[1][0] * m.m[3][3]
        - m.m[0][0] * m.m[1][3] * m.m[3][1]
        ) / abs;

    result.m[2][3] = (
        m.m[0][0] * m.m[1][1] * m.m[2][3]
        + m.m[0][1] * m.m[1][3] * m.m[2][0]
        + m.m[0][3] * m.m[1][0] * m.m[2][1]
        - m.m[0][3] * m.m[1][1] * m.m[2][0]
        - m.m[0][1] * m.m[1][0] * m.m[2][3]
        - m.m[0][0] * m.m[1][3] * m.m[2][1]
        ) / abs;

    result.m[3][0] = (
        -m.m[1][0] * m.m[2][1] * m.m[3][2]
        - m.m[1][1] * m.m[2][2] * m.m[3][0]
        - m.m[1][2] * m.m[2][0] * m.m[3][1]
        + m.m[1][2] * m.m[2][1] * m.m[3][0]
        + m.m[1][1] * m.m[2][0] * m.m[3][2]
        + m.m[1][0] * m.m[2][2] * m.m[3][1]
        ) / abs;
    result.m[3][1] = (

        m.m[0][0] * m.m[2][1] * m.m[3][2]
        + m.m[0][1] * m.m[2][2] * m.m[3][0]
        + m.m[0][2] * m.m[2][0] * m.m[3][1]
        - m.m[0][2] * m.m[2][1] * m.m[3][0]
        - m.m[0][1] * m.m[2][0] * m.m[3][2]
        - m.m[0][0] * m.m[2][2] * m.m[3][1]

        ) / abs;
    result.m[3][2] = (
        -m.m[0][0] * m.m[1][1] * m.m[3][2]
        - m.m[0][1] * m.m[1][2] * m.m[3][0]
        - m.m[0][2] * m.m[1][0] * m.m[3][1]
        + m.m[0][2] * m.m[1][1] * m.m[3][0]
        + m.m[0][1] * m.m[1][0] * m.m[3][2]
        + m.m[0][0] * m.m[1][2] * m.m[3][1]
        ) / abs;

    result.m[3][3] = (
        m.m[0][0] * m.m[1][1] * m.m[2][2]
        + m.m[0][1] * m.m[1][2] * m.m[2][0]
        + m.m[0][2] * m.m[1][0] * m.m[2][1]
        - m.m[0][2] * m.m[1][1] * m.m[2][0]
        - m.m[0][1] * m.m[1][0] * m.m[2][2]
        - m.m[0][0] * m.m[1][2] * m.m[2][1]
        ) / abs;

    return result;
}

Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle)
{
    float cos = std::cosf(angle);
    float sin = std::sinf(angle);

    return  MakeRotateAxisSinCos(axis, cos, sin);
}
Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to)
{
    Vector3 cross = Cross(from, to);
    float sin = Length(cross);
    float cos = Dot(from, to);

    Vector3 n = { 0.0f };

    if (from == -to) {
        if (from.x != 0.0f || from.y != 0.0f) {
            n = Normalize({ from.y ,-from.x,0.0f });
        } else if (from.z != 0.0f) {
            n = Normalize({ from.z ,0.0f,-from.x });
        }
    } else {
        n = Normalize(cross);
    }

    return MakeRotateAxisSinCos(n, cos, sin);
};

Matrix4x4 MakeRotateAxisSinCos(const Vector3& n, const float& cos, const float& sin) {

    float cosM = 1.0f - cos;

    Matrix4x4 tnn;
    tnn.m[0][0] = n.x * n.x * cosM + cos;
    tnn.m[0][1] = n.x * n.y * cosM + n.z * sin;
    tnn.m[0][2] = n.x * n.z * cosM - n.y * sin;
    tnn.m[0][3] = 0.0f;

    tnn.m[1][0] = n.y * n.x * cosM - n.z * sin;
    tnn.m[1][1] = n.y * n.y * cosM + cos;
    tnn.m[1][2] = n.y * n.z * cosM + n.x * sin;
    tnn.m[1][3] = 0.0f;

    tnn.m[2][0] = n.z * n.x * cosM + n.y * sin;
    tnn.m[2][1] = n.z * n.y * cosM - n.x * sin;
    tnn.m[2][2] = n.z * n.z * cosM + cos;
    tnn.m[2][3] = 0.0f;

    tnn.m[3][0] = 0.0f;
    tnn.m[3][1] = 0.0f;
    tnn.m[3][2] = 0.0f;
    tnn.m[3][3] = 1.0f;

    return tnn;

}



Vector2 Math::GetWorldTransformByMatrix(const Matrix3x3& mat)
{
    return { mat.m[2][0], mat.m[2][1] };
}
Vector3 Math::GetWorldTransformByMatrix(const Matrix4x4& mat)
{
    return { mat.m[3][0], mat.m[3][1], mat.m[3][2] };
}

Vector3 Math::GetForward(const Matrix4x4& mat)
{
    return Normalize({ mat.m[2][0],mat.m[2][1], mat.m[2][2] });
}

Matrix4x4 Math::GetBillBordMatrix(const Matrix4x4& cameraWorldMatrix)
{

    Matrix4x4 mat = Multiply(MakeRotateYMatrix(std::numbers::pi_v<float>), cameraWorldMatrix);
    //移動要素の排除
    mat.m[3][0] = 0.0f;
    mat.m[3][1] = 0.0f;
    mat.m[3][2] = 0.0f;

    return mat;
}


