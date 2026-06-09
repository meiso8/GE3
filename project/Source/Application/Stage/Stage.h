#pragma once
#include"Player/Player.h"
#include"CollisionManager.h"
#include"Item/ItemManager.h"
#include"../GameObject/Memo/MemoManager.h"
#include"UI/UIManager.h"
#include"LightingManager.h"

class Camera;
class Stage
{
protected:
    static Player* player_;
    static ItemManager* itemManager_;
    static MemoManager* memoManager_;
    static UIManager* uiManager_;
    static LightingManager* lightingManager_;
public:
    Stage();
    static void SetMemoManager(MemoManager* memoManager) { memoManager_ = memoManager; };
    static void SetItemManager(ItemManager* itemManager) { itemManager_ = itemManager;};
    static void SetPlayer(Player* player) { player_ = player; };
    static void SetUIManager(UIManager* uiManager) { uiManager_ = uiManager; };
    static void SetLightingManager(LightingManager* lightingManager) { lightingManager_ = lightingManager; }

    virtual void CheckCollision(CollisionManager& collisionManager) = 0;
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw(Camera& camera) = 0;
    virtual void DrawSprite() {};
};

