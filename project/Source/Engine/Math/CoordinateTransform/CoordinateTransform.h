#pragma once
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vector4.h"
/// @brief 座標変換関数
/// @param vector ベクトル
/// @param matrix 3x3行列
/// @return 変換後ベクトル
Vector3 CoordinateTransform(const Vector3& vector, const Matrix3x3& matrix);

/// @brief 座標変換関数
/// @param vector ベクトル
/// @param matrix 4x4行列
/// @return 変換後ベクトル
Vector3 CoordinateTransform(const Vector3 &vector, const Matrix4x4 &matrix);
