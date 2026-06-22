#include "Papyrus.h"

#include "CollisionConfig.h"
#include"ModelManager.h"
#include"Model.h"
#include"JsonFile.h"
#include"DebugUI.h"
Papyrus::Papyrus() {

    object_ = std::make_unique<Object3d>();
    object_->Create();
    object_->SetMeshAndMaterial(ModelManager::GetModel("Papyrus"));

    Json file = JsonFile::GetJsonFiles("memo");
    std::string sizeKeys = "bookSize";

    AABB aabb = { {-2.5f,-1.0f,-0.063f}, {2.5f,1.0f,0.063f} };

    SetCollisionAttribute(kCollisionWall); //かべ
    SetCollisionMask(kCollisionPlayer | kCollisionEnemy|kCollisionMummy); // プレイヤーや壁と衝突

    // memoのサイズに合わせる
    SetAABB(aabb);
    SetWorldMatrix(object_->GetWorldTransform().matWorld_);
   
}

void Papyrus::Initialize() {
    object_->Initialize();
    object_->SetTranslate({ 0.0f,2.0f,5.0f });
}

void Papyrus::Update() {
    object_->Update();
    DebugUI::CheckObject3d(*object_, "Papyrus");
    ColliderUpdate();
}

void Papyrus::Draw(Camera& camera) {
    object_->Draw(camera);
    ColliderDraw(camera);

}

void Papyrus::OnCollision(Collider* collider) {
    if (collider->GetCollisionAttribute() == kCollisionPlayer) {
        // プレイヤーとぶつかったときの処理（必要なら）
    }
    OnCollisionCollider();

}
