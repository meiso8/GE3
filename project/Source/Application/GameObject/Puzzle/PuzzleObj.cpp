#include "PuzzleObj.h"

#include "CollisionConfig.h"
#include"ModelManager.h"
#include"Model.h"

PuzzleObj::PuzzleObj() {

    AABB aabb = { .min = {-0.25f,-0.25f,-0.25f},.max = {0.25f,0.25f,0.25f} };

    SetCollisionAttribute(CollisionTag::GetTag("Wall")); // ミイラの衝突属性
    SetCollisionMask(CollisionTag::GetTag("Player") | CollisionTag::GetTag("Enemy")); // プレイヤーや壁と衝突
 
    // memoのサイズに合わせる
    SetAABB(aabb);

    aniObj_ = std::make_unique<AnimationObject3d>();

    auto* model = ModelManager::GetModel("Puzzle.gltf");
    aniObj_->Create();
    aniObj_->SetModelAndLoadAnimation(model);
    aniObj_->SetMeshAndMaterial(model);
    aniObj_->SetTemperature(0.75f);

    SetWorldMatrix(aniObj_->GetWorldTransform().matWorld_);
}

PuzzleObj::~PuzzleObj()
{
}

void PuzzleObj::Initialize() {

    isOpen_ = false;
    aniObj_->Initialize();
    aniObj_->SetTranslate({ -24.0f,0.25f,-24.0f });
    aniObj_->SetObjectName("PuzzleObj");
    aniObj_->RegisterObject();
}

void PuzzleObj::Update() {

    if (isOpen_) {
        aniObj_->UpdateAniTimer(false);
    }

    aniObj_->Update();
    aniObj_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
}

void PuzzleObj::Draw(Camera& camera) {
    aniObj_->Draw(camera);

}

void PuzzleObj::OnCollision(Collider* collider) {

    if (collider->GetCollisionAttribute() == CollisionTag::GetTag("Player")) {
        // プレイヤーとぶつかったときの処理（必要なら）
    }

}
