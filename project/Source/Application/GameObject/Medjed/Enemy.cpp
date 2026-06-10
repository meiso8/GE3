#include "Enemy.h"
#include"Model.h"
#include"ModelManager.h"
#include"Camera.h"
#include"Input.h"
#include"Sound.h"
#include"JsonFile.h"
#include"CollisionConfig.h"
#include"SphericalCoordinate.h"
#include"Collision.h"
#include"Easing.h"
#include"DebugUI.h"
#include"TransformAni/TransformAni.h"
#include"TimeManager.h"

namespace {
    const float kMoveSpeed_ = 2.0f;
}

Enemy::Enemy()
{
    UpdateActions_ = {
         {PHASE::APPEAR, std::bind(&Enemy::Appear, this)},
         {PHASE::ROUND, std::bind(&Enemy::Round, this)},
         {PHASE::FIREBALL, std::bind(&Enemy::Fireball, this)},
         {PHASE::ALPHA_WALK,std::bind(&Enemy::AlphaWalk,this)},
         {PHASE::EXIT, std::bind(&Enemy::Exit, this)},

    };

    model_ = ModelManager::GetModel("medjed");

    bodyPos_.SetModelAndLoadAnimation(model_);
    bodyPos_.Create();
    bodyPos_.SetMeshAndMaterial(model_);

    bodyPos_.SetTemperature(1.0f);
    float halfScale = kScale_ * 0.25f;
    Init();
    SetAABB({ { -halfScale - halfScale * 1.5f ,-halfScale }, { halfScale ,halfScale * 1.5f ,halfScale } });
    SetWorldMatrix(bodyPos_.worldTransform_.matWorld_);
    SetCenter({ 0.0f,0.0f, 0.0f });

    SetCollisionAttribute(kCollisionEnemy);
    // 敵は「プレイヤー」と「プレイヤーの弾」と衝突したい
    SetCollisionMask(kCollisionPlayer | kCollisionPlayerBullet);


}

void Enemy::Init()
{
    Json file = JsonFile::GetJsonFiles("Boss");
    actionTime_ = file["First"]["ActionTimer"];
    characterState_.hps.maxHp = file["First"]["HP"];
    characterState_.hps.hp = characterState_.hps.maxHp;
    characterState_.isDead = false;
    characterState_.isHit = false;

    attackTime_ = 10.0f;

    velocity_ = { 10.0f,10.0f,10.0f };
    startPos_ = { 0.0f };
    startScale_ = { 0.0f,0.0f,0.0f };
    isAppear_ = false;
    isShotStart_ = false;

    phase_ = APPEAR;

    bodyPos_.Initialize();
    bodyPos_.worldTransform_.scale_ = { 0.0f };
    bodyPos_.SetAnimation("Nod");

    timer_ = 0.0f;
    poyoAnimTimer_ = 0.0f;

}

void Enemy::Draw(Camera& camera, const LightMode& lightMode)
{
    if (!isAppear_) { return; }

    bodyPos_.SetLightMode(lightMode);
    bodyPos_.Draw(camera, kBlendModeNormal);
    ColliderDraw(camera);
}

void Enemy::Update()
{
    if (!isAppear_) { return; }

    // とりあえずフェーズが最大になったら処理を終える  
    if (phase_ >= MAX_PHASE || phase_ < 0) {
        return;
    }


#ifdef USE_IMGUI  
    DebugUI::CheckCaracterState(characterState_, "enemy");
    DebugUI::CheckAnimation(bodyPos_, "EnemyAnimation");



#endif // USE_IMGUI  

    UpdateTimer();

    // 呼び出す  
    UpdateActions_[phase_]();
    HitUpdate();
    bodyPos_.UpdateAniTimer();
    bodyPos_.Update();
    ColliderUpdate();
}

void Enemy::OnCollision(Collider* collider)
{

    if (!isAppear_) { return; }

    //デバック用
    OnCollisionCollider();

    if (collider->GetCollisionAttribute() == kCollisionPlayerBullet) {
        if (!characterState_.isHit) {
            characterState_.isHit = true;

            Sound::PlaySE(SoundFactory::CRACKER);

            if (characterState_.hps.hp > 0) {
                characterState_.hps.hp--;
            } else {
                characterState_.isDead = true;
            }

            poyoAnimTimer_ = 0.0f;
        }

        if (collider->GetCollisionAttribute() == kCollisionPlayer) {

            Sound::PlaySE(SoundFactory::VOICE_Asobimasyo, 1.0f);

        }

    }
};


void Enemy::SetPhase(const PHASE phase)
{
    timer_ = 0.0f;
    phase_ = phase;
    poyoAnimTimer_ = 0.0f;
    isShotStart_ = false;
    bodyPos_.InitTime();

    if (phase_ == ROUND || phase_ == APPEAR) {
        //ここを変更する
        bodyPos_.SetAnimation("Round");
    }

    if (phase_ == ALPHA_WALK) {
        //透明移動する
        bodyPos_.SetAnimation("Walk");
    }

    if (phase_ == FIREBALL) {

        startRotateY_ = bodyPos_.worldTransform_.rotate_.y;
        endRotateY_ = startRotateY_ + std::numbers::pi_v<float>*2.0f;

        //ここを変更する
        int randNum = rand() % 4;
        if (randNum == 0) {
            bodyPos_.SetAnimation("Swing");
        } else if (randNum == 1) {
            bodyPos_.SetAnimation("Nod");
        } else if (randNum == 2) {
            bodyPos_.SetAnimation("Step");
        } else if (randNum == 3) {
            bodyPos_.SetAnimation("Jump");
        }

    }
}

void Enemy::Appear()
{
    float time = timer_ / kApperTime_;
    time = std::clamp(time, 0.0f, 1.0f);
    Look();

    bodyPos_.worldTransform_.scale_ = Easing::EaseInBounce(startScale_, { kScale_,kScale_,kScale_ }, time);

    if (timer_ >= kApperEndTime_) {
        SetPhase(FIREBALL);
        bodyPos_.worldTransform_.scale_ = { kScale_,kScale_,kScale_ };
    }

}

void Enemy::Round()
{
    bodyPos_.SetColor({ 1.0f,1.0f,1.0f,Easing::EaseInBounce(0.0f,1.0f,fmod(timer_,1.0f)) });

    bodyPos_.worldTransform_.translate_ = Lerp(bodyPos_.worldTransform_.translate_, { 0.0f,0.0f,0.0f }, 0.5f);
    Look();
    if (timer_ >= actionTime_) {
        SetPhase(FIREBALL);
    }
}

void Enemy::Fireball()
{
    float theta = timer_ * std::numbers::phi_v<float>*2.0f;
    bodyPos_.SetColor({ 1.0f,1.0f,1.0f,sinf(timer_) * 0.5f + 1.0f });

    if (timer_ <= 1.0f) {
        RotateY(timer_);
    } else {

        if (!isShotStart_) {
            isShotStart_ = true;
        }

        Look();
    }

    if (timer_ >= attackTime_) {
        /*     isShotStart_ = false;*/
        SetPhase(ALPHA_WALK);
    }

}


void Enemy::Exit()
{

}

void Enemy::AlphaWalk()
{

    Look();

    if (timer_ < kAlphaWalkTime_) {

        if (timer_ <= 1.0f) {
            //スーッとαが下がるよ
            bodyPos_.SetColor({ 1.0f,1.0f,1.0f,Easing::EaseInOutBack(1.0f,0.0f,timer_) });
        }

    } else if (timer_ < kAlphaWalkEndTime_) {

        float time = kAlphaWalkTime_ - timer_ / (kAlphaWalkEndTime_ - kAlphaWalkTime_);
        time = std::clamp(time, 0.0f, 1.0f);
        bodyPos_.SetColor({ 1.0f,1.0f,1.0f,Easing::EaseInOutBack(0.0f,1.0f,time) });

    } else {

        bodyPos_.SetColor({ 1.0f,1.0f,1.0f,1.0f });
        isShotStart_ = false;
        SetPhase(ROUND);
    }

    if (target_) {
        Vector3 distance = *target_ - bodyPos_.worldTransform_.translate_;
        float length = Distance(*target_, bodyPos_.worldTransform_.translate_);

        velocity_ = ToTargetVector(*target_, bodyPos_.worldTransform_.translate_);
        const float deltaTime = Time::DeltaTime();
        velocity_ *= deltaTime * kMoveSpeed_;

        bodyPos_.worldTransform_.translate_.x += velocity_.x;
        bodyPos_.worldTransform_.translate_.z += velocity_.z;
    }

}

void Enemy::UpdateTimer()
{
    timer_ += Time::DeltaTime();
}

void Enemy::Look()
{
    TransformAni::LookTarget(bodyPos_.worldTransform_, *target_);
}

void Enemy::PoyoPoyo(const float& endTimer)
{
    poyoAnimTimer_ += Time::DeltaTime();
    poyoAnimTimer_ = std::clamp(poyoAnimTimer_, 0.0f, endTimer);
    TransformAni::PoyoPoyo(bodyPos_.worldTransform_, poyoAnimTimer_, kScale_);
}

void Enemy::HitUpdate()
{
    if (characterState_.isHit) {
        PoyoPoyo();

        if (poyoAnimTimer_ == 0.25f) {
            characterState_.isHit = false;
        }
    } else {
        if (phase_ != APPEAR) {
            //出現時じゃないとき線形補間する
            LerpScale();
        }

    }
}

void Enemy::LerpScale()
{
    bodyPos_.worldTransform_.scale_ = Lerp(Vector3{ bodyPos_.worldTransform_.scale_ }, { kScale_,kScale_,kScale_ }, 0.5f);
}

void Enemy::RotateY(const float& timer)
{
    bodyPos_.worldTransform_.rotate_.y = Easing::EaseInBack(startRotateY_, endRotateY_, timer);
}
