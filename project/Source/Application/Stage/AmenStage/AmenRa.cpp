#include "AmenRa.h"

#include"Window.h"

#include"ModelManager.h"
#include"Model.h"
#include"DebugUI.h"
#include"Lerp.h"

AmenRa::AmenRa() {

    object_ = std::make_unique<Object3d>();
    object_->Create();
    object_->SetMeshAndMaterial(ModelManager::GetModel("AmenRa"));
    object_->SetLightMode(Object3d::kLightModeLReflectance);
    object_->SetTemperature(0.1f);
    AABB aabb = { .min = {-1.0f,0.0f,-1.0f},.max = {1.0f,6.01f,1.0f} };
    SetCollisionAttribute(kCollisionWall); // ミイラの衝突属性
    SetCollisionMask(kCollisionPlayer | kCollisionEnemy); // プレイヤーや壁と衝突

    //サイズに合わせる
    SetAABB(aabb);
    SetWorldMatrix(object_->GetWorldTransform().matWorld_);
}

void AmenRa::Initialize() {

    object_->Initialize();
    auto& transform = object_->GetTransform();
    transform.translate = {0.0f,0.0f,0.0f};
    transform.rotate.y = std::numbers::pi_v<float>;
}

void AmenRa::Update() {

    object_->Update();
    DebugUI::CheckObject3d(*object_, "AmenRa");

}

void AmenRa::Draw(Camera& camera) {
    object_->Draw(camera);

}

void AmenRa::OnCollision(Collider* collider) {
    if (collider->GetCollisionAttribute() == kCollisionPlayer) {
        // プレイヤーとぶつかったときの処理（必要なら）
    }

}
