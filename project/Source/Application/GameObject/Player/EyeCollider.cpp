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
    SetWorldMatrix(object_->worldTransform_.matWorld_);
}

void EyeCollider::Update()
{
    object_->Update();
    ColliderUpdate();
    SavePosition();
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
    object_->worldTransform_.translate_.y = kEyeDefaultPosY_;
    object_->worldTransform_.translate_.z = kEyeDefaultPosZ_;


}

void EyeCollider::OnCollision(Collider* collider)
{

    if (collider->GetCollisionAttribute() == kCollisionWall) {
        
        BackToPrevPosition();
        object_->Update(); // 座標を戻した状態で行列を再計算
    }

    OnCollisionCollider();
}

Vector3& EyeCollider::GetForward()
{
    static Vector3 forward;
    forward = Normalize(Vector3{ object_->worldTransform_.matWorld_.m[2][0],
        object_->worldTransform_.matWorld_.m[2][1],  object_->worldTransform_.matWorld_.m[2][2] });
    return forward;

}

void EyeCollider::Walk(const float& speed)
{
    //移動時の縦揺れを再現　速さによって揺れの周期を変更
    walkingTheta_ += Time::DeltaTime() * 15.0f * speed;
    object_->worldTransform_.translate_.y = kEyeDefaultPosY_ + sinf(walkingTheta_) * 0.125f;
}

void EyeCollider::WalkStop()
{
    object_->worldTransform_.translate_.y = Lerp(object_->worldTransform_.translate_.y, kEyeDefaultPosY_, 0.5f);
}

void EyeCollider::MouseLook(const float& rotateX)
{
    object_->worldTransform_.rotate_.x = Lerp(object_->worldTransform_.rotate_.y, rotateX, 0.5f);
}

