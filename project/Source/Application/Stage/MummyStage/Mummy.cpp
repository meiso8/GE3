#include "Mummy.h"
#include "CollisionConfig.h"
#include"ModelManager.h"
#include"Model.h"

Mummy::Mummy() {
    platform_ = std::make_unique<Platform>();

    object_ = std::make_unique<Object3d>();

    object_->Create();
    object_->SetMeshAndMaterial(ModelManager::GetModel("mummy.obj"));

    object_->SetTemperature(0.2f);
    coffinModel_ = ModelManager::GetModel("coffin.gltf");
    aniObj_ = std::make_unique<AnimationObject3d>();

    aniObj_->Create();
    aniObj_->SetMeshAndMaterial(coffinModel_);

    aniObj_->SetTemperature(0.1f);


    //ペアレント
    object_->GetWorldTransform().Parent(platform_->GetWorldTransform());
    aniObj_->GetWorldTransform().Parent(platform_->GetWorldTransform());

    aniObj_->SetModelAndLoadAnimation(coffinModel_);

    SetCollisionAttribute(CollisionTag::GetTag("Wall")); // ミイラの衝突属性

    // プレイヤーや壁と衝突
    SetCollisionMask(
        CollisionTag::GetTag("Player")
        | CollisionTag::GetTag("Enemy")
        | CollisionTag::GetTag("Mummy"));

    SetAABB({ {-1.0f, 0.0f, -0.25f}, {1.0f, 0.5f, 0.25f} });
    SetWorldMatrix(aniObj_->GetWorldTransform().matWorld_);

}

void Mummy::Initialize() {
    isOpen_ = false;
    isOpenEnd_ = false;
    platform_->Initialize();

    aniObj_->Initialize();
    object_->Initialize();

    auto& transform = object_->GetTransform();
    auto& aniTransform = aniObj_->GetTransform();

    transform.rotate.y = 1.57f;
    aniTransform.rotate.y = 1.57f;
    // 台の高さを取得してオフセット 
    float platformHeight = platform_->GetAABB().max.y-platform_->GetAABB().min.y;
    transform.translate.y = platformHeight;
    aniTransform.translate.y = platformHeight;


}

void Mummy::Update() {

    if (isOpen_) {
        //ループしない
        aniObj_->UpdateAniTimer(false);
        
        if (aniObj_->IsAnimEnd()) {
            isOpenEnd_ = true;
        }
    }
    platform_->Update();
    aniObj_->Update();
    object_->Update();

}

void Mummy::Draw(Camera& camera) {
    platform_->Draw(camera);
    aniObj_->Draw(camera, kBlendModeNormal, kCullModeNone);
    object_->Draw(camera);

}

void Mummy::OnCollision(Collider* collider) {

}
