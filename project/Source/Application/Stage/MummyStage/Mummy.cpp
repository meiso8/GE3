#include "Mummy.h"
#include "CollisionConfig.h"
#include"ModelManager.h"
#include"Model.h"

Mummy::Mummy() {
    platform_ = std::make_unique<Platform>();

    object_ = std::make_unique<Object3d>();
    object_->Create();
    object_->SetMesh(ModelManager::GetModel("mummy"));
    object_->SetTemperature(0.2f);
    coffinModel_ = ModelManager::GetModel("coffin");
    aniObj_ = std::make_unique<AnimationObject3d>();
    aniObj_->Create();
    aniObj_->SetTemperature(0.1f);
    //ペアレント
    object_->worldTransform_.Parent(platform_->GetWorldTransform());
    aniObj_->worldTransform_.Parent(platform_->GetWorldTransform());

    skinningModel_ = std::make_unique<SkinningModel>();
    skinningModel_->CreateDatas(coffinModel_, coffinModel_);
    aniObj_->SetMeshAndData(skinningModel_.get());

    SetCollisionAttribute(kCollisionWall); // ミイラの衝突属性
    SetCollisionMask(kCollisionPlayer | kCollisionEnemy|kCollisionMummy); // プレイヤーや壁と衝突

    SetAABB({ {-1.0f, 0.0f, -0.25f}, {1.0f, 0.5f, 0.25f} });
    SetWorldMatrix(aniObj_->worldTransform_.matWorld_);

}

void Mummy::Initialize() {
    isOpen_ = false;
    isOpenEnd_ = false;
    platform_->Initialize();

    aniObj_->Initialize();
    object_->Initialize();
    object_->worldTransform_.rotate_.y = 1.57f;
    aniObj_->worldTransform_.rotate_.y = 1.57f;
    // 台の高さを取得してオフセット 
    float platformHeight = platform_->GetAABB().max.y-platform_->GetAABB().min.y;
    object_->worldTransform_.translate_.y = platformHeight;
    aniObj_->worldTransform_.translate_.y = platformHeight;


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
