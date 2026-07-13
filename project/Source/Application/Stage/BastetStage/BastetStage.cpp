#include"BastetStage.h"
#include "ParticleEmitter.h"
BastetStage::BastetStage()
{
    backGround_ = std::make_unique<BackGround>();
    bastet_ = std::make_unique<Bastet>();
}

void BastetStage::Initialize()
{

    //ステージのロード
    LoadAndCreateObject("MeltStage_objectEditor");
    //ライトの初期化
    lightingManager_->Initialize();
    //パーティクルのリセット
    ParticleManager::ResetAll();

    backGround_->Initialize();

    //少し手前側に移動する
    player_->Init({ 0.0f, 0.0f, -5.0f });
}

void BastetStage::Update()
{
  
    backGround_->Update();
   
    //オブジェクトの更新
    UpdateObject();

}

void BastetStage::Draw(Camera& camera)
{
    backGround_->Draw(camera);

    //オブジェクトの描画
    DrawObject(camera);
}

void BastetStage::CheckCollision(CollisionManager& collisionManager)
{

    //meltBlockMap_->RayCastHit(*player_->GerRaySprite());

    // 壁との当たり判定
    for (auto& [type, object] : backGround_->GetBuilding()->GetFieldPoses()) {
        collisionManager.AddCollider(object.get());
    }

    //コライダーを追加する
    AddObjectCollision(collisionManager);
}