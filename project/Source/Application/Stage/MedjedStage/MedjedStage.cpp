#include "MedjedStage.h"
#include"SoundManager/SoundManager.h"
#include"Sound.h"
#include "ParticleEmitter.h"
#include"../StageManager.h"
#include"DebugUI.h"

const bool MedjedStage::IsClear()
{
    //メジェド様が死亡
    return  medjedManager_->GetIsEnemyDead();
}

MedjedStage::MedjedStage()
{
    backGround_ = std::make_unique<BackGround>();
    medjedManager_ = std::make_unique<MedjedManager>();
    rhythmBullet_ = std::make_unique<RhythmBullet>();
    rhythmBullet_->SetEnemy(medjedManager_->GetEnemy());
    //パーティクルの作成
    CreateParticle();
    //HPゲージの追加
    uiManager_->CreateHpGage(*GetEnemy()->GetHpsPtr(), *player_->GetHpsPtr());
}

void MedjedStage::Initialize()
{
    //ライトの初期化？
    lightingManager_->Initialize();
    //パーティクルの初期化
    particleEmitters_[0]->GetEmitter().transform.Parent(GetMedjed()->GetWorldTransform());
    //パーティクルのリセット
    ParticleManager::ResetAll();

    //メモの生成
    memoManager_->GenerateMemos({ TextureFactory::BOOK3 });
    SoundManager::InitMedjedScene();
    backGround_->Initialize();
    backGround_->Update();
    medjedManager_->Initialize();
    rhythmBullet_->Initialize();
    medjedManager_->SetTarget(player_->GetBodyPos());
    medjedManager_->SetRaySprite(player_->raySprite_.get());
    medjedManager_->Update();
    player_->Init();
    player_->SetBodyPos({ 0.0f, 0.0f, 0.0f });
    player_->Update();

    itemManager_->GenerateItems({ "SunRod" });
    Sound::PlaySE(SoundFactory::VOICE_Asobimasyo,0.5f);

}

void MedjedStage::Update()
{

    medjedManager_->Update();
    backGround_->Update();

    if (FindMedjed()) {
        
        backGround_->UpdateApperMedjed();

        if (medjedManager_->GetIsApperMedjed()) {
            //メジェド出現！
            SoundManager::ApperMedjedUpdate();
            rhythmBullet_->Update();
        }


        //ライト管理
        lightingManager_->DirectionalLightUpdate();

        if (GetEnemyApper()) {
            particleEmitters_[0]->GetEmitter().transform.Parent(GetEnemy()->GetWorldTransform());
        }

        for (int i = 0; i < particleEmitters_.size(); ++i) {
            particleEmitters_[i]->UpdateTimer();
            particleEmitters_[i]->UpdateEmitter();
        }

        uiManager_->UpdateGage();

    } else {
        SoundManager::NotFindMedjedUpdate();
    }

    if (IsClear()) {
        //クリアしたらシーンを切り替える
        TransitionScene();
    }

#ifdef USE_IMGUI

    DebugUI::CheckParticle(*particleEmitters_[0], "Emitter0");

#endif // !USE_IMGUI

}

void MedjedStage::Draw(Camera& camera)
{
    backGround_->Draw(camera);

    if (FindMedjed()) {
        backGround_->DrawField(camera);
        rhythmBullet_->Draw(camera);
    }

    medjedManager_->Draw(camera);

}

void MedjedStage::DrawSprite()
{
    if (FindMedjed()) {
        //HPゲー描画
        uiManager_->DrawHPGage();
    }
}

void MedjedStage::CheckCollision(CollisionManager& collisionManager)
{
    //メジェドたちがヒットしているかどうか
    medjedManager_->RayCastHit(*player_->raySprite_);
    //弾がヒットしているかどうか
    rhythmBullet_->GetShotBulletManager()->RayCastHit(*player_->raySprite_);


    //メジェド探したかどうか
    if (FindMedjed()) {

        collisionManager.AddCollider(medjedManager_->GetMedjed());

        if (medjedManager_->GetIsApperMedjed()) {
            collisionManager.AddCollider(medjedManager_->GetEnemy());

            //巨大メジェド出現し、弾を打ってくる
            for (auto& bullet : rhythmBullet_->GetBulletManager()->GetBullets()) {
                if (bullet->isActive_) {
                    collisionManager.AddCollider(bullet.get());
                }
            }

        }

    } else {
        for (auto& locker : medjedManager_->GetAllMedjeds()) {
            collisionManager.AddCollider(locker.get());
        }

    }

    // 壁との当たり判定
    for (auto& [type, object] : backGround_->GetBuilding()->GetFieldPoses()) {
        collisionManager.AddCollider(object.get());
    }
}

void MedjedStage::CreateParticle()
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

void MedjedStage::TransitionScene()
{
    //メジェドを倒したらシーン切り替え
    StageManager::GetInstance()->TransitionScene("Result");
}
