#pragma once

#include"Matrix4x4.h"

//3.ビューポート変換行列　正規化デバイス座標系->スクリーン座標
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
