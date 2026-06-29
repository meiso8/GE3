#pragma once
#include"SceneManager.h"

#include<memory>
#include<array>

#include"Object3d.h"
#include"SkyBoxObject3d.h"

#pragma region//ゲームオブジェクト
#include"Player/Player.h"
#include"UI/UIManager.h"
#include"LightingManager.h"
#include"Item/ItemManager.h"
#include "Memo/MemoManager.h" 
#pragma endregion
#include"CollisionManager.h"

#include"../Stage/StageManager.h"

class Sprite;
class PlaneMesh;
class SphereMesh;
class CubeMesh;
class Model;

class SampleScene :public BaseScene {

public:
    SampleScene();
    ~SampleScene() override;
    void Initialize()override;
    void Update()override;
    void DrawModel()override;
    void DrawSprite()override;
    void CheckAllCollision();
    void BackToTitle();
    void SetSceneChange();

private:
#pragma region//ゲームオブジェクト
    std::unique_ptr<Player>player_ = nullptr;
    std::unique_ptr<LightingManager>lightingManager_ = nullptr;
    std::shared_ptr<ItemManager> itemManager_;
    std::unique_ptr<UIManager> uIManager_ = nullptr;
    std::unique_ptr<MemoManager>memoManager_ = nullptr;
#pragma endregion

#pragma region//SkyBox
    std::unique_ptr<SkyboxObject3d>skyboxObject3d_ = nullptr;
#pragma endregion

#pragma region//ステージ
    StageManager* stageManager_ = nullptr;

    //レベルエディタ
    std::unique_ptr<LevelEditor> levelEditor_ = nullptr;
    std::vector<std::unique_ptr<LevelEditor::ObjectSet>>objects_;
    // 1. トリガーを管理するリストを用意
    std::vector<std::unique_ptr<StageChangeTrigger>> stageTriggers_;

#pragma endregion
    //衝突判定
    std::unique_ptr<CollisionManager>collisionManager_ = nullptr;
private:
    void Debug();
};
