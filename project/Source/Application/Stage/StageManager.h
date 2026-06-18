#pragma once
#include"Stage.h"
#include"SceneManager/SceneChange.h"

class StageManager
{
private:
    StageManager() = default;
    ~StageManager();
public:

    // 2. コピーと代入を禁止する（インスタンスが2つに増えるのを防ぐため）
    StageManager(const StageManager&) = delete;
    StageManager& operator=(const StageManager&) = delete;
    // 3. インスタンスを取得するための GetInstance 関数（必ず static にする）
    static StageManager* GetInstance()
    {
        static StageManager instance;
        return &instance;
    }
    void Initialize();
    void Update();
    void DrawModel(Camera* camera);
    void DrawSprite();
    void TransitionStage();
    void TransitionScene(const std::string& sceneName);
    void SetMap(const std::string& name, std::unique_ptr<Stage> stage);
    void SetNestStage(const std::string& name);
    void CheckCollision(CollisionManager& collisionManager);
    void SetSceneChange(SceneChange* sceneChange) { sceneChange_ = sceneChange; }
    void SetMemoManager(MemoManager* memoManager);
    void SetItemManager(ItemManager* itemManager);
    void SetUIManager(UIManager* uiManager);
    void SetPlayer(Player* player);
    void SetLightingManager(LightingManager* lightingManager);
private:

    MemoManager* memoManager_ = nullptr;
    SceneChange* sceneChange_ = nullptr;
    Stage* currentStage_ = nullptr;
    Stage* nextStage_ = nullptr;
    std::map < std::string, std::unique_ptr<Stage>> stages_;
};

