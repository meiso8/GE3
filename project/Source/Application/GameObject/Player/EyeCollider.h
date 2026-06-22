#pragma once
#include"Collider.h"
#include<memory>
class EyeCollider:public Collider
{
private:
    std::unique_ptr <Object3d> object_;
    float walkingTheta_ = 0.0f;
public:
    const float   kEyeDefaultPosY_ = 1.5f;
    const float   kEyeDefaultPosZ_ = 0.0f;


    EyeCollider();
    void Initialize();
    void Update();
    void Draw(Camera& camera);
    void OnCollision(Collider* collider)override;
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
