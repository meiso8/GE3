#include "Enemy.h"
#include"Model.h"
#include"ModelManager.h"
#include"Camera.h"
#include"Input.h"

#include"JsonFile.h"
#include"CollisionConfig.h"
#include"SphericalCoordinate.h"
#include"Collision.h"
#include"Easing.h"
#include"DebugUI.h"
#include"TransformAni/TransformAni.h"
#include"TimeManager.h"
#include"Math/Matrix/MakeMatrix.h"

namespace {
    constexpr float kScale_ = 5.0f;
    constexpr float kMoveSpeed_ = 2.0f;

    constexpr float kApperTime_ = 7.0f;
    constexpr float kApperEndTime_ = kApperTime_ + 2.0f;

    constexpr float kAlphaWalkTime_ = 10.0f;
    constexpr float kAlphaWalkEndTime_ = kAlphaWalkTime_ + 1.0f;

    constexpr float kShotTime_ = 10.0f;

    constexpr float kBeamTime_ = 3.0f;
    constexpr float kBeamEndTime_ = 10.0f;
    constexpr float kColliderRad_ = 0.5f;
    constexpr float kWalkFootstepInterval_ = 0.4f;
    float soundTimer_ = 0.0f;
    bool isFootPreCollided_ = false;
}

Enemy::Enemy()
{
    UpdateActions_ = {
         {PHASE::APPEAR, std::bind(&Enemy::Appear, this)},
         {PHASE::ROUND, std::bind(&Enemy::Round, this)},
         {PHASE::FIREBALL, std::bind(&Enemy::Fireball, this)},
         {PHASE::ALPHA_WALK,std::bind(&Enemy::AlphaWalk,this)},
         {PHASE::BEAM,std::bind(&Enemy::Beam,this)},
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
    SetWorldMatrix(bodyPos_.GetWorldTransform());
    SetCenter({ 0.0f,0.0f, 0.0f });

    SetCollisionAttribute(kCollisionEnemy);
    // 敵は「プレイヤー」と「プレイヤーの弾」と衝突したい
    SetCollisionMask(kCollisionPlayer | kCollisionPlayerBulletCold | kCollisionPlayerBulletHot);

    colliders_["EnemyFoot_L"].collider_ = std::make_unique<Collider>();
    colliders_["EnemyFoot_R"].collider_ = std::make_unique<Collider>();
    colliders_["EnemyHead"].collider_ = std::make_unique<Collider>();

    eyeMats_["eye_L"] = MakeIdentity4x4();
    eyeMats_["eye_R"] = MakeIdentity4x4();

    for (auto& [name, group] : colliders_) {
        group.matrix_ = MakeIdentity4x4();
        group.collider_->SetRadius(kColliderRad_);

        group.collider_->SetCollisionAttribute(kCollisionEnemy);
        // 足とfloorを判定する

        if (name == "EnemyHead") {
            //目だったら
            group.collider_->SetCollisionMask(kCollisionPlayerBulletCold | kCollisionPlayerBulletHot);
            /*     group.collider_->SetCenter({ 0.0f,0.0f,0.125f });*/
        } else {
            group.collider_->SetCollisionMask(kCollisionFloor);
            group.collider_->SetCenter({ 0.0f,0.125f,0.0f });
        }

        group.collider_->SetWorldMatrix(group.matrix_);
    }
}

void Enemy::Init()
{
    Json file = JsonFile::GetJsonFiles("Boss");
    actionTime_ = file["First"]["ActionTimer"];
    characterState_.hps.maxHp = file["First"]["HP"];
    characterState_.hps.hp = characterState_.hps.maxHp;
    characterState_.isDead = false;
    characterState_.isHit = false;

    velocity_ = { 10.0f,10.0f,10.0f };
    startPos_ = { 0.0f };
    startScale_ = { 0.0f,0.0f,0.0f };
    isAppear_ = false;
    isShotStart_ = false;
    isFootPreCollided_ = false;

    phase_ = APPEAR;

    bodyPos_.Initialize();
    bodyPos_.SetScale({ 0.0f,0.0f,0.0f });

    bodyPos_.SetAnimation("Nod");

    phaseTimer_ = 0.0f;
    poyoAnimTimer_ = 0.0f;

}

void Enemy::Draw(Camera& camera, const Object3d::LightMode& lightMode)
{
    if (!isAppear_) { return; }

    bodyPos_.SetLightMode(lightMode);
    bodyPos_.Draw(camera);
    //ColliderDraw(camera);

    for (auto& [name, group] : colliders_) {
        group.collider_->ColliderDraw(camera);
    }
}

void Enemy::Update()
{



#ifdef USE_IMGUI  

    DebugUI::CheckObject3d(bodyPos_, "Enemy");


    ImGui::Begin("Enemy");

    DebugUI::CheckCaracterState(characterState_, "Enemy");
    ImGui::Checkbox("isApper", &isAppear_);

    for (auto& [name, collider] : colliders_) {

        auto& collided = collider.collider_->GetCollisionInfo().collided;

        if (ImGui::TreeNode(name.c_str())) {
            ImGui::Checkbox("isCollided", &collided);
            ImGui::TreePop();
        }

    }

    const char* phase[] = { "APPEAR", "ROUND", "FIREBALL","ALPHA_WALK","BEAM","EXIT" };
    int currentPhase = phase_;


    if (ImGui::Combo("CurrentPhase", &currentPhase, phase, IM_ARRAYSIZE(phase))) {
        //APPEAR,
        //ROUND,
        //FIREBALL,
        //ALPHA_WALK,
        //BEAM,
        //EXIT,

        SetPhase(static_cast<PHASE>(currentPhase));

    };


    ImGui::End();

#endif // USE_IMGUI  

    if (!isAppear_) { return; }

    // とりあえずフェーズが最大になったら処理を終える  
    if (phase_ >= MAX_PHASE || phase_ < 0) {
        return;
    }

    UpdateTimer();

    // 呼び出す  
    UpdateActions_[phase_]();
    HitUpdate();
    bodyPos_.UpdateAniTimer();
    bodyPos_.Update();



    ColliderUpdate();

    //マトリックスの更新
    colliders_["EnemyFoot_L"].matrix_ = bodyPos_.GetWorldJointMatrix("foot_L");
    colliders_["EnemyFoot_R"].matrix_ = bodyPos_.GetWorldJointMatrix("foot_R");
    //頭の位置を仮入れ
    colliders_["EnemyHead"].matrix_ = bodyPos_.GetWorldJointMatrix("head");

    for (auto& [name, collider] : colliders_) {
        /*   collider.collider_->SetWorldMatrix(collider.matrix_);*/
        collider.collider_->ColliderUpdate();
    }


    for (auto& [name, mat] : eyeMats_) {
        mat = bodyPos_.GetWorldJointMatrix(name);
    }

}

void Enemy::SoundFootStep(const SoundFactory::TAG tag)
{

    soundTimer_ -= TimeManager::DeltaTime();


    bool isCollided = false;

    for (auto& [name, collider] : colliders_) {

        //名前が目じゃないとき
        if (name == "EnemyHead") { continue; }

        if (collider.collider_->GetCollisionInfo().collided) {
            isCollided = true;
            break;

        }
    }

    if (isCollided && !isFootPreCollided_) {

        if (soundTimer_ <= 0.0f) {

            //距離によって変化させる
            float distance = Distance(*target_, bodyPos_.GetTranslate());
            distance /= 100.0f;
            distance = std::clamp(distance, 0.0f, 1.0f);
            Sound::PlaySE(tag, distance);
            soundTimer_ = kWalkFootstepInterval_;
        }
    }

    isFootPreCollided_ = isCollided;
}

void Enemy::OnCollision(Collider* collider)
{

    if (!isAppear_) { return; }

    //デバック用
    OnCollisionCollider();

    if (collider->GetCollisionAttribute() == kCollisionPlayerBulletCold || collider->GetCollisionAttribute() == kCollisionPlayerBulletHot) {
        //プレイヤーの弾の温度によって後でカエル
        if (!characterState_.isHit) {
            characterState_.isHit = true;

            Sound::PlaySE(SoundFactory::CRACKER);

            if (characterState_.hps.hp > 0) {
                characterState_.hps.hp--;
            }

            poyoAnimTimer_ = 0.0f;
        }

        if (collider->GetCollisionAttribute() == kCollisionPlayer) {

            Sound::PlaySE(SoundFactory::VOICE_Asobimasyo, 1.0f);

        }

    }
}
Vector3 Enemy::GetToTarget()
{
    if (target_) {
        return ToTargetVector(*target_, bodyPos_.GetWorldTransform().GetWorldPosition());
    }

    return { 0.0f };
}


void Enemy::SetPhase(const PHASE phase)
{
    phaseTimer_ = 0.0f;
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
        auto& transform = bodyPos_.GetTransform();
        startRotateY_ = transform.rotate.y;
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
    float time = phaseTimer_ / kApperTime_;
    time = std::clamp(time, 0.0f, 1.0f);
    Look();
    auto& transform = bodyPos_.GetTransform();
    transform.scale = Easing::EaseInBounce(startScale_, { kScale_,kScale_,kScale_ }, time);

    if (phaseTimer_ >= kApperEndTime_) {
        SetPhase(ROUND);
        transform.scale = { kScale_,kScale_,kScale_ };
    }

}

void Enemy::Round()
{
    bodyPos_.SetColor({ 1.0f,1.0f,1.0f,Easing::EaseInBounce(0.0f,1.0f,fmod(phaseTimer_,1.0f)) });
    auto& transform = bodyPos_.GetTransform();
   transform.translate = Lerp(transform.translate, { 0.0f,0.0f,0.0f }, 0.5f);
    Look();
    if (phaseTimer_ >= actionTime_) {
        SetPhase(FIREBALL);
    }
}

void Enemy::Fireball()
{
    float theta = phaseTimer_ * std::numbers::phi_v<float>*2.0f;
    bodyPos_.SetColor({ 1.0f,1.0f,1.0f,sinf(phaseTimer_) * 0.5f + 1.0f });

    if (phaseTimer_ <= 1.0f) {
        RotateY(phaseTimer_);
    } else {

        if (!isShotStart_) {
            isShotStart_ = true;
        }

        Look();
    }

    if (phaseTimer_ >= kShotTime_) {
        //透明移動をする
        SetPhase(ALPHA_WALK);
    }

    if (bodyPos_.GetCurrentAnimation() == "Jump" || bodyPos_.GetCurrentAnimation() == "Step") {
        //足音の更新処理をここで呼び出す
        SoundFootStep(SoundFactory::MEDJED_JUMP);
    }


}


void Enemy::Exit()
{

}

void Enemy::AlphaWalk()
{
    //常にプレイヤーを向く
    Look();

    if (phaseTimer_ < kAlphaWalkTime_) {

        if (phaseTimer_ <= 1.0f) {
            //スーッとαが下がるよ
            bodyPos_.SetColor({ 1.0f,1.0f,1.0f,Easing::EaseInOutBack(1.0f,0.0f,phaseTimer_) });
        }

    } else if (phaseTimer_ < kAlphaWalkEndTime_) {

        float time = kAlphaWalkTime_ - phaseTimer_ / (kAlphaWalkEndTime_ - kAlphaWalkTime_);
        time = std::clamp(time, 0.0f, 1.0f);
        bodyPos_.SetColor({ 1.0f,1.0f,1.0f,Easing::EaseInOutBack(0.0f,1.0f,time) });

    } else {
        bodyPos_.SetColor({ 1.0f,1.0f,1.0f,1.0f });

        SetPhase(BEAM);
    }

    if (target_) {


        velocity_ = GetToTarget();
        const float deltaTime = TimeManager::DeltaTime();
        velocity_ *= deltaTime * kMoveSpeed_;

        //Y軸方向には移動しない
        auto& transform =bodyPos_.GetTransform();
        transform.translate.x += velocity_.x;
        transform.translate.z += velocity_.z;

        //足音の更新処理をここで呼び出す
        SoundFootStep(SoundFactory::MEDJED_FOOT_STEP_SMALL);

    }

}

void Enemy::Beam()
{

    //ずっとコチラを覗く
    Look();

    if (phaseTimer_ < kBeamTime_) {

        if (PoyoPoyoUpdateAndGetEnd(1.0f)) {

            //ぽよぽよ終わったら
            bodyPos_.SetAnimation("Swing");
            if (!isShotStart_) {
                //打つ
                isShotStart_ = true;
            }
        } else {
            //ぽよぽよしている時はコレ
            bodyPos_.SetAnimation("Idle");
        }

    } else if (phaseTimer_ < kBeamEndTime_) {
        //うなずく
        //ショットを中止する
        isShotStart_ = false;
        bodyPos_.SetAnimation("Nod");
    } else {
        SetPhase(ROUND);
    }

}

void Enemy::UpdateTimer()
{
    phaseTimer_ += TimeManager::DeltaTime();
}

void Enemy::Look()
{
    TransformAni::LookTarget(bodyPos_.GetWorldTransform(), *target_);
}

bool Enemy::PoyoPoyoUpdateAndGetEnd(const float& endTimer)
{
    poyoAnimTimer_ += TimeManager::DeltaTime();
    poyoAnimTimer_ = std::clamp(poyoAnimTimer_, 0.0f, endTimer);
    TransformAni::PoyoPoyo(bodyPos_.GetWorldTransform(), poyoAnimTimer_, kScale_);

    return (poyoAnimTimer_ == endTimer);

}

void Enemy::HitUpdate()
{
    //毎フレーム呼ばれる

    if (characterState_.isHit) {

        //ヒットしたらぽよぽよ終わったらヒットフラグを下げる
        if (PoyoPoyoUpdateAndGetEnd()) {
            characterState_.isHit = false;

            if (characterState_.hps.hp <= 0.0f) {
                characterState_.isDead = true;
            }
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
    auto& transform = bodyPos_.GetTransform();
   transform.scale = Lerp(Vector3{ transform.scale }, { kScale_,kScale_,kScale_ }, 0.5f);
}

void Enemy::RotateY(const float& timer)
{
    auto& transform = bodyPos_.GetTransform();
    transform.rotate.y = Easing::EaseInBack(startRotateY_, endRotateY_, timer);
}
