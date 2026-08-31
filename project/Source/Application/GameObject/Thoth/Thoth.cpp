#include "Thoth.h"
#include"ModelManager.h"
#include"Model.h"
#include"Easing.h"
#include"TimeManager.h"

namespace {
    const float kHidePosY_ = -3.1f;

}

Thoth::Thoth()
{
    object_ = std::make_unique<Object3d>();
    object_->Create();
    object_->SetMeshAndMaterial(ModelManager::GetModel("thoth.obj"));
    object_->SetLightMode(Object3d::kLightModeLReflectance);
    object_->SetTemperature(0.1f);
    object_->SetShininess(50.0f);
    //ピカピカ！！
    object_->SetEnvironmentCoefficient(1.0f);

    AABB aabb = { .min = {-0.75f,0.0f,-0.75f},.max = {0.75f,3.0f,0.75f} };

    SetCollisionAttribute(CollisionTag::GetTag("Wall"));
    SetCollisionMask(CollisionTag::GetTag("Player") | CollisionTag::GetTag("Enemy")); // プレイヤーや壁と衝突

    //サイズに合わせる
    SetAABB(aabb);
    SetWorldMatrix(object_->GetWorldTransform().matWorld_);
}

Thoth::~Thoth()
{
}

void Thoth::Initialize()
{
    object_->Initialize();
    auto& transform = object_->GetTransform();
    transform.translate = { 0.0f,kHidePosY_,11.0f };
    transform.rotate.y = std::numbers::pi_v<float>;

    isActive_ = false;
    timer_ = 0.0f;
}

void Thoth::Update()
{

    if (isActive_) {
        timer_ += TimeManager::DeltaTime()*0.25f;
        timer_ = std::clamp(timer_, 0.0f, 1.0f);
        //アクティブだったら上に上がる
        object_->GetTransform().translate.y = Easing::EaseOutQuad(kHidePosY_, 0.0f, timer_);
    }

    object_->Update();
}

void Thoth::Draw() {
    
    if (!isActive_) {
        return;
    }

    object_->Draw();

}

void Thoth::OnCollision(Collider* collider) {
    if (collider->GetCollisionAttribute() == CollisionTag::GetTag("Player")) {
        // プレイヤーとぶつかったときの処理（必要なら）
    }

}
