#include "PuzzleObj.h"

#include"Window.h"
#include "CollisionConfig.h"
#include"ModelManager.h"
#include"Model.h"
#include"DebugUI.h"
#include"Lerp.h"

PuzzleObj::PuzzleObj() {

    AABB aabb = { .min = {-0.25f,-0.25f,-0.25f},.max = {0.25f,0.25f,0.25f} };

    SetCollisionAttribute(kCollisionWall); // ミイラの衝突属性
    SetCollisionMask(kCollisionPlayer | kCollisionEnemy); // プレイヤーや壁と衝突
 
    // memoのサイズに合わせる
    SetAABB(aabb);

    cubeMesh_ = std::make_unique<Primitive>();
    cubeMesh_->Create(PrimitiveGenerator::CreateCube(aabb));


    object_ = std::make_unique<Object3d>();

    object_->Create();
    object_->SetMeshAndMaterial(cubeMesh_.get());

    object_->SetTextureHandle(TextureFactory::PUZZLE_NUM);
    object_->SetTemperature(0.75f);

    SetWorldMatrix(object_->GetWorldTransform().matWorld_);

}

void PuzzleObj::Initialize() {

    object_->Initialize();
    object_->SetTranslate({ -24.0f,0.25f,-24.0f });

}

void PuzzleObj::Update() {

    object_->Update();
    object_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
    DebugUI::CheckObject3d(*object_, "PuzzleObj");
}

void PuzzleObj::Draw(Camera& camera) {
    object_->Draw(camera);

}

void PuzzleObj::OnCollision(Collider* collider) {
    if (collider->GetCollisionAttribute() == kCollisionPlayer) {
        // プレイヤーとぶつかったときの処理（必要なら）
    }

}
