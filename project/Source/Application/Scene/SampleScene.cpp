#define NOMINMAX
#include "SampleScene.h"

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

#include"../Stage/MummyStage/MummyStage.h"
#include"../Stage/WaterStage/WaterStage.h"
#include"../Stage/MedjedStage/MedjedStage.h"
#include"../Stage/AmenStage/AmenStage.h"


SampleScene::SampleScene()
{
    // 現在のカメラを設定
    currentCamera_ = camera_.get();

    //プレイヤーのインスタンスを生成
    player_ = std::make_unique<Player>();
    //ライトマネージャーのインスタンスを生成
    lightingManager_ = std::make_unique<LightingManager>();
    lightingManager_->playerHandPos_.Parent(player_->GetEyeWorldTransform());
    lightingManager_->direction_ = &player_->GetForward();

    //衝突判定
    collisionManager_ = std::make_unique<CollisionManager>();
    //アイテム管理
    itemManager_ = std::make_unique<ItemManager>();
    //UI管理
    uIManager_ = std::make_unique<UIManager>();
    //メモ管理
    memoManager_ = std::make_unique<MemoManager>();

    //ステージマネージャーのインスタンスを取得する
    stageManager_ = StageManager::GetInstance();
    //プレイヤーをセットする
    stageManager_->SetPlayer(player_.get());
    //メモ管理セット
    stageManager_->SetMemoManager(memoManager_.get());
    //アイテム管理セット
    stageManager_->SetItemManager(itemManager_.get());
    //カーソルのポジションをセットする
    stageManager_->SetUIManager(uIManager_.get());
    //ライト管理をセットする
    stageManager_->SetLightingManager(lightingManager_.get());
    stageManager_->SetSceneChange(sceneChange_.get());

    stageManager_->SetMap("AmenStage", std::move(std::make_unique<AmenStage>()));
    stageManager_->SetMap("WaterStage", std::move(std::make_unique<WaterStage>()));
    stageManager_->SetMap("MedjedStage", std::move(std::make_unique<MedjedStage>()));
    stageManager_->SetMap("MummyStage", std::move(std::make_unique<MummyStage>()));
    /*   stageManager_->SetNestStage("AmenStage");*/


    skyboxObject3d_ = std::make_unique<SkyboxObject3d>();
    skyboxObject3d_->Create();
}

void SampleScene::Initialize() {

    //パーティクルのリセット
    ParticleManager::ResetAll();
    //ライト管理の初期化
    lightingManager_->Initialize();
    //カメラを初期化する
    camera_->Initialize();
    camera_->UpdateMatrix();
    //UI管理の初期化
    uIManager_->Initialize();
    //アイテム管理の初期化
    itemManager_->Init();
    //メモマネージャー
    memoManager_->Initialize();
    //アメンステージにする
    stageManager_->SetNestStage("AmenStage");
//#ifdef _DEVELOP
//    stageManager_->SetNestStage("MummyStage");
//#endif
    sceneChange_->Initialize();
 /*   sceneChange_->SetState(SceneChange::kFadeOut, 1.0f);*/
    //現在のステージの初期化
    stageManager_->TransitionStage();
}

void SampleScene::Update() {

    //デバック処理
    Debug();

    lightingManager_->UpdatePointLight();

    if (isDebugCameraActive_) {
        currentCamera_->UpdateMatrix();
    } else {
        camera_->worldMat_ = player_->GetEyeMatrix();
        camera_->fovAngleY_ = Easing::EaseOutBack(camera_->kFovAngle_, camera_->kFovAngle_ * 0.5f, player_->zoomTimer_);
        camera_->UpdateViewProjectionMatrix();
    }

    if (PauseScreen::isBackToTitle) {
        BackToTitle();
    }

    if (!PauseScreen::isActive_ && !sceneChange_->IsStateTransition()) {
        //最初は移動しない

        //アクティブなら更新しない
        player_->Update();

        //プレイヤーが死んだら初期化する
        if (player_->IsDead()) {
            // ステージごとの初期化
            stageManager_->Initialize();
        }
    }

    //ステージの更新処理
    stageManager_->Update();

    // 共通更新
    ParticleManager::GetInstance()->Update(*currentCamera_);

    itemManager_->Update();
    uIManager_->UpdatePauseScreen();
    memoManager_->Update();

    CheckAllCollision();
}

SampleScene::~SampleScene()
{
    camera_ = nullptr;
}

void SampleScene::CheckAllCollision()
{

    if (PauseScreen::isActive_) {
        //ポーズ中はコライダーヒットしない
        return;
    }

    // ========================//Ray================================

    //アイテムがヒットしているか
    auto hitItem = itemManager_->RaycastHitItem(*player_->raySprite_);
    if (hitItem) { itemManager_->GetItemSlot().OnTriggerItemPickup(hitItem); }

    //メモがヒットしているかどうか
    memoManager_->RayCastHit(*player_->raySprite_);
    // ========================//Ray================================
    //めもとの当たり判定
    for (auto& [texture, memo] : memoManager_->GetMemos()) {
        collisionManager_->AddCollider(memo.get());
    }

    collisionManager_->ClearColliders();

    //プレイヤーのコライダーを追加する
    collisionManager_->AddCollider(player_.get());
    collisionManager_->AddCollider(player_->GetEyeCollider());

    stageManager_->CheckCollision(*collisionManager_);

    collisionManager_->CheckAllCollisions();

}

void SampleScene::BackToTitle()
{
    sceneChange_->SetState(SceneChange::kFadeIn, 1.0f);
    SceneManager::SetNextScene("Title");
}

void SampleScene::SetSceneChange()
{
    sceneChange_->Initialize();
    sceneChange_->SetState(SceneChange::kFadeOut, 1.0f);
}

void SampleScene::Debug()
{

    if (Input::IsTriggerKey(DIK_V)) {
        stageManager_->SetNestStage("MedjedStage");
    }

#ifdef USE_IMGUI

    ImGui::Begin("Debug");

    if (Input::IsTriggerKey(DIK_1)) {
        SwitchCamera();
    }

    ImGui::Text("SwitchCamera : 1 key");

    DebugUI::CheckFlag(isDebugCameraActive_, "isDebugCameraAvtive");
    std::function<void()> func = [this]() { SwitchCamera(); };
    DebugUI::Button("ChangeCamera", func);
    DebugUI::CheckCamera(*currentCamera_);

    const char* stages[] = { "AmenStage", "WaterStage", "MedjedStage","MummyStage" };
    int stageCurrent = 0;

    if (ImGui::Combo("CurrentStage", &stageCurrent, stages, IM_ARRAYSIZE(stages))) {
        stageManager_->SetNestStage(stages[stageCurrent % 4]);
    };

    ImGui::End();
    //プレイヤーのデバッグ
    player_->Debug();

#endif // !USE_IMGUI
}

void SampleScene::DrawModel() {

#ifdef _DEVELOP
    DrawGrid::Draw(*currentCamera_);
#endif

    skyboxObject3d_->Draw(*currentCamera_);

    memoManager_->Draw(*currentCamera_);
    itemManager_->Draw(*currentCamera_);
    //ステージごとの描画
    stageManager_->DrawModel(currentCamera_);
    //プレイヤーの描画
    player_->Draw(*currentCamera_);

    ParticleManager::GetInstance()->Draw();
    //アイテムを手前に描画する
    itemManager_->DrawGetItem();


}

void SampleScene::DrawSprite() {


    Sprite::PreDraw();

    itemManager_->DrawUI();

    uIManager_->DrawPauseScreen();
    //ステージごとのスプライトを描画する
    stageManager_->DrawSprite();

    memoManager_->DrawUI();
    uIManager_->DrawCurPos();
    player_->DrawRaySprite();

    uIManager_->DrawEffect();

    sceneChange_->Draw();


}