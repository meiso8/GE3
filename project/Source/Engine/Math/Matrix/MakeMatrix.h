#pragma once

#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vector2.h"

/// @brief 3×3単位行列の作成
/// @return 3x3単位行列
Matrix3x3 MakeIdentity3x3();

//単位行列の作成  
Matrix4x4 MakeIdentity4x4();

//拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
//2次元拡大縮小行列
Matrix3x3 MakeScaleMatrix(const Vector2& scale);

//1.X軸回転行列
Matrix4x4 MakeRotateXMatrix(const float& radian);

//1.Y軸回転行列
Matrix4x4 MakeRotateYMatrix(const float& radian);

//1.Z軸回転行列
Matrix4x4 MakeRotateZMatrix(const float& radian);

/// @brief XYZ回転/// @param radian /// @return 
Matrix4x4 MakeRotateXYZMatrix(const Vector3& radian);
//2次元回転行列
Matrix3x3 MakeRotateMatrix(const float& theta);

//3次元平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
//2次元平行移動行列作成関数
Matrix3x3 MakeTranslateMatrix(const Vector2& rectCenter);
//3次元アフィン変換行列の生成
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

//2次元アフィン行列作成関数
Matrix3x3 MakeAffineMatrix(const Vector2& scale, const float& theta, const Vector2& translate);

//3.ビューポート変換行列　正規化デバイス座標系->スクリーン座標
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
//3.ビューポート変換行列　
Matrix3x3 MakeViewportMatrix(float left, float top, float width, float height);
//2.正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
//2次元正射影行列
Matrix3x3 MakeOrthographicMatrix(float left, float top, float right, float bottom);

//1.透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

//転置行列
Matrix4x4 Transpose(const Matrix4x4& m);

/// @brief 3x3逆行列を求める
/// @param m 3x3行列
/// @return 3x3逆行列
Matrix3x3 Inverse(const Matrix3x3& m);

//逆行列
Matrix4x4 Inverse(const Matrix4x4& m);

//任意軸回転行列作成関数
Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);
//
Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to);

Matrix4x4 MakeRotateAxisSinCos(const Vector3& n, const float& cos, const float& sin);

namespace Math{
    //ワールド行列からワールド座標を取得する関数
    Vector2 GetWorldTransformByMatrix(const Matrix3x3& mat);
    //ワールド行列からワールド座標を取得する関数
    Vector3 GetWorldTransformByMatrix(const Matrix4x4& mat);
    Vector3 GetForward(const Matrix4x4& mat);
    Matrix4x4 GetBillBordMatrix(const Matrix4x4& cameraWorldMatrix);
}
