#pragma once
#include"Player/Player.h"
#include"CollisionManager.h"
#include"ItemManager/ItemManager.h"
#include"../GameObject/Memo/MemoManager.h"
#include"UI/UIManager.h"
#include"LightingManager.h"
#include"LevelEditor/LevelEditor.h"

class Camera;
class Stage
{
protected:
    static Player* player_;
    static ItemManager* itemManager_;
    static MemoManager* memoManager_;
    static UIManager* uiManager_;
    static LightingManager* lightingManager_;

    //レベルエディタ
    std::vector<std::unique_ptr<LevelEditor::ObjectSet>>objects_;
    // 1. トリガーを管理するリストを用意
    std::vector<std::unique_ptr<StageChangeTrigger>> stageTriggers_;
    bool isInitialize_ = false;
protected:
    void LoadAndCreateObject(const std::string& fileName );
    void UpdateObject();
    void DrawObject(Camera& camera);
    void AddObjectCollision(CollisionManager& collisionManager);
public:
    Stage();
   virtual ~Stage();
    static void SetMemoManager(MemoManager* memoManager) { memoManager_ = memoManager; };
    static void SetItemManager(ItemManager* itemManager) { itemManager_ = itemManager;};
    static void SetPlayer(Player* player) { player_ = player; };
    static void SetUIManager(UIManager* uiManager) { uiManager_ = uiManager; };
    static void SetLightingManager(LightingManager* lightingManager) { lightingManager_ = lightingManager; }
    void ResetInitializeFlag();
    virtual void CheckCollision(CollisionManager& collisionManager) = 0;
    virtual void Initialize() = 0;
    //ステージ遷移時初期化
    virtual void StageTransitionInitialize() = 0;
    virtual void Update() = 0;
    virtual void Draw(Camera& camera) = 0;
    virtual void DrawSprite() {};
};

