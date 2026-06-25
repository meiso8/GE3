#pragma once
#include"Matrix4x4.h"

struct CameraMatrix {
    //ワールド行列
    Matrix4x4 worldMat = { 0.0f };
    //ビュー行列
    Matrix4x4 viewMat = { 0.0f };
    //射影行列
    Matrix4x4 projectionMat = { 0.0f };
    //ビュープロジェクション行列
    Matrix4x4 viewProjectionMat = { 0.0f };
};
