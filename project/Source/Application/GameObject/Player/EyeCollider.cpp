#include "EyeCollider.h"
#include"CollisionConfig.h"
#include"TimeManager.h"
#include"Easing.h"
#include<algorithm>
EyeCollider::EyeCollider()
{
    object_ = std::make_unique<Object3d>();
    object_->Create();
    SetCollisionAttribute(kCollisionPlayerEye);
    SetCollisionMask(kCollisionWall);
    SetAABB({ { -0.5f,-0.5f,-0.5f }, { 0.5f,0.5f,0.5f } });
    SetWorldMatrix(object_->GetWorldTransform());
}

void EyeCollider::Update()
{
    object_->Update();
    ColliderUpdate();
}

void EyeCollider::Draw(Camera& camera)
{

    object_->Draw(camera);
    ColliderDraw(camera);
}

void EyeCollider::Initialize()
{
    walkingTheta_ = 0.0f;
    object_->Initialize();
    auto& transform = object_->GetTransform();
    transform.translate.y = kEyeDefaultPosY_;
    transform.translate.z = kEyeDefaultPosZ_;


}

void EyeCollider::OnCollision(Collider* collider)
{

    if (collider->GetCollisionAttribute() == kCollisionWall) {
        object_->Update();
    }

    OnCollisionCollider();
}

Vector3& EyeCollider::GetForward()
{
    static Vector3 forward;
    forward = Math::GetForward(object_->GetWorldMatrix());
    return forward;

}

void EyeCollider::Walk(const float& speed)
{
    //移動時の縦揺れを再現　速さによって揺れの周期を変更
    walkingTheta_ += TimeManager::DeltaTime() * 15.0f * speed;

    object_->GetTransform().translate.y = kEyeDefaultPosY_ + sinf(walkingTheta_) * 0.125f;
}

void EyeCollider::WalkStop()
{
    object_->GetTransform().translate.y = Lerp(object_->GetTransform().translate.y, kEyeDefaultPosY_, 0.5f);
}

void EyeCollider::MouseLook(const float& rotateX)
{
    object_->GetTransform().rotate.x = Lerp(object_->GetTransform().rotate.y, rotateX, 0.5f);
}

