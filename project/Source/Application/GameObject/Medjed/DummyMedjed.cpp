#include "DummyMedjed.h"
#include"ModelManager.h"
#include"Model.h"
#include"CollisionConfig.h"
#include"Random.h"
#include"Easing.h"
#include"TimeManager.h"
#include<algorithm>
void DummyMedjed::Look(const Vector3& target)
{
    aniTimer_ += TimeManager::DeltaTime() * 0.25f;
    aniTimer_ = std::clamp(aniTimer_, 0.0f, 1.0f);
    Vector3 direction = target - object3d_->GetWorldTransform().GetWorldPosition();
    float targetAngle = std::atan2(direction.x, direction.z);
    // 差分を最短経路に補正
    float delta = NormalizeAngle(targetAngle - startRotateY_);

    object3d_->GetTransform().rotate.y = startRotateY_ + Easing::EaseInOutBounce(0.0f, delta, aniTimer_);
}

void DummyMedjed::GoToTarget(const Vector3& target)
{
    auto& transform = object3d_->GetTransform();
   transform.translate = Lerp(transform.translate, target, 0.05f);
}

void DummyMedjed::Hide()
{
    hideTimer_ += TimeManager::DeltaTime()*1.2f;
    hideTimer_ = std::clamp(hideTimer_, 0.0f, 1.0f);
}

DummyMedjed::DummyMedjed()
{
    model_ = ModelManager::GetModel("medjed.gltf");

    object3d_ = std::make_unique<Object3d>();

    object3d_->Create();    
    object3d_->SetMeshAndMaterial(model_);
    object3d_->SetTemperature(0.1f);

    SetAABB(localAABB_);
    SetCollisionAttribute(CollisionTag::GetTag("DummyMedjed"));
    SetCollisionMask(CollisionTag::GetTag("Player") | CollisionTag::GetTag("Medjed"));
    SetWorldMatrix(object3d_->GetWorldTransform());

}
void DummyMedjed::Init()
{
    hideTimer_ = 0.0f;
    aniTimer_ = 0.0f;
    object3d_->Initialize();
    Random random;
    random.SetMinMax(0.0f,rotateRange_);
    startRotateY_ = random.Get();
    auto& transform = object3d_->GetTransform();
    transform.rotate.y = startRotateY_;
    transform.translate.y = startPosY_;
}

void DummyMedjed::Draw(Camera& camera)
{

    if (IsHide()) {
        return;
    }

    object3d_->SetLightMode(Object3d::kLightModeHalfL);
    object3d_->Draw(camera);

    //ColliderDraw(camera);
}

void DummyMedjed::Update()
{
    if (IsHide()) {
        return;
    }
    SetColor({ 1.0f,1.0f,1.0f,1.0f });

    object3d_->Update();

}


void DummyMedjed::OnCollision(Collider* collider)
{
}
