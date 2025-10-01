#pragma once

#include "Matrix4x4.h"
#include"Vector3.h"

//1.X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);

//1.Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);

//1.Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);

/// @brief XYZ回転/// @param radian /// @return 
Matrix4x4 MakeRotateXYZMatrix(Vector3 radian);