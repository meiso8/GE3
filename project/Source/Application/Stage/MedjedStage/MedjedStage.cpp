#include "MedjedStage.h"
#include"SoundManager/SoundManager.h"
#include"Sound.h"
#include "ParticleEmitter.h"
#include"../StageManager/StageManager.h"
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

    auto* enemy = medjedManager_->GetEnemy();

    rhythmBullet_ = std::make_unique<RhythmBullet>(enemy, player_, player_->GerRaySprite());


    medjedManager_->SetRaySprite(player_->GerRaySprite());

    //パーティクルの作成
    CreateParticle();
    //HPゲージの追加
    uiManager_->CreateHpGage(*GetEnemy()->GetHpsPtr(), *player_->GetHpsPtr());
}

void MedjedStage::Initialize()
{
  
    StageTransitionInitialize();
    if (isInitialize_) {
        //いまコンストラクタでやればいいのではとなっている
        return;
    }

    isInitialize_ = true;

}

void MedjedStage::StageTransitionInitialize()
{

    Sound::PlaySE(SoundFactory::VOICE_Asobimasyo, 0.5f);

    //ステージのロード
    LoadAndCreateObject("MedjedStage_objectEditor");
    
    backGround_->Initialize();
    backGround_->Update();

    medjedManager_->Initialize();
    rhythmBullet_->Initialize();

    //パーティクルの初期化
    particleEmitters_[kMedjed_Particle]->GetEmitter().transform.Parent(GetMedjed()->GetWorldTransform());

    //メモの生成
    memoManager_->GenerateMemos({ TextureFactory::BOOK3 });
    //アイテムの生成
    itemManager_->GenerateItems({ "SunRod" });

    player_->Init({ 0.0f, 0.0f, 0.0f });
}

void MedjedStage::Update()
{

    medjedManager_->Update();
    backGround_->Update();

    //メジェド様が出現したかどうかで音を区別する
    SoundManager::SetMedjedApper(FindMedjed()&& medjedManager_->GetIsApperMedjed());

    if (FindMedjed()) {

        backGround_->UpdateApperMedjed();

        if (medjedManager_->GetIsApperMedjed()) {
            //メジェド出現！
            rhythmBullet_->SetSound(SoundFactory::BGM_ArabRuins);
            rhythmBullet_->Update();
   
        }


        //ライト管理
        lightingManager_->DirectionalLightUpdate();

        if (GetEnemyApper()) {
            particleEmitters_[kSky_Particle]->GetEmitter().transform.Parent(GetEnemy()->GetWorldTransform());
            UpdateEmitter(kSky_Particle);
        }


        auto enemyPhase = GetEnemy()->GetPhase();

        //透明移動じゃないときEmittする
        if (enemyPhase != Enemy::PHASE::ALPHA_WALK && enemyPhase != Enemy::PHASE::BEAM) {
            UpdateEmitter(kMedjed_Particle);
        }


        uiManager_->UpdateGage();

    }

    if (IsClear()) {
        //クリアしたらシーンを切り替える
        TransitionScene();
    }

#ifdef USE_IMGUI

    DebugUI::CheckEmitter(particleEmitters_[kSky_Particle]->GetEmitter());
    DebugUI::CheckEmitter(particleEmitters_[kMedjed_Particle]->GetEmitter());

#endif // !USE_IMGUI

}

void MedjedStage::Draw()
{
    backGround_->Draw();

    if (FindMedjed()) {
        backGround_->DrawField();
    }

    medjedManager_->Draw();

    if (FindMedjed()) {
        rhythmBullet_->Draw();
    }

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
    medjedManager_->RayCastHit();
    //弾がヒットしているかどうか
    rhythmBullet_->GetShotBulletManager()->RayCastHit();
    rhythmBullet_->GetShotBeamManager()->RayCastHit();

    //メジェド探したかどうか
    if (FindMedjed()) {

        collisionManager.AddCollider(medjedManager_->GetMedjed());

        if (medjedManager_->GetIsApperMedjed()) {

            auto* enemy = medjedManager_->GetEnemy();

            collisionManager.AddCollider(enemy);

            for (auto& [name, group] : enemy->GetColliderGroup()) {
                //各コライダーの登録
                collisionManager.AddCollider(group.collider_.get());
            }

            //巨大メジェド出現し、弾を打ってくる
            for (auto& bullet : rhythmBullet_->GetBulletManager()->GetBullets()) {
                if (bullet->GetIsActive()) {
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

void MedjedStage::UpdateEmitter(const Particels& particles)
{
    particleEmitters_[particles]->Update();
}

void MedjedStage::CreateParticle()
{
    for (int i = 0; i < particleEmitters_.size(); ++i) {
        particleEmitters_[i] = std::make_unique<ParticleEmitter>();
        particleEmitters_[i]->Initialize();
    }

    particleEmitters_[kMedjed_Particle]->SetName("medjedParticle");
    particleEmitters_[kSky_Particle]->SetName("people");

    auto& emitter0 = particleEmitters_[kMedjed_Particle]->GetEmitter();
    emitter0.count = 8;
    emitter0.startColor = { 1.0f,0.75f,0.75f,1.0f };
    emitter0.endColor = { 1.0f,0.75f,0.75f,0.0f };
    emitter0.frequencyTime = 0.25f;
    emitter0.lifeTime = 6.0f;
    emitter0.blendMode = kBlendModeMultiply;
    emitter0.movement = ParticleMovements::kParticleSphere;
    emitter0.radius = 3.0f;
    float pi = std::numbers::pi_v<float>;
    emitter0.rotateAABB_ = { .min = {-pi ,-pi ,-pi } ,.max = { pi, pi, pi} };
    emitter0.isLoop_ = true;
    emitter0.accelerationField_.acceleration.y = 5.0f;
    emitter0.accelerationField_.area = { .min = {-25.0f,0.0f,-25.0f},.max = {25.0f,40.0f,25.0f} };

    auto& emitter1 = particleEmitters_[kSky_Particle]->GetEmitter();
    emitter1.transform.eTransform_.translate.y = 30.0f;
    emitter1.transform.eTransform_.scale = { 10.0f,10.0f,10.0f };
    emitter1.count = 4;
    emitter1.startColor = { 1.0f,0.75f,0.75f,1.0f };
    emitter1.endColor = { 1.0f,0.75f,0.75f,0.0f };
    emitter1.frequencyTime = 0.1f;
    emitter1.lifeTime = 10.0f;
    emitter1.blendMode = kBlendModeScreen;
    emitter1.velocityAABB = { { -10.0f,-10.0f,-10.0f }, { 10.0f,0.0f,10.0f } };
    emitter1.rotateAABB_ = { .min = {-pi ,-pi ,-pi } ,.max = { pi, pi, pi} };
    emitter1.isLoop_ = true;
    emitter1.accelerationField_.acceleration.y = 10.0f;
    emitter1.accelerationField_.area = { .min = {-25.0f,0.0f,-25.0f},.max = {25.0f,15.0f,25.0f} };
}

void MedjedStage::TransitionScene()
{
    //メジェドを倒したらシーン切り替え
    StageManager::GetInstance()->TransitionScene("Result");
}
