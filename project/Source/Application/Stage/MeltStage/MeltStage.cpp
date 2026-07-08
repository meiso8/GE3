#include "MeltStage.h"

MeltStage::MeltStage()
{
    backGround_ = std::make_unique<BackGround>();
}

void MeltStage::Initialize()
{

    //少し手前側に移動する
    player_->Init({ 0.0f, 0.0f, -5.0f });
}

void MeltStage::Update()
{

    backGround_->Update();

    //オブジェクトの更新
    UpdateObject();
}

void MeltStage::Draw(Camera& camera)
{
    backGround_->Draw(camera);
    //オブジェクトの描画
    DrawObject(camera);
}

void MeltStage::CheckCollision(CollisionManager& collisionManager)
{    // 壁との当たり判定
    for (auto& [type, object] : backGround_->GetBuilding()->GetFieldPoses()) {
        collisionManager.AddCollider(object.get());
    }


    //コライダーを追加する
    AddObjectCollision(collisionManager);
}
