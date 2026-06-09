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


SampleScene::SampleScene()
{
    // 現在のカメラを設定
    currentCamera_ = camera_.get();

    player_ = std::make_unique<Player>();

    lightingManager_ = std::make_unique<LightingManager>();
    lightingManager_->playerHandPos_.Parent(player_->GetEyeWorldTransform());
    lightingManager_->direction_ = &player_->GetForward();

    collisionManager_ = std::make_unique<CollisionManager>();

    itemManager_ = std::make_unique<ItemManager>();
    uIManager_ = std::make_unique<UIManager>();
    memoManager_ = std::make_unique<MemoManager>();

    Stage::SetPlayer(player_.get());

    amenStage_ = std::make_unique<AmenStage>();
    waterStage_ = std::make_unique<WaterStage>();
    medjedStage_ = std::make_unique<MedjedStage>();
    mummyStage_ = std::make_unique<MummyStage>();

    // 各ステージに渡す
    amenStage_->SetItemManager(itemManager_);
    amenStage_->SetCurPos(*uIManager_->GetCurPosPtr());

    waterStage_->SetItemManager(itemManager_);
    mummyStage_->SetItemManager(itemManager_);


    medjedStage_->SetItemManager(itemManager_);
    if (medjedStage_) {
        CreateParticle();
        uIManager_->CreateHpGage(*medjedStage_->GetEnemy()->GetHpsPtr(), *player_->GetHpsPtr());
    }

    skyboxObject3d_ = std::make_unique<SkyboxObject3d>();
    skyboxObject3d_->Create();
}

void SampleScene::Initialize() {

    ParticleManager::ResetAll();

    lightingManager_->Initialize();

    SetSceneChange();

    camera_->Initialize();
    //camera_->nearZ_ = 1.0f;
    camera_->UpdateMatrix();

    uIManager_->Initialize();
    itemManager_->Init();
    //メモマネージャー
    memoManager_->Initialize();

    currentPhase_ = StagePhase::Amen;

    InitAmenScene();
}

void SampleScene::InitAmenScene()
{
    //メモマネージャー
    memoManager_->Initialize();
    SetSceneChange();
    memoManager_->GenerateMemos({ TextureFactory::MEMO1, TextureFactory::MEMO3,TextureFactory::MEMO4,TextureFactory::BOOK4 });
    amenStage_->Initialize();
}

void SampleScene::InitWaterScene()
{
    //メモマネージャー
    memoManager_->Initialize();
    SetSceneChange();
    memoManager_->GenerateMemos({ TextureFactory::MEMO2, TextureFactory::BOOK2 });
    waterStage_->Initialize();
}

void SampleScene::InitMummyScene()
{
    //メモマネージャー
    memoManager_->Initialize();
    SetSceneChange();
    memoManager_->GenerateMemos({ TextureFactory::BOOK,TextureFactory::MEMO5 });
    mummyStage_->Initialize();
}

void SampleScene::InitMedjedScene()
{
    if (medjedStage_) {
        particleEmitters_[0]->GetEmitter().transform.Parent(medjedStage_->GetMedjed()->GetWorldTransform());
    }
    //メモマネージャー
    memoManager_->Initialize();
    SetSceneChange();
    ParticleManager::ResetAll();
    SoundManager::InitMedjedScene();
    lightingManager_->Initialize();

    memoManager_->GenerateMemos({ TextureFactory::BOOK3 });
    medjedStage_->Initialize();

}

void SampleScene::Update() {

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


    if (!PauseScreen::isActive_) {
        //アクティブなら更新しない
        player_->Update();

        if (player_->IsDead()) {
            // ステージごとの更新
            switch (currentPhase_) {
            case StagePhase::Amen:
                if (amenStage_) {
                    InitAmenScene();
                }
                break;
            case StagePhase::Water:
                if (waterStage_) {
                    InitWaterScene();
                }
                break;
            case StagePhase::Mummy:
                if (mummyStage_) {
                    InitMummyScene();
                }
                break;
            case StagePhase::Medjed:
                if (medjedStage_) {
                    InitMedjedScene();
                }
                break;
            }

        }
    }

    // ステージごとの更新
    switch (currentPhase_) {
    case StagePhase::Amen:

        if (amenStage_) {
            amenStage_->Update();

            // 水の謎解きクリアで心臓を取得
            if (amenStage_->IsClear()) {
                currentPhase_ = StagePhase::Water;


                InitWaterScene();
            }
        }

        break;
    case StagePhase::Water:
        if (waterStage_) {
            waterStage_->Update();

            // 水の謎解きクリアで心臓を取得
            if (waterStage_->IsClear()) {
                currentPhase_ = StagePhase::Mummy;
                InitMummyScene();
            }
        }
        break;

    case StagePhase::Mummy:
        if (mummyStage_) {
            mummyStage_->Update();

            // 心臓をはめてメジェドが現れたら
            if (mummyStage_->IsEndTime()) {
                currentPhase_ = StagePhase::Medjed;
                InitMedjedScene();
            }
        }
        break;

    case StagePhase::Medjed:
        if (medjedStage_) {
            medjedStage_->Update();

            if (medjedStage_->MedjedDead()) {
                //メジェドを倒したらシーン切り替え
                sceneChange_->SetState(SceneChange::kFadeIn, 60);
                SceneManager::SetNestScene("Result");
            }

            if (medjedStage_->FindMedjed()) {
                lightingManager_->DirectionalLightUpdate();

                if (medjedStage_->GetEnemyApper()) {
                    particleEmitters_[0]->GetEmitter().transform.Parent(medjedStage_->GetEnemy()->GetWorldTransform());
                }

                for (int i = 0; i < particleEmitters_.size(); ++i) {
                    particleEmitters_[i]->UpdateTimer();
                    particleEmitters_[i]->UpdateEmitter();
                }
                uIManager_->UpdateGage();
            }
        }
        break;
    }

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

void SampleScene::Debug()
{

#ifdef USE_IMGUI

    if (Input::IsTriggerKey(DIK_Q)) {
        SwitchCamera();
    }

    ImGui::Text("SwitchCamera : Q key");
    DebugUI::CheckFlag(isDebugCameraActive_, "isDebugCameraAvtive");
    std::function<void()> func = [this]() { SwitchCamera(); };
    DebugUI::CheckCamera(*currentCamera_);

    DebugUI::Button("ChangeCamera", func);
    DebugUI::CheckParticle(*particleEmitters_[0], "Emitter0");

#endif // !USE_IMGUI
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

    collisionManager_->AddCollider(player_.get());
    collisionManager_->AddCollider(player_->GetEyeCollider());

    switch (currentPhase_) {
    case StagePhase::Amen:
        if (amenStage_) amenStage_->CheckCollision(*collisionManager_);
        break;
    case StagePhase::Water:
        if (waterStage_) waterStage_->CheckCollision(*collisionManager_);
        break;
    case StagePhase::Mummy:
        if (mummyStage_) mummyStage_->CheckCollision(*collisionManager_);
        break;
    case StagePhase::Medjed:
        if (medjedStage_) medjedStage_->CheckCollision(*collisionManager_);
        break;
    }

    collisionManager_->CheckAllCollisions();

}

void SampleScene::BackToTitle()
{
    sceneChange_->SetState(SceneChange::kFadeIn, 60);
    SceneManager::SetNestScene("Title");
}

void SampleScene::SetSceneChange()
{
    sceneChange_->Initialize();
    sceneChange_->SetState(SceneChange::kFadeOut, 60);
}

void SampleScene::CreateParticle()
{
    for (int i = 0; i < particleEmitters_.size(); ++i) {
        particleEmitters_[i] = std::make_unique<ParticleEmitter>();
        particleEmitters_[i]->Initialize();
    }

    ParticleManager::GetInstance()->Create();

    particleEmitters_[0]->SetName("medjedParticle");
    particleEmitters_[1]->SetName("people");


    auto& emitter0 = particleEmitters_[0]->GetEmitter();
    emitter0.count = 8;
    emitter0.color = { 1.0f,0.75f,0.75f,1.0f };
    emitter0.frequencyTime = 0.25f;
    emitter0.lifeTime = 6.0f;
    emitter0.blendMode = kBlendModeMultiply;
    emitter0.movement = ParticleMovements::kParticleSphere;
    emitter0.radius = 3.0f;
    float pi = std::numbers::pi_v<float>;
    emitter0.rotateAABB_ = { .min = {-pi ,-pi ,-pi } ,.max = { pi, pi, pi} };
    //particleEmitters_[0]->emitter_.radiusSpeed = ;
    auto& group = ParticleManager::GetInstance()->GetParticleGroup(emitter0.name);
    group->accelerationField.acceleration.y = 5.0f;
    group->accelerationField.area = { .min = {-25.0f,0.0f,-25.0f},.max = {25.0f,40.0f,25.0f} };

    auto& emitter1 = particleEmitters_[1]->GetEmitter();
    emitter1.transform.translate_.y = 30.0f;
    emitter1.transform.scale_ = { 10.0f,10.0f,10.0f };
    emitter1.count = 4;
    emitter1.color = { 1.0f,0.75f,0.75f,1.0f };
    emitter1.frequencyTime = 0.1f;
    emitter1.lifeTime = 10.0f;
    emitter1.blendMode = kBlendModeScreen;
    emitter1.velocityAABB = { { -10.0f,-10.0f,-10.0f }, { 10.0f,0.0f,10.0f } };
    emitter1.rotateAABB_ = { .min = {-pi ,-pi ,-pi } ,.max = { pi, pi, pi} };

    auto& enemyGroup = ParticleManager::GetInstance()->GetParticleGroup(emitter1.name);
    enemyGroup->accelerationField.acceleration.y = 10.0f;
    enemyGroup->accelerationField.area = { .min = {-25.0f,0.0f,-25.0f},.max = {25.0f,15.0f,25.0f} };
}

void SampleScene::DrawModel() {

#ifdef _DEVELOP

    DrawGrid::Draw(*currentCamera_);

#endif
    memoManager_->Draw(*currentCamera_);
    itemManager_->Draw(*currentCamera_);

    switch (currentPhase_) {
    case StagePhase::Amen:
        if (amenStage_) amenStage_->Draw(*currentCamera_);
        break;
    case StagePhase::Water:
        if (waterStage_) waterStage_->Draw(*currentCamera_);
        break;
    case StagePhase::Mummy:
        if (mummyStage_) mummyStage_->Draw(*currentCamera_);
        break;
    case StagePhase::Medjed:
        if (medjedStage_) {
            medjedStage_->Draw(*currentCamera_);

            if (medjedStage_->FindMedjed()) {
                //HPゲー描画
                uIManager_->DrawHPGage();
            }
        };
        break;
    }

    player_->Draw(*currentCamera_, kLightModeHalfL);

    skyboxObject3d_->Draw(*currentCamera_);
    ParticleManager::GetInstance()->Draw();
    //アイテムを手前に描画する
    itemManager_->DrawGetItem();
}

void SampleScene::DrawSprite() {




    Sprite::PreDraw();
    
    itemManager_->DrawUI();

    uIManager_->DrawPauseScreen();
    if (amenStage_) amenStage_->DrawUI();


    
    memoManager_->DrawUI();
    uIManager_->DrawCurPos();
    player_->DrawRaySprite();

    uIManager_->DrawEffect();

    sceneChange_->Draw();


}