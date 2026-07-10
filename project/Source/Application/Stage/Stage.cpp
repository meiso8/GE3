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

void Stage::DrawObject(Camera& camera)
{

    //オブジェクトの描画
    for (auto& obj : objects_) {
        obj->obj_->Draw(camera);
    }

    //ステージトリガーの描画
    for (auto& trigger : stageTriggers_) {
        trigger->Draw(camera);
    }
}

void Stage::AddObjectCollision(CollisionManager& collisionManager)
{
    //ステージ移動トリガーとの衝突
    for (auto& trigger : stageTriggers_) {
        collisionManager.AddCollider(trigger.get());
    }
}

Stage::Stage()
{
}

Stage::~Stage()
{

}
