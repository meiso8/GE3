#include "Player.h"

#include"Input.h"
#include"Normalize.h"
#include"Model.h"

Player::Player(const ModelData& modelData) {

    kSpeed_ = 0.0f;
    velocity_ = { 0.0f };
    modelData_ = &modelData;
    model_ = std::make_unique<Model>();

    model_->Create(*modelData_);

}

void Player::Init()
{
    worldTransform_.Initialize();
    velocity_ = { 0.0f,0.0f,0.0f };
    kSpeed_ = { 0.5f };
}

void Player::Draw(Camera& camera, uint32_t lightType)
{
    model_->Draw(worldTransform_.matWorld_, camera, lightType);
}

void Player::Update()
{

    Input* input = Input::GetInstance();

    velocity_ = { 0.0f,0.0f,0.0f };


    if (input->GetJoyStickPos(&velocity_.x, &velocity_.z,Input::BUTTON_LEFT)){
        //JoyStick
        Vector2 rotate = { velocity_.x,velocity_.z };
        worldTransform_.rotate_.y = std::atan2(-rotate.x, -rotate.y); 
    } 

    if (input->IsPushKey(DIK_A)) {
        velocity_.x = -1.0f;
    }

    if (input->IsPushKey(DIK_D)) {
        velocity_.x = 1.0f;
    }


    if (input->IsPushKey(DIK_W)) {
        velocity_.z += 1.0f;
    }

    if (input->IsPushKey(DIK_S)) {
        velocity_.z -= 1.0f;
    }

    kSpeed_ = (input->IsPushKey(DIK_LSHIFT)) ? 0.25f : 0.5f;


    if (std::fabsf(velocity_.x) + std::fabsf(velocity_.y) + std::fabsf(velocity_.z) > 1.0f) {
        velocity_ = Normalize(velocity_);
    }

    worldTransform_.translate_ += velocity_ * kSpeed_;


        WorldTransformUpdate(worldTransform_);

    //ImGui::Begin("Player");
    //ImGui::SliderFloat3("velocity", &velocity_.x, 0.0f, 1.0f);
    //ImGui::End();
}
