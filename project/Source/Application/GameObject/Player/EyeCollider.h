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
public:

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
