#pragma once
#include"Object3d.h"
#include<memory>
#include"Transform.h"

class EyePosition
{
public:

private:
    //親のポインタ
    Matrix4x4* parent_ = nullptr;
    //トランスフォーム
    WorldTransform transform_{};
    //上から視点フラグ
    bool isUp_ = false;
    //上から視点から戻る際のフラグ
    bool isDown_ = false;
    //開始位置
    Vector3 startPos_{};
    //終了位置
    Vector3 endPos_{};
    //開始回転
    Vector3 startRot_{};
    //終了回転
    Vector3 endRot_{};
    //時間
    float time_ = 0.0f;
private:
    void EasingUpdate();
public:
    //カメラの上昇か下降時
    bool IsCameraUpOrDown() { return isUp_ || isDown_; };
    //上昇をセットする
    void SiftToUp();
    //下降をセットする
    void ShftToTDown();
    void Initialize();
    void Update();

     Matrix4x4& GetWorldMatrix() {
        return transform_.matWorld_;
    }
     WorldTransform& GetWorldTransform() { return transform_; };
     void SetParentMatrix(Matrix4x4* parent) {
         parent_ = parent;
     };

     Vector3& GetForward();

     void MouseLook(const float& rotateX);
};
