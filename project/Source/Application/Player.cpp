#include "Player.h"

#include"MyEngine.h"
#include"Normalize.h"

Player::Player(MyEngine& myEngine, const ModelData& modelData) {

    modelData_ = &modelData;
    model_ = std::make_unique<Model>();
    model_->Create(*modelData_, myEngine.GetModelConfig(), 3);

}

void Player::Init()
{
    worldTransform_.Initialize();
    velocity_ = { 0.0f,0.0f,0.0f };
    kSpeed_ = { 0.5f };
}

void Player::Draw(Camera& camera,uint32_t lightType)
{
    model_->Draw(worldTransform_.matWorld_, camera, lightType);
}

void Player::Update()
{

    velocity_ = { 0.0f,0.0f,0.0f };

    if (Input::GetInstance()->IsPushKey(DIK_A)) {
        velocity_.x = -1.0f;
    }

    if (Input::GetInstance()->IsPushKey(DIK_D)) {
        velocity_.x = 1.0f;
    }


    if (Input::GetInstance()->IsPushKey(DIK_W)) {
        velocity_.z -= 1.0f;
    }

    if (Input::GetInstance()->IsPushKey(DIK_S)) {
        velocity_.z += 1.0f;
    }

    kSpeed_ = (Input::GetInstance()->IsPushKey(DIK_LSHIFT)) ? 0.25f : 0.5f;

    if (Input::GetInstance()->IsPushKey(DIK_A) || Input::GetInstance()->IsPushKey(DIK_D) || Input::GetInstance()->IsPushKey(DIK_W) || Input::GetInstance()->IsPushKey(DIK_S)) {
        velocity_ = Normalize(velocity_);
        worldTransform_.translate_ += velocity_ * kSpeed_;
    }

    WorldTransformUpdate(worldTransform_);

    //ImGui::Begin("Player");
    //ImGui::SliderFloat3("velocity", &velocity_.x, 0.0f, 1.0f);
    //ImGui::End();
}
