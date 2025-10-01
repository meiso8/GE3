#include "Player.h"

#include"MyEngine.h"

Player::Player(MyEngine& myEngine,const ModelData& modelData) {

    modelData_ = &modelData;
    model_ = std::make_unique<Model>();
    model_->Create(*modelData_, myEngine.GetModelConfig(0), myEngine.GetDevice(), myEngine.GetSrvDescriptorHeap(), 2);

}

void Player::Init()
{
    worldTransform_.Initialize();
}

void Player::Draw(Camera& camera)
{
    model_->PreDraw();
    model_->Draw(worldTransform_.matWorld_, camera);
}

void Player::Update()
{
    if (Input::GetInstance()->IsPushKey(DIK_A)) {
       worldTransform_.translate_.x -= 1.0f;
    }

    if (Input::GetInstance()->IsPushKey(DIK_D)) {
        worldTransform_.translate_.x += 1.0f;
    }


    if (Input::GetInstance()->IsPushKey(DIK_W)) {
        worldTransform_.translate_.y -= 1.0f;
    }

    if (Input::GetInstance()->IsPushKey(DIK_S)) {
        worldTransform_.translate_.y += 1.0f;
    }


    WorldTransformUpdate(worldTransform_);


}
