#pragma once
#include"Object3d.h"
#include<memory>

class EyeCollider
{
public:
    const float   kEyeDefaultPosY_ = 1.5f;
    const float   kEyeDefaultPosZ_ = 0.0f;
private:
    std::unique_ptr <Object3d> object_ = nullptr;
    float walkingTheta_ = 0.0f;

public:
    EyeCollider();
    void Initialize();
    void Update();
    void Draw(Camera& camera);

     Matrix4x4& GetWorldMatrix() {
        return object_->GetWorldTransform().matWorld_;
    }
     WorldTransform& GetWorldTransform() { return object_->GetWorldTransform(); };
     void SetParent(WorldTransform& parent) { object_->GetWorldTransform().Parent(parent); }
     Vector3& GetForward();

     void Walk(const float& speed);
     void WalkStop();
     void MouseLook(const float& rotateX);
};
