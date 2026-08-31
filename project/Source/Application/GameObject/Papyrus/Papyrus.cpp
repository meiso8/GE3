#include "Papyrus.h"

#include "CollisionConfig.h"
#include"ModelManager.h"
#include"Model.h"
#include"JsonFile.h"
#include"DebugUI.h"
Papyrus::Papyrus() {

    object_ = std::make_unique<Object3d>();
    object_->Create();
    object_->SetMeshAndMaterial(ModelManager::GetModel("Papyrus.obj"));

    Json file = JsonFile::GetJsonFiles("memo");
    std::string sizeKeys = "bookSize";

    AABB aabb = { {-2.5f,-1.0f,-0.063f}, {2.5f,1.0f,0.063f} };

    SetCollisionAttribute(CollisionTag::GetTag("Wall")); //かべ

    SetCollisionMask(
        CollisionTag::GetTag("Player") 
        | CollisionTag::GetTag("Enemy")
        | CollisionTag::GetTag("Mummy")
    ); // プレイヤーや壁と衝突

    // memoのサイズに合わせる
    SetAABB(aabb);
    SetWorldMatrix(object_->GetWorldTransform().matWorld_);
   
}

Papyrus::~Papyrus()
{
    object_->UnRegisterObject();
}

void Papyrus::Initialize() {
    object_->Initialize();
    object_->SetObjectName("Papyrus");
    object_->RegisterObject();
    object_->SetTranslate({ 0.0f,2.0f,5.0f });
}

void Papyrus::Update() {
    object_->Update();
}

void Papyrus::Draw() {
    object_->Draw();
}

void Papyrus::OnCollision(Collider* collider) {
    if (collider->GetCollisionAttribute() == CollisionTag::GetTag("Player")) {
        // プレイヤーとぶつかったときの処理（必要なら）
    }
    OnCollisionCollider();

}
