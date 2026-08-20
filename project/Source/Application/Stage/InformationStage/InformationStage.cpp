#include "InformationStage.h"


InformationStage::InformationStage()
{
    room_ = std::make_unique<InformationRoom>();
}

void InformationStage::Initialize()
{

    StageTransitionInitialize();

    if (isInitialize_) {
        return;
    }
    room_->Init();
    room_->Update();

    isInitialize_ = true;
}

void InformationStage::StageTransitionInitialize()
{

    //ステージのロード
    LoadAndCreateObject("Information_objectEditor");

    memoManager_->GenerateMemos({ });
    // ミイラ前に移動
    player_->Init({ 3.25f, 0.0f, -3.25f });

}

void InformationStage::Update()
{
    room_->Update();
}

void InformationStage::Draw(Camera& camera)
{
    room_->Draw(camera);

}

void InformationStage::CheckCollision(CollisionManager& collisionManager)
{

    //かべ
    for (auto& [type, object] : room_.get()->GetFieldPoses()) {
        collisionManager.AddCollider(object.get());
    }

}
