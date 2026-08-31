#include "Stage.h"


Player* Stage::player_ =nullptr;
ItemManager* Stage::itemManager_ = nullptr;
MemoManager* Stage::memoManager_ = nullptr;
UIManager* Stage:: uiManager_ = nullptr;
LightingManager* Stage::lightingManager_ = nullptr;

void Stage::LoadAndCreateObject(const std::string& fileName)
{
    auto* levelEditor = LevelEditor::GetInstance();
    levelEditor->Load(fileName, true);
    //オブジェクトの生成
    levelEditor->CreateObject(objects_);
    // ★ステージ遷移トリガーの生成と配置
    levelEditor->CreateStageChangeTriggers(stageTriggers_);
}

void Stage::UpdateObject()
{

    for (auto& obj : objects_) {
        obj->obj_->Update();
    }

    for (auto& trigger : stageTriggers_) {
        trigger->Update();
    }
}

void Stage::DrawObject()
{

    //オブジェクトの描画
    for (auto& obj : objects_) {
        obj->obj_->Draw();
    }

    //ステージトリガーの描画
    for (auto& trigger : stageTriggers_) {
        trigger->Draw();
    }
}

void Stage::AddObjectCollision(CollisionManager& collisionManager)
{
    //ステージ移動トリガーとの衝突
    for (auto& trigger : stageTriggers_) {
        collisionManager.AddCollider(trigger.get());
    }

    for (auto& obj : objects_) {
        //オブジェクト内のコライダーを追加
        collisionManager.AddCollider(obj->collider_.get());
    }

}

Stage::Stage()
{
}

Stage::~Stage()
{

}

void Stage::ResetInitializeFlag()
{
    isInitialize_ = false;
}
