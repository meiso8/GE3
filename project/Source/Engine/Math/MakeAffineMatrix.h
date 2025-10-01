#pragma once

#include "Matrix4x4.h"
#include"Vector3.h"

//3次元アフィン変換行列の生成
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
