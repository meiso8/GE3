#include "EyeCollider.h"
#include"CollisionConfig.h"
#include"TimeManager.h"
#include"Easing.h"
#include<algorithm>
#include"MakeMatrix.h"

EyeCollider::EyeCollider()
{
    object_ = std::make_unique<Object3d>();
    object_->Create();
}

void EyeCollider::Update()
{
    //object_->Update();
    auto& transform = object_->GetTransform();
    Matrix4x4 child = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate)* *parent_;
    object_->SetWorldMatrix(child);

}

void EyeCollider::Draw(Camera& camera)
{
    object_->Draw(camera);
}

void EyeCollider::Initialize()
{
    object_->Initialize();
    auto& transform = object_->GetTransform();
    transform.translate.y = kEyeDefaultPosY_;
    transform.translate.z = kEyeDefaultPosZ_;
}

Vector3& EyeCollider::GetForward()
{
    static Vector3 forward;
    forward = Math::GetForward(GetWorldMatrix());
    return forward;
}


void EyeCollider::MouseLook(const float& rotateX)
{
    auto& transform = object_->GetTransform();
    transform.rotate.x = Lerp(transform.rotate.x, rotateX, 0.5f);
}

