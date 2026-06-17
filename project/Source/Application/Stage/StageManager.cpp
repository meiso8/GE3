#include "StageManager.h"
#include"Log.h"
#include"SceneManager.h"


StageManager::~StageManager()
{
    stages_.clear();
    currentStage_ = nullptr;
    nextStage_ = nullptr;
    LogFile::Log("Destory StageManager");
}

void StageManager::Initialize()
{

    //メモマネージャー
    memoManager_->Initialize();
    LogFile::Log("InitializeMemoManager");
    currentStage_->Initialize();
    LogFile::Log("InitializeNextScene");

}

void StageManager::Update()
{

    TransitionStage();

    if (currentStage_) {
        currentStage_->Update();
    }

}

void StageManager::DrawModel(Camera* camera)
{
    if (currentStage_) {
        currentStage_->Draw(*camera);
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
        sceneChange_->SetState(SceneChange::kFadeOut,1.0f);
        LogFile::Log("Start FadeOut");
        //メモマネージャー
        memoManager_->Initialize();
        LogFile::Log("InitializeMemoManager");
        currentStage_ = nextStage_;
        currentStage_->Initialize();
        nextStage_ = nullptr;
        LogFile::Log("InitializeNextScene");
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
}

void StageManager::SetNestStage(const std::string& name)
{
    //最初の位置を保持
    nextStage_ = stages_[name].get();
}

void StageManager::CheckCollision(CollisionManager& collisionManager)
{
    if (currentStage_) {
        currentStage_->CheckCollision(collisionManager);
    }
}

void StageManager::SetSceneChange(SceneChange* sceneChange)
{
    sceneChange_ = sceneChange;
    assert(sceneChange_);
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

void StageManager::SetLightingManager(LightingManager* lightingManager)
{
    Stage::SetLightingManager(lightingManager);
}

