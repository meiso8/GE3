#pragma once
#include"Vector3.h"
#include"Matrix4x4.h"
#include"Input.h"

class Input;

class DebugCamera
{
private:
    //XYZ軸回りのローカル回転角
    Vector3 deltaRotate_ = { 0.0f,0.0f,0.0f };
    //累積回転行列
    Matrix4x4 matRot_ = {};
    //ローカル座標
    Vector3 translation_ = { 0.0f,0.0f,-50.0f };
    //ビュー行列
    Matrix4x4 viewMatrix_ = {};
    //射影行列
    Matrix4x4 projectionMatrix_ = {};
    //回転速度
    float rotateSpeed_ = {};

    //上下左右移動
    float speed_ = {};
    //入力
    Input* input_ = nullptr;
    float width_ = {};
    float height_ = {};
    //平行投影フラグ
    bool isOrthographic_ = false;
public:
    /// @brief 初期化
    void Initialize(Input* input, const float& width, const float& height);
    /// @brief 更新
    void Update();
    void MoveZ(const float& speed);
    void MoveX(const float& speed);
    void MoveY(const float& speed);
    void InputTranslate();
    void InputRotate();
    Matrix4x4 GetViewMatrix() { return viewMatrix_; };
    Matrix4x4 GetProjectionMatrix() { return projectionMatrix_; };
    Matrix4x4 GetViewProjectionMatrix();
    void SetIsOrthographic(const bool& flag) { isOrthographic_ = flag; };
};

