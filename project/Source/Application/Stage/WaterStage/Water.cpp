#include "Water.h"
#include"Window.h"

#include "CollisionConfig.h"
#include"ModelManager.h"
#include"Model.h"
#include"JsonFile.h"
#include"DebugUI.h"
#include"Lerp.h"
#include"Sound.h"
#include"TimeManager.h"
Water::Water() {

    object_ = std::make_unique<Object3d>();

    object_->Create();
    object_->SetMeshAndMaterial(ModelManager::GetModel("Water"));

    object_->GetWaveData(0).amplitude = 0.2f;
    object_->GetWaveData(0).direction = Normalize(Vector3{ 0.134f,0.12f,0.98f });
    object_->GetWaveData(0).frequency = 2.0f;
    object_->GetWaveData(1).amplitude = 0.1f;
    object_->GetWaveData(1).direction = { 1.0f,0.0f,0.0f };
    object_->GetWaveData(1).frequency = 4.0f;
    object_->SetTemperature(0.1f);

    AABB aabb = { .min = {-12.5f,-0.5f,-12.5f},.max = {12.5f,0.5f,12.5f} };
    SetCollisionAttribute(kCollisionWater);
    SetCollisionMask(kCollisionPlayer); // プレイヤー
    SetWorldMatrix(object_->GetWorldTransform().matWorld_);
    // memoのサイズに合わせる
    SetAABB(aabb);
}

void Water::Initialize() {
    isDrain_ = false;
    isPrePlayerHit_ = false;
    isPlayerHit_ = false;
    object_->GetWaveData(0).time = 0.0f;
    object_->GetWaveData(1).time = 0.0f;
    object_->Initialize();
    object_->SetTranslate({ 0.0f,0.75f,0.0f });
    object_->GetWaveData(0).amplitude = 0.2f;
    object_->GetWaveData(1).amplitude = 0.1f;
}

void Water::Update() {

    if (!isPrePlayerHit_ && isPlayerHit_|| isPrePlayerHit_ && !isPlayerHit_) {
    
        object_->GetWaveData(0).amplitude = 0.2f;
        object_->GetWaveData(1).amplitude = 0.1f;

        Sound::PlayOriginSE(SoundFactory::WATER_DROP);
    }

    isPrePlayerHit_ = isPlayerHit_;
    isPlayerHit_ = false;

    if (isDrain_) {
        object_->GetTransform().translate.y = Lerp(object_->GetTransform().translate.y, -0.625f, 0.01f);
        object_->GetWaveData(0).amplitude = Lerp(object_->GetWaveData(0).amplitude, 0.0f, 0.1f);
        object_->GetWaveData(1).amplitude = Lerp(object_->GetWaveData(1).amplitude, 0.0f, 0.1f);
    }



    object_->GetWaveData(0).time += TimeManager::DeltaTime();
    object_->GetWaveData(1).time = object_->GetWaveData(0).time + 1.5f;
    object_->Update();
    DebugUI::CheckObject3d(*object_, "Water");
}

void Water::Draw(Camera& camera) {
    object_->Draw(camera, kBlendModeMultiply);

}

void Water::OnCollision(Collider* collider) {

    isPlayerHit_ = true;





}
