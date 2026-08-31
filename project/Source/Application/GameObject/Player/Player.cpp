#define NOMINMAX
#include<numbers>
#include "Player.h"
#include"ModelManager.h"
#include"Model.h"

#include"Easing.h"
#include<algorithm>
#include"Collision.h"
#include"JsonFile.h"
#include"LightingManager.h"
#include"MakeMatrix.h"
#include"CoordinateTransform.h"

#include"CollisionConfig.h"
#include"InputBind.h"
#include"TimeManager.h"
#include"PostProcessManager/PostProcessManager.h"
#include"Sound.h"
#include"DebugUI.h"
#include"ItemManager/ItemManager.h"


void Player::OnCollision(Collider* collider)
{

    if (collider->GetCollisionAttribute() == CollisionTag::GetTag("Enemy")
        || collider->GetCollisionAttribute() == CollisionTag::GetTag("EnemyBulletCold")
        || collider->GetCollisionAttribute() == CollisionTag::GetTag("EnemyBulletHot")
        || collider->GetCollisionAttribute() == CollisionTag::GetTag("Medjed")
        || collider->GetCollisionAttribute() == CollisionTag::GetTag("Mummy")

        ) {
        OnCollisionEnemy();
    }

    if (collider->GetCollisionAttribute() == CollisionTag::GetTag("Floor")|| collider->GetCollisionAttribute() == CollisionTag::GetTag("Block")) {
        isFloorHit_ = true;
        isJump_ = false;
    }

    if (
        collider->GetCollisionAttribute() == CollisionTag::GetTag("DummyMedjed")
        || collider->GetCollisionAttribute() == CollisionTag::GetTag("Wall")
        || collider->GetCollisionAttribute() == CollisionTag::GetTag("Medjed")
        || collider->GetCollisionAttribute() == CollisionTag::GetTag("Enemy")
        || collider->GetCollisionAttribute() == CollisionTag::GetTag("Mummy")
        || collider->GetCollisionAttribute() == CollisionTag::GetTag("Floor")
        || collider->GetCollisionAttribute() == CollisionTag::GetTag("Block")

        ) {

        ResolveCollision(aniObject_->GetTransform().translate, velocity_, GetCollisionInfo());

    }

    if (collider->GetCollisionAttribute() == CollisionTag::GetTag("CameraUp")) {
        //カメラ上昇するコライダーに当たった時を記入していく
        eyePosition_->SiftToUp();
    } else {
        eyePosition_->ShftToTDown();
    }


    OnCollisionCollider();

}

Player::Player() {

    //モデルを取得する
    model_ = ModelManager::GetModel("player.gltf");

    float radius = 0.25f;

    SetAABB(AABB{ .min = {-radius , 0.0f ,-radius},.max = { radius , 1.5f ,radius} });
    SetCollisionAttribute(CollisionTag::GetTag("Player"));
    SetCollisionMask(
        CollisionTag::GetTag("Enemy")
        | CollisionTag::GetTag("EnemyBulletCold")
        | CollisionTag::GetTag("EnemyBulletHot")
        | CollisionTag::GetTag("Medjed")
        | CollisionTag::GetTag("DummyMedjed")
        | CollisionTag::GetTag("Wall")
        | CollisionTag::GetTag("Mummy")
        | CollisionTag::GetTag("Water")
        | CollisionTag::GetTag("Floor")
        | CollisionTag::GetTag("StageTrigger")
        | CollisionTag::GetTag("Block")
        | CollisionTag::GetTag("CameraUp")
        | CollisionTag::GetTag("Sensor")
    );

    //それぞれのObject3dを作る
    aniObject_ = std::make_unique<AnimationObject3d>();
    aniObject_->Create();
    aniObject_->SetMeshAndMaterial(model_);
    aniObject_->SetModelAndLoadAnimation(model_);
    aniObject_->SetAnimation("Idle");
    SetWorldMatrix(aniObject_->GetWorldTransform());

    raySprite_ = std::make_unique<RaySprite>();
    eyePosition_ = std::make_unique<EyePosition>();
    //体の位置を親に設定
    eyePosition_->SetParentMatrix(&headMatrix_);

    isInvincible_ = false;
#ifdef _DEBUG
    isInvincible_ = true;
#endif

    velocity_ = { 0.0f,0.0f,0.0f };
    speed_ = { 0.5f };
}

Player::~Player()
{
    aniObject_->UnRegisterObject();
}

void Player::Init(const Vector3& pos)
{
    isJump_ = false;
    zoomTimer_ = 0.0f;
    zoomStartTimer_ = 0.0f;

    //体の位置初期化
    aniObject_->Initialize();

    aniObject_->SetTranslate(pos);
    aniObject_->SetObjectName("Player");
    aniObject_->RegisterObject();

    aniObject_->SetAnimation("Idle");
    aniObject_->UpdateAniTimer();
   
    aniObject_->Update();

    //頭の行列を取得する
    headMatrix_ = aniObject_->GetWorldJointMatrix("Head");

    //目の位置初期化
    eyePosition_->Initialize();

    UpdateRay();

    velocity_ = { 0.0f,0.0f,0.0f };
    speed_ = { 0.5f };


    isThermography_ = false;
    isThermographyEnd_ = false;
    thermography_ = 0.0f;

    PostProcessManager::GetInstance()->
        GetPostEffectMaterial(PostProcessManager::kModel)->
        GetMaterialForDissolve()
        ->maskVal = 1.0f - thermography_;


    Json file = JsonFile::GetJsonFiles("config");

    characterState_.hps.hp = file["CharacterState"]["hp"];
    characterState_.hps.maxHp = file["CharacterState"]["hp"];
    characterState_.isDead = false;
    characterState_.isHit = file["CharacterState"]["isHit"];

    cameraRotateY_ = 0.0f;
    cameraRotateX_ = 0.0f;
}



void Player::UpdateRay()
{
    raySprite_->UpdateRay(Ray{ .origin = eyePosition_->GetWorldTransform().GetWorldPosition(),.diff = GetEyeForward() });
}

void Player::Draw()
{

    if (eyePosition_->IsCameraUpOrDown()) {
        aniObject_->Draw();
    }

}

void Player::DrawRaySprite()
{
    raySprite_->Draw();
}

void Player::Update()
{

    if (characterState_.isHit) {
        if (hitTimer_ > 0.0f) {
            hitTimer_ -= TimeManager::DeltaTime();
        } else {
            hitTimer_ = 0.0f;
            characterState_.isHit = false;
        }
    }

    Move();
    Jump();
    Zoom();

    Thermography();
    MouseLook();
    UpdateRay();

    //クリックしたらサウンド
    if (InputBind::IsClick()) {
        Sound::PlaySE(SoundFactory::SWITCH_ON);
    }

    //アニメーションタイマーのアップデート
    aniObject_->UpdateAniTimer();

    aniObject_->Update();
    headMatrix_ = aniObject_->GetWorldJointMatrix("Head");
    handMatrix_ = aniObject_->GetWorldJointMatrix("Hand.L");

    eyePosition_->Update();

}

void Player::Debug()
{
#ifdef USE_IMGUI

    ImGui::Begin("Player");
    DebugUI::CheckCaracterState(characterState_, "CharacterStage");
    ImGui::Checkbox("isInvincible", &isInvincible_);
    ImGui::SliderFloat3("velocity_", &velocity_.x, -1000.0f, 1000.0f);
    DebugUI::CheckObject3d(*aniObject_);
    DebugUI::ShowMatrix4x4(headMatrix_, "HeadMatrix");
    ImGui::End();

#endif //USE_IMGUI
}

void Player::Move()
{

    velocity_.x = { 0.0f };
    velocity_.z = { 0.0f };


    Vector2 controllerPos = { velocity_.x ,velocity_.z };
    if (Input::IsControllerStickPosMove(BUTTON_LEFT, 0, &controllerPos)) {
        velocity_.x = controllerPos.x;
        velocity_.z = controllerPos.y;
    }

    if (InputBind::IsPressMoveL()) { velocity_.x = -1.0f; }
    if (InputBind::IsPressMoveR()) { velocity_.x = 1.0f; }
    if (InputBind::IsPressMoveF()) { velocity_.z = 1.0f; }
    if (InputBind::IsPressMoveB()) { velocity_.z = -1.0f; }

    float length = Length(Vector2{ velocity_.x,velocity_.z });
    speed_ = (InputBind::IsPressSpeedButton() || length <= 0.5f) ? 0.125f : 0.25f;

    if (fabs(velocity_.x) > 0.0f || fabs(velocity_.z) > 0.0f) {

        if (!isJump_) {
            if (soundTimer_ == 0.0f) {
                Sound::PlaySE(SoundFactory::FOOT_STEP, (speed_ == 0.25f) ? 1.5f : 1.0f);
            }

        }

        if (soundTimer_ < 7.5f) {
            soundTimer_ += speed_;
        } else {
            soundTimer_ = 0.0f;
        }

        //前の方向を取得
        Vector3 forward = GetBodyForward();
        forward.y = 0.0f;

        // forwardに垂直な右方向ベクトルを計算
        Vector3 right = Cross(Vector3(0, 1, 0), forward);
        right = Normalize(right);


        //速度を正規化しそれぞれ足す
     // x, z 成分だけ正規化 
        Vector3 horizontal = Normalize(Vector3{ velocity_.x, 0.0f, velocity_.z });

        auto& transform = aniObject_->GetTransform();
        transform.translate += forward * horizontal.z * speed_;
        transform.translate += right * horizontal.x * speed_;

        aniObject_->SetAnimation("Walk");

    } else {
        aniObject_->SetAnimation("Idle");
    }


}

void Player::Jump()
{

    if (isFloorHit_) {

        isFloorHit_ = false;

        if (InputBind::IsTriggerJump()) {

            if (!isJump_) {
                isJump_ = true;
                velocity_.y = kJumpSpeed_;
            }

        }

    }

    velocity_.y = std::clamp(velocity_.y, -1.0f, kJumpSpeed_);


    velocity_.y -= TimeManager::DeltaTime() * 0.98f;

    auto& transform = aniObject_->GetTransform();
    transform.translate.y += velocity_.y;
}

void Player::Zoom()
{
    const float deltaTime = TimeManager::DeltaTime();

    if (InputBind::IsClickPress()) {

        zoomStartTimer_ += deltaTime;
        zoomStartTimer_ = std::clamp(zoomStartTimer_, 0.0f, 0.2f);



        if (zoomStartTimer_ >= 0.2f) {
            if (!isZoom_) {
                isZoom_ = true;
                Sound::PlaySE(SoundFactory::FALL);
            }

            zoomTimer_ += deltaTime * 2.0f;

        }

    } else {

        zoomStartTimer_ = 0.0f;

        if (zoomTimer_ > 0.0f) {
            zoomTimer_ -= deltaTime * 2.0f;
        } else {
            isZoom_ = false;
        }
    }
    zoomTimer_ = std::clamp(zoomTimer_, 0.0f, 1.0f);
}

Vector3& Player::GetEyeForward()
{
    return eyePosition_->GetForward();
}
Vector3& Player::GetBodyForward()
{
    //前方を取得する
    static Vector3 forward;
    forward = Math::GetForward(aniObject_->GetWorldMatrix());
    return forward;
}
void Player::Thermography()
{

    if (!ItemManager::IsGetSolarDisc()) {
        //ソーラーディスク未取得は早期リターン
        return;
    }

    PostProcessManager::GetInstance()->
        GetPostEffectMaterial(PostProcessManager::kModel)->
        GetMaterialForDissolve()
        ->maskVal = 1.0f - thermography_;

    if (InputBind::IsClickR()) {
        isThermography_ = true;

        if (isThermographyEnd_) {
            isThermographyEnd_ = false;
            thermography_ = 0.0f;
        }
    }

    if (!isThermography_) {
        return;
    }


    const float deltaTime = TimeManager::DeltaTime();

    if (InputBind::IsClickPressR()) {

        if (!isThermographyEnd_) {
            if (thermography_ < 1.0f) {
                thermography_ += deltaTime * 2.0f;
            } else {
                thermography_ = 1.0f;
                isThermographyEnd_ = true;
            }
        }

    } else {
        if (thermography_ > 0.0f) {
            thermography_ -= deltaTime * 2.0f;
        } else {
            thermography_ = 0.0f;
            isThermography_ = false;
        }
    }


}

void Player::MouseLook()
{

    if (eyePosition_->IsCameraUpOrDown()) {
        //上昇時または下降時は回転を固定する
        aniObject_->GetTransform().rotate.y = 0.0f;
        return;
    }

    Vector2 controllerPos = { cameraRotateY_ ,cameraRotateX_ };

    const float kDeltaTime = TimeManager::DeltaTime();

    if (Input::IsControllerStickPosMove(BUTTON_RIGHT, 0, &controllerPos)) {
        cameraRotateY_ += controllerPos.x * kDeltaTime * cameraSpeed_ * 2.0f;
        cameraRotateX_ -= controllerPos.y * kDeltaTime * cameraSpeed_ * 2.0f;
    } else {
        cameraRotateY_ += Input::GetMousePosFiltered().x * kDeltaTime / cameraSpeed_ * 0.03125f;
        cameraRotateX_ += Input::GetMousePosFiltered().y * kDeltaTime / cameraSpeed_ * 0.03125f;
    }

    cameraRotateX_ = std::clamp(
        cameraRotateX_,
        -std::numbers::pi_v<float> *0.5f,
        std::numbers::pi_v<float> *0.5f);
    auto& transform = aniObject_->GetTransform();
    transform.rotate.y = Lerp(transform.rotate.y, cameraRotateY_, 0.5f);


    eyePosition_->MouseLook(cameraRotateX_);

}

void Player::OnCollisionEnemy(const int hitPoint)
{

    if (isInvincible_) {
        //無敵だったらリターンする
        return;
    }

    if (characterState_.isHit) {
        return;
    }

    Sound::PlaySE(SoundFactory::CRACKER);
    //衝突フラグを真に
    characterState_.isHit = true;
    characterState_.hps.hp -= hitPoint;
    hitTimer_ = 1.0f;

    if (characterState_.hps.hp <= 0.0f) {

        characterState_.isDead = true;
    }

}
