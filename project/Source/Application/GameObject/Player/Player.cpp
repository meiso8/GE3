#define NOMINMAX
#include<numbers>
#include "Player.h"
#include"ModelManager.h"
#include"Model.h"
#include"MyEngine.h"
#include"Easing.h"
#include<algorithm>
#include"Collision.h"
#include"JsonFile.h"
#include"LightingManager.h"
#include"MakeMatrix.h"
#include"CoordinateTransform.h"
#include"Sprite.h"
#include"CollisionConfig.h"
#include"InputBind.h"
#include"TimeManager.h"
#include"RenderTexture/RenderTexture.h"

void Player::OnCollision(Collider* collider)
{

    if (collider->GetCollisionAttribute() == kCollisionEnemy
        || collider->GetCollisionAttribute() == kCollisionEnemyBullet
        || collider->GetCollisionAttribute() == kCollisionMedjed
        || collider->GetCollisionAttribute() == kCollisionMummy

        ) {
        OnCollisionEnemy();
    }

    if (collider->GetCollisionAttribute() == kCollisionFloor) {
        isFloorHit_ = true;
        isJump_ = false;
    }

    if (collider->GetCollisionAttribute() == kCollisionDummyMedjed
        || collider->GetCollisionAttribute() == kCollisionWall
        || collider->GetCollisionAttribute() == kCollisionMedjed
        || collider->GetCollisionAttribute() == kCollisionEnemy
        || collider->GetCollisionAttribute() == kCollisionMummy
        || collider->GetCollisionAttribute() == kCollisionFloor

        ) {

        ResolveCollision(bodyPos_.worldTransform_.translate_, velocity_, GetCollisionInfo());

    }



    OnCollisionCollider();

}

Player::Player() {

    //モデルを取得する
    model_ = ModelManager::GetModel("player");

    float radius = 0.25f;
    localAabb_.min = { -radius , 0.0f ,-radius };
    localAabb_.max = { radius , 1.5f ,radius };

    SetAABB(localAabb_);
    SetCollisionAttribute(kCollisionPlayer);
    SetCollisionMask(kCollisionEnemy | kCollisionEnemyBullet | kCollisionMedjed | kCollisionDummyMedjed | kCollisionWall | kCollisionMummy | kCollisionWater | kCollisionFloor);

    //それぞれのObject3d（WorldTransform）を作る
    bodyPos_.Create();
    bodyPos_.SetMeshAndMaterial(model_);
    SetWorldMatrix(bodyPos_.worldTransform_.matWorld_);


    raySprite_ = std::make_unique<RaySprite>();
    eyeCollider_ = std::make_unique<EyeCollider>();
    //体の位置を親に設定
    eyeCollider_->SetParent(bodyPos_.worldTransform_);
}

void Player::Init()
{

    isJump_ = false;
    zoomTimer_ = 0.0f;
    zoomStartTimer_ = 0.0f;
    //体の位置初期化
    bodyPos_.Initialize();
    //目の位置初期化
    eyeCollider_->Initialize();


    velocity_ = { 0.0f,0.0f,0.0f };
    kSpeed_ = { 0.5f };
    lookBackTime_ = 1.0f;
    isLookBackEnd_ = true;

    isThermography_ = false;
    isThermographyEnd_ = false;
    thermography_ = 0.0f;

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
    raySprite_->UpdateRay(Ray{ .origin = eyeCollider_->GetWorldTransform().GetWorldPosition(),.diff = GetForward() });
}

void Player::Draw(Camera& camera, const LightMode& lightType)
{
#ifdef _DEBUG
    bodyPos_.SetLightMode(lightType);
    bodyPos_.Draw(camera, kBlendModeNormal);
    eyeCollider_->Draw(camera);
    ColliderDraw(camera);
#endif
}

void Player::DrawRaySprite()
{
    raySprite_->Draw();
}

void Player::Update()
{

    if (characterState_.isHit) {
        if (hitTimer_ > 0.0f) {
            hitTimer_ -= kInverseFPS;
        } else {
            hitTimer_ = 0.0f;
            characterState_.isHit = false;
        }
    }

    Move();
    Jump();
    Zoom();
    /*   LookBack();*/
    Thermography();
    MouseLook();
    UpdateRay();
    //クリックしたらサウンド
    if (InputBind::IsClick()) {
        Sound::PlaySE(SoundFactory::SWITCH_ON);
    }

    bodyPos_.Update();
    eyeCollider_->Update();

    ColliderUpdate();

#ifdef _DEBUG
    DebugUI::CheckCaracterState(characterState_, "player");
    ImGui::SliderFloat3("velocity_", &velocity_.x, -1000.0f, 1000.0f);
#endif // _DEBUG



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
    kSpeed_ = (InputBind::IsPressSpeedButton() || length <= 0.5f) ? 0.125f : 0.25f;

    if (fabs(velocity_.x) > 0.0f || fabs(velocity_.z) > 0.0f) {

        if (!isJump_) {
            if (soundTimer_ == 0.0f) {
                Sound::PlaySE(SoundFactory::FOOT_STEP, (kSpeed_ == 0.25f) ? 0.5f : 0.0f);
            }

        }

        if (soundTimer_ < 7.5f) {
            soundTimer_ += kSpeed_;
        } else {
            soundTimer_ = 0.0f;
        }



        //前の方向を取得
        Vector3 forward = GetForward();
        forward.y = 0.0f;

        // forwardに垂直な右方向ベクトルを計算
        Vector3 right = Cross(Vector3(0, 1, 0), forward);
        right = Normalize(right);

        //移動時の縦揺れを再現　速さによって揺れの周期を変更
        eyeCollider_->Walk(kSpeed_);

        //速度を正規化しそれぞれ足す
     // x, z 成分だけ正規化 
        Vector3 horizontal = Normalize(Vector3{ velocity_.x, 0.0f, velocity_.z });

        bodyPos_.worldTransform_.translate_ += forward * horizontal.z * kSpeed_;
        bodyPos_.worldTransform_.translate_ += right * horizontal.x * kSpeed_;
    } else {
        eyeCollider_->WalkStop();
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


    velocity_.y -= kInverseFPS * 0.98f;
    bodyPos_.worldTransform_.translate_.y += velocity_.y;
}

void Player::Zoom()
{
    if (InputBind::IsClickPress()) {

        zoomStartTimer_ += kInverseFPS;
        zoomStartTimer_ = std::clamp(zoomStartTimer_, 0.0f, 0.2f);



        if (zoomStartTimer_ >= 0.2f) {
            if (!isZoom_) {
                isZoom_ = true;
                Sound::PlaySE(SoundFactory::FALL);
            }

            zoomTimer_ += kInverseFPS * 2.0f;

        }

    } else {

        zoomStartTimer_ = 0.0f;

        if (zoomTimer_ > 0.0f) {
            zoomTimer_ -= kInverseFPS * 2.0f;
        } else {
            isZoom_ = false;
        }
    }
    zoomTimer_ = std::clamp(zoomTimer_, 0.0f, 1.0f);
}

Vector3& Player::GetForward()
{
    return eyeCollider_->GetForward();
}

void Player::LookBack()
{

    if (InputBind::IsClickR()) {
        isLookBack_ = true;

        if (isLookBackEnd_) {
            isLookBackEnd_ = false;
            lookBackTime_ = 0.0f;
            startRotateY = bodyPos_.worldTransform_.rotate_.y;
            endRotateY_ = bodyPos_.worldTransform_.rotate_.y + std::numbers::pi_v<float>;
        }

    }

    if (!isLookBack_) {
        return;
    }

    if (InputBind::IsClickPressR()) {

        if (!isLookBackEnd_) {
            if (lookBackTime_ < 1.0f) {
                lookBackTime_ += kInverseFPS * 2.0f;
            } else {
                lookBackTime_ = 1.0f;
                isLookBackEnd_ = true;
            }
        }
        bodyPos_.worldTransform_.rotate_.y = Easing::EaseOutBack(startRotateY, endRotateY_, lookBackTime_);

    } else {
        if (lookBackTime_ > 0.0f) {
            lookBackTime_ -= kInverseFPS * 2.0f;
        } else {
            lookBackTime_ = 0.0f;
            isLookBack_ = false;
        }

        bodyPos_.worldTransform_.rotate_.y = Easing::EaseOutQuad(startRotateY, endRotateY_, lookBackTime_);

    }


}

void Player::Thermography()
{

    RenderTexture::GetInstance()->GetMaterialDissolve()->maskVal = 1.0f - thermography_;

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

    if (InputBind::IsClickPressR()) {

        if (!isThermographyEnd_) {
            if (thermography_ < 1.0f) {
                thermography_ += kInverseFPS * 2.0f;
            } else {
                thermography_ = 1.0f;
                isThermographyEnd_ = true;
            }
        }

    } else {
        if (thermography_ > 0.0f) {
            thermography_ -= kInverseFPS * 2.0f;
        } else {
            thermography_ = 0.0f;
            isThermography_ = false;
        }
    }


}

void Player::MouseLook()
{

    if (isLookBack_) {
        return;
    }

    Vector2 controllerPos = { cameraRotateY_ ,cameraRotateX_ };

    if (Input::IsControllerStickPosMove(BUTTON_RIGHT, 0, &controllerPos)) {
        cameraRotateY_ += controllerPos.x * kInverseFPS * cameraSpeed_ * 2.0f;
        cameraRotateX_ -= controllerPos.y * kInverseFPS * cameraSpeed_ * 2.0f;
    }


    cameraRotateY_ += Input::GetMousePosFiltered().x * kInverseFPS / cameraSpeed_;
    cameraRotateX_ += Input::GetMousePosFiltered().y * kInverseFPS / cameraSpeed_;

    cameraRotateX_ = std::clamp(
        cameraRotateX_,
        -std::numbers::pi_v<float> *0.5f,
        std::numbers::pi_v<float> *0.5f);

    bodyPos_.worldTransform_.rotate_.y = Lerp(bodyPos_.worldTransform_.rotate_.y, cameraRotateY_, 0.5f);
    eyeCollider_->MouseLook(cameraRotateX_);

}

void Player::OnCollisionEnemy()
{
    if (characterState_.isHit) {
        return;
    }

    Sound::PlaySE(SoundFactory::CRACKER);
    //衝突フラグを真に
    characterState_.isHit = true;
    characterState_.hps.hp -= 10;
    hitTimer_ = 1.0f;

    if (characterState_.hps.hp <= 0.0f) {
        characterState_.isDead = true;
    }

}
