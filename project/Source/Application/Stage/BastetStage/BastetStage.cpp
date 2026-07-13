#include"BastetStage.h"
#include "ParticleEmitter.h"
BastetStage::BastetStage()
{
    backGround_ = std::make_unique<BackGround>();
    bastet_ = std::make_unique<Bastet>();
    bastetBlockMap_ = std::make_unique<BastetBlockMap>();
}

void BastetStage::Initialize()
{

    //ステージのロード
    LoadAndCreateObject("BastetStage_objectEditor");
    //ライトの初期化
    lightingManager_->Initialize();
    //パーティクルのリセット
    ParticleManager::ResetAll();
    //背景の初期化
    backGround_->Initialize();
   
    //ブロックマップ
    bastetBlockMap_->Initialize();
    //バステト　
    bastet_->LoadMap("BastetStage_BastetStep");
    bastet_->Initialize();
    //少し手前側に移動する
    player_->Init({ 0.0f, 0.0f, -5.0f });
}

void BastetStage::Update()
{

    backGround_->Update();
    bastetBlockMap_->Update();
    
    bastet_->Update();

    if (bastetBlockMap_->IsClear()) {

    }

    //オブジェクトの更新
    UpdateObject();

}

void BastetStage::Draw(Camera& camera)
{
    backGround_->Draw(camera);
    bastetBlockMap_->Draw(camera);

    //オブジェクトの描画
    DrawObject(camera);
    //バステトの描画
    bastet_->Draw(camera);
}

void BastetStage::CheckCollision(CollisionManager& collisionManager)
{

    bastetBlockMap_->RayCastHit(*player_->GerRaySprite());

    // 壁との当たり判定
    for (auto& [type, object] : backGround_->GetBuilding()->GetFieldPoses()) {
        collisionManager.AddCollider(object.get());
    }

    //ブロックの当たり判定の追加
    for (auto& object : bastetBlockMap_->GetMap()) {
        collisionManager.AddCollider(object.get());
    }

    //コライダーを追加する
    AddObjectCollision(collisionManager);
}