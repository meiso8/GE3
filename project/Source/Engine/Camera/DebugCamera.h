#pragma once
#include"Camera.h"

class DebugCamera :public Camera
{
private:
    //XYZ軸回りのローカル回転角
    Vector3 deltaRotate_ = { 0.0f,0.0f,0.0f };
    //累積回転行列
    Matrix4x4 matRot_ = {};
    //回転速度
    float rotateSpeed_ = 0.0f;
    //上下左右移動
    float speed_ = 0.0f;
public:
    // 2. コピーと代入を禁止する（インスタンスが2つに増えるのを防ぐため）
    DebugCamera(const DebugCamera&) = delete;
    DebugCamera& operator=(const DebugCamera&) = delete;
    // 3. インスタンスを取得するための GetInstance 関数（必ず static にする）
    static DebugCamera* GetInstance()
    {
        // 関数内の static 変数は、プログラム実行中に1回だけ作られる
        // （C++11以降ではスレッドセーフが保証されているため安全です）
        static DebugCamera instance;
        return &instance;
    }

    /// @brief 更新
    void UpdateMatrix()override;
    void Initialize(const PROJECTION_TYPE& type = PROJECTION_TYPE::PERSPECTIVE)override;
private:
    DebugCamera();
    ~DebugCamera();
    void UpdateProjectionMatrix();
    void MouseInputMove();
    void InputTranslate();
    void InputRotate();
    void MoveZ(const float& speed);
    void MoveX(const float& speed);
    void MoveY(const float& speed);
};

