#include "StageManager.h"
#include"Log.h"
#include"SceneManager.h"
#include"ObjectManager/ObjectManager.h"
#include"ItemManager/ItemManager.h"
#include"CollisionManager.h"
#include"SoundManager/SoundManager.h"
#include"Particle/Particle.h"

StageManager::~StageManager()
{
    LogFile::Log("Destory StageManager");
}

void StageManager::Finalize()
{
    stageNames_.clear();
    stages_.clear();
    currentStage_ = nullptr;
    nextStage_ = nullptr;
}

void StageManager::Initialize()
{

    //メモマネージャー
    memoManager_->Initialize();
    LogFile::Log("Initialize　MemoManager");

    for (auto& [name, stage] : stages_) {
        stage->ResetInitializeFlag();
        LogFile::Log("ResetInitializeFlag");
    }
}

void StageManager::Update()
{

    TransitionStage();

    if (currentStage_) {
        currentStage_->Update();
        currentStage_->UpdateObject();
    }

}

void StageManager::DrawModel(Camera* camera)
{
    if (currentStage_) {
        //一旦二回描画しておく
        currentStage_->DrawObject(*camera);
        currentStage_->Draw(*camera);
        currentStage_->DrawObject(*camera);
    }
}

void StageManager::DrawSprite()
{
    if (currentStage_) {
        currentStage_->DrawSprite();
    }
}

void StageManager::TransitionStage()
{

    //次のステージがセットされていたら次のステージにする
    if (nextStage_) {

        collisionManager_->ClearColliders();
        //コマンドを初期化する
        ObjectManager::GetInstance()->Initialize();
        //オブジェクトをクリアする
        ObjectManager::GetInstance()->Clear();

        sceneChange_->Initialize();
        sceneChange_->SetState(SceneChange::kFadeOut, 1.0f);
        LogFile::Log("Start FadeOut");
        //メモマネージャー
        memoManager_->Initialize();
        LogFile::Log("Initialize MemoManager");
        
        //サウンド管理
        SoundManager::InitSound();
        //パーティクルのリセット
        ParticleManager::ResetAll();
        
        currentStage_ = nextStage_;
        currentStage_->Initialize();
       
        nextStage_ = nullptr;
        LogFile::Log("Initialize NextScene");
    }

}

void StageManager::TransitionScene(const std::string& sceneName)
{
    //メジェドを倒したらシーン切り替え
    sceneChange_->SetState(SceneChange::kFadeIn, 1.0f);
    SceneManager::SetNextScene(sceneName);

}

void StageManager::SetMap(const std::string& name, std::unique_ptr<Stage> stage)
{
    stages_[name] = std::move(stage);
    stageNames_[stages_[name].get()] = name;
}

void StageManager::SetNestStage(const std::string& name)
{
    // すでに遷移予約が入っている場合は重複予約しない
    if (nextStage_ != nullptr) return;
    auto it = stages_.find(name);
    if (it != stages_.end()) {
        nextStage_ = it->second.get();
    }
    ////最初の位置を保持
    //nextStage_ = stages_[name].get();
}

void StageManager::CheckCollision()
{
    if (currentStage_&& collisionManager_) {
        currentStage_->CheckCollision(*collisionManager_);
        currentStage_-> AddObjectCollision(*collisionManager_);
    }
}


void StageManager::SetMemoManager(MemoManager* memoManager)
{
    memoManager_ = memoManager;
    assert(memoManager_);
    Stage::SetMemoManager(memoManager);
}

void StageManager::SetItemManager(ItemManager* itemManager)
{
    Stage::SetItemManager(itemManager);
}

void StageManager::SetUIManager(UIManager* uiManager)
{
    Stage::SetUIManager(uiManager);
}

void StageManager::SetPlayer(Player* player)
{
    Stage::SetPlayer(player);
}

void StageManager::SetCollisionManager(CollisionManager* collisionManger)
{
    collisionManager_ = collisionManger;
}

void StageManager::SetLightingManager(LightingManager* lightingManager)
{
    Stage::SetLightingManager(lightingManager);
}

