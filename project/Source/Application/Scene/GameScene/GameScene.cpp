#define NOMINMAX
#include "GameScene.h"

//入力処理に必要なもの
#include "Input.h"
//Debug用のImGui表示セット

#include"DebugUI.h"
//グリッド表示
#include"DrawGrid.h"
#include "ParticleEmitter.h"
#include"Particle.h"

#include"Easing.h"
#include"SoundManager/SoundManager.h"
#include"UI/PauseScreen.h"

#include"SceneManager.h"


#include"../Stage/InformationStage/InformationStage.h"
#include"../Stage/MummyStage/MummyStage.h"
#include"../Stage/WaterStage/WaterStage.h"
#include"../Stage/MedjedStage/MedjedStage.h"
#include"../Stage/AmenStage/AmenStage.h"
#include"../Stage/AnubisStage/AnubisStage.h"
#include"../Stage/MeltStage/MeltStage.h"
#include"../Stage/BastetStage/BastetStage.h"
#include"ObjectManager/ObjectManager.h"
GameScene::GameScene()
{
    // 現在のカメラを設定
    currentCamera_ = camera_.get();

    //プレイヤーのインスタンスを生成
    player_ = std::make_unique<Player>();

    //ライトマネージャーのインスタンスを生成
    lightingManager_ = std::make_unique<LightingManager>();

    lightingManager_->GetPlayerHandPos().Parent(player_->GetEyeWorldTransform());
    lightingManager_->SetDirection(&player_->GetEyeForward());

    //衝突判定
    collisionManager_ = std::make_unique<CollisionManager>();
    //アイテム管理
    itemManager_ = std::make_unique<ItemManager>();
    //プレイヤーの手の行列
    itemManager_->SetPlayerHandMatrix(player_->GetHandMatrixPtr());
    //UI管理
    uIManager_ = std::make_unique<UIManager>();
    //メモ管理
    memoManager_ = std::make_unique<MemoManager>();

    //ステージマネージャーのインスタンスを取得する
    auto* stageManager = StageManager::GetInstance();

    

    //プレイヤーをセットする
    stageManager->SetPlayer(player_.get());
    //メモ管理セット
    stageManager->SetMemoManager(memoManager_.get());
    //アイテム管理セット
    stageManager->SetItemManager(itemManager_.get());
    //カーソルのポジションをセットする
    stageManager->SetUIManager(uIManager_.get());
    //ライト管理をセットする
    stageManager->SetLightingManager(lightingManager_.get());
    stageManager->SetSceneChange(sceneChange_.get());
    stageManager->SetCollisionManager(collisionManager_.get());

    stageManager->SetMap("InformationStage", std::move(std::make_unique<InformationStage>()));
    stageManager->SetMap("AmenStage", std::move(std::make_unique<AmenStage>()));
    stageManager->SetMap("WaterStage", std::move(std::make_unique<WaterStage>()));
    stageManager->SetMap("MedjedStage", std::move(std::make_unique<MedjedStage>()));
    stageManager->SetMap("MummyStage", std::move(std::make_unique<MummyStage>()));
    stageManager->SetMap("AnubisStage", std::move(std::make_unique<AnubisStage>()));
    stageManager->SetMap("MeltStage", std::move(std::make_unique<MeltStage>()));
    stageManager->SetMap("BastetStage", std::move(std::make_unique<BastetStage>()));

    //スカイボックス
    skyboxObject3d_ = std::make_unique<SkyboxObject3d>();
    skyboxObject3d_->Create();

    SoundManager::SetCollisionManager(collisionManager_.get());
    SoundManager::SetRaySprite(player_->GerRaySprite());
}

void GameScene::Initialize() {

    //パーティクルのリセット
    ParticleManager::ResetAll();
    //ライト管理の初期化
    lightingManager_->Initialize();

    //UI管理の初期化
    uIManager_->Initialize();
    //アイテム管理の初期化
    itemManager_->Init();
    //メモマネージャー
    memoManager_->Initialize();
    //アメンステージにする

    auto* stageManager = StageManager::GetInstance();

    stageManager->SetNestStage("InformationStage");
#ifdef _DEVELOP
    //stageManager->SetNestStage("InformationStage");

#endif

    //カメラを初期化する
    camera_->Initialize();
    camera_->UpdateMatrix();

    //ステージ遷移の初期化
    sceneChange_->Initialize();

    stageManager->Initialize();
    //現在のステージの初期化
    stageManager->TransitionStage();
}

void GameScene::Update() {

    //デバック処理
    Debug();

    lightingManager_->UpdatePointLight();

    if (isDebugCameraActive_) {
        currentCamera_->UpdateMatrix();
    } else {
        //プレイヤーの目の位置をカメラの位置とする
        camera_->SetWorldMatrix(player_->GetEyeMatrix());
        camera_->SetFovAngleY(Easing::EaseOutBack(Camera::kFovAngle, Camera::kFovAngle * 0.5f, player_->GetZoomTimer()));
        camera_->UpdateViewProjectionMatrix();
    }

    if (PauseScreen::GetIsBackToTitle()) {
        BackToTitle();
    }



    auto* stageManager = StageManager::GetInstance();



    if (!PauseScreen::GetIsActive() && !sceneChange_->IsStateTransition()) {
        //最初は移動しない


    //アクティブなら更新しない
        player_->Update();

        //プレイヤーが死んだら初期化する
        if (player_->IsDead()) {
            // ステージごとの初期化
            stageManager->Initialize();
        }
    }

    //ステージの更新処理
    stageManager->Update();

    itemManager_->Update();
    uIManager_->UpdatePauseScreen();
    memoManager_->Update();

    CheckAllCollision();

    SoundManager::Update();
}

GameScene::~GameScene()
{
    collisionManager_->Finalize();
    StageManager::GetInstance()->Finalize();
}

void GameScene::CheckAllCollision()
{

    if (PauseScreen::GetIsActive()) {
        //ポーズ中はコライダーヒットしない
        return;
    }


#ifdef _DEVELOP
    collisionManager_->UpdateAllCollider();
#endif

    //// ========================//Ray================================

    //アイテムがヒットしているか
    auto hitItem = itemManager_->RaycastHitItem(*player_->GerRaySprite());
    if (hitItem) { itemManager_->GetItemSlot().OnTriggerItemPickup(hitItem); }

    //メモがヒットしているかどうか
    memoManager_->RayCastHit(*player_->GerRaySprite());
    //// ========================//Ray================================

    collisionManager_->ClearColliders();

    //めもとの当たり判定
    //for (auto& [texture, memo] : memoManager_->GetMemos()) {
    //    collisionManager_->AddCollider(memo.get());
    //}

    //プレイヤーのコライダーを追加する
    collisionManager_->AddCollider(player_.get());

    StageManager::GetInstance()->CheckCollision();
    //ここに入れるかどうかは迷い中
    ObjectManager::GetInstance()->CheckCollision(collisionManager_.get());

    collisionManager_->CheckAllCollisions();

}

void GameScene::BackToTitle()
{
    sceneChange_->SetState(SceneChange::kFadeIn, 1.0f);
    SceneManager::SetNextScene("Title");
}

void GameScene::SetSceneChange()
{
    sceneChange_->Initialize();
    sceneChange_->SetState(SceneChange::kFadeOut, 1.0f);
}

void GameScene::Debug()
{

#ifdef USE_IMGUI

    ImGui::Begin("Debug");

    if (ImGui::Button("SwitchCamera")) {
        SwitchCamera();
    };

    DebugUI::CheckFlag(isDebugCameraActive_, "isDebugCameraAvtive");
    //ステージの確認
    DebugUI::CheckStageManager();

    //太陽円盤取得フラグ
    bool isGetDisc = ItemManager::IsGetSolarDisc();
    ImGui::Checkbox("isGetDisc", &isGetDisc);
    ItemManager::SetIsGetSolarDisc(isGetDisc);
    //コライダーの描画フラグ
    bool isColliderDraw =collisionManager_->GetIsDraw();
    ImGui::Checkbox("isColliderDraw", &isColliderDraw);
    collisionManager_->SetIsDraw(isColliderDraw);

    ImGui::End();
    //プレイヤーのデバッグ
    player_->Debug();


#endif // !USE_IMGUI


}

void GameScene::DrawModel() {

#ifdef _DEVELOP
    DrawGrid::Draw(*currentCamera_);
#endif

    skyboxObject3d_->Draw(*currentCamera_);
    memoManager_->Draw(*currentCamera_);
    itemManager_->Draw(*currentCamera_);

    //ステージごとの描画
    StageManager::GetInstance()->DrawModel(currentCamera_);

    //プレイヤーの描画
    player_->Draw(*currentCamera_);

    //アイテムを手前に描画する
    itemManager_->DrawItemSlotItem();

#ifdef _DEVELOP
    collisionManager_->DrawAllCollider(currentCamera_);
#endif
}

void GameScene::DrawSprite() {

    Sprite::PreDraw();

    itemManager_->DrawUI();
    uIManager_->DrawPauseScreen();
    //ステージごとのスプライトを描画する
    StageManager::GetInstance()->DrawSprite();

    memoManager_->DrawUI();
    uIManager_->DrawCurPos();
    player_->DrawRaySprite();

    uIManager_->DrawEffect();
    sceneChange_->Draw();

}