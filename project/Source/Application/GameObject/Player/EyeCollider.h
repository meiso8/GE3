#pragma once
#include"Object3d.h"
#include<memory>
#include"Transform.h"

class EyeCollider
{
public:
    const float   kEyeDefaultPosY_ = 0.0f;
    const float   kEyeDefaultPosZ_ = 0.0f;
private:
    std::unique_ptr <Object3d> object_ = nullptr;

    Matrix4x4* parent_ = nullptr;
 
public:
    EyeCollider();
    void Initialize();
    void Update();
    void Draw(Camera& camera);

     Matrix4x4& GetWorldMatrix() {
        return object_->GetWorldTransform().matWorld_;
    }
     WorldTransform& GetWorldTransform() { return object_->GetWorldTransform(); };
     void SetParentMatrix(Matrix4x4* parent) {
         parent_ = parent;
     };
     Vector3& GetForward();
     void MouseLook(const float& rotateX);
};
