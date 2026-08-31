#include "AmenRa.h"

#include"Window.h"

#include"ModelManager.h"
#include"Model.h"
#include"DebugUI.h"
#include"Lerp.h"

AmenRa::AmenRa() {

    object_ = std::make_unique<Object3d>();
    object_->Create();
    object_->SetMeshAndMaterial(ModelManager::GetModel("AmenRa.obj"));
    object_->SetLightMode(Object3d::kLightModeLReflectance);
    object_->SetTemperature(0.1f);
    AABB aabb = { .min = {-1.0f,0.0f,-1.0f},.max = {1.0f,6.01f,1.0f} };

    SetCollisionAttribute(CollisionTag::GetTag("Wall"));
    SetCollisionMask(CollisionTag::GetTag("Player") | CollisionTag::GetTag("Enemy")); // プレイヤーや壁と衝突

    //サイズに合わせる
    SetAABB(aabb);
    SetWorldMatrix(object_->GetWorldTransform().matWorld_);
}

AmenRa::~AmenRa()
{
    object_.reset();
}

void AmenRa::Initialize() {

    object_->Initialize();
    auto& transform = object_->GetTransform();
    transform.translate = {0.0f,0.0f,0.0f};
    transform.rotate.y = std::numbers::pi_v<float>;
    object_->SetObjectName("AmenRa");
    object_->RegisterObject();
}

void AmenRa::Update() {

    object_->Update();

}

void AmenRa::Draw() {
    object_->Draw();

}

void AmenRa::OnCollision(Collider* collider) {
    if (collider->GetCollisionAttribute() == CollisionTag::GetTag("Player")) {
        // プレイヤーとぶつかったときの処理（必要なら）
    }

}
