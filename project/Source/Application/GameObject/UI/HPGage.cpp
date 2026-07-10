#include "HPGage.h"
#include"Easing.h"
#include"DebugUI.h"
#include"Input.h"
#include"TimeManager.h"
HPGage::HPGage()
{
    sprites_.emplace(SpriteTypes::layer1, std::make_unique<Sprite>());
    sprites_.emplace(SpriteTypes::layer2, std::make_unique<Sprite>());
    sprites_.emplace(SpriteTypes::MaxHp, std::make_unique<Sprite>());

    sprites_[layer1]->Create(TextureFactory::WHITE_1X1, { 640.0,64.0f }, {1.0f,0.0f,0.0f,1.0f});
    sprites_[layer2]->Create(TextureFactory::WHITE_1X1, { 640.0,64.0f }, { 1.0f,1.0f,0.0f,1.0f });
    sprites_[MaxHp]->Create(TextureFactory::WHITE_1X1, { 640.0,64.0f }, { 0.0f,0.0f,0.0f,0.5f });
}

HPGage::~HPGage()
{

}

void HPGage::Setting(const Vector2& size, const Vector2& pos, const Vector2& anchorPoint)
{
    for (const auto& [type, sprite] : sprites_) {

        sprite->SetSize(size);
        sprite->SetPosition(pos);
        sprite->SetAnchorPoint(anchorPoint);
        sprite->Update();
    }
}

void HPGage::Initialize()
{
    preHP_ = hps_->maxHp;
    timer_ = 0.0f;
    preScale_ = 0.0f;
}

void HPGage::Update()
{

    if (hps_->hp != preHP_) {
        preHP_ = hps_->hp;
        timer_ = 0.0f;
    }

    if (timer_ < 1.0f) {
        timer_ += TimeManager::DeltaTime();
    } else {
        timer_ = 1.0f;
    }

    float scale = Easing::EaseInCubic(preScale_, { static_cast<float>(hps_->hp) / static_cast<float>(hps_->maxHp) }, timer_);
    float preHPScale = Easing::EaseOutBack(preScale_, { static_cast<float>(preHP_) / static_cast<float>(hps_->maxHp)},timer_);
    sprites_[layer1]->SetScale({preHPScale,1.0f });
   
    sprites_[layer2]->SetScale({ scale,1.0f });
    preScale_ = scale;

    DebugUI::CheckSprite(*sprites_[layer1], "HPGage");
}

void HPGage::Draw()
{
    sprites_[layer1]->PreDraw();

    for (const auto& [type, sprite] : sprites_) {
        sprite->Draw();
    }


}
