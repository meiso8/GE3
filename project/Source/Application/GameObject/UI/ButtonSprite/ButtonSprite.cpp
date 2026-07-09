#include "ButtonSprite.h"
#include"TransformAni/TransformAni.h"
#include"TimeManager.h"
#include"DebugUI.h"
#include"InputBind.h"
#include"Easing.h"

namespace {
    const float kTimer_ = 10.0f;
    const float kScaleSpeed_ = 0.5f;
}
ButtonSprite::ButtonSprite()
{
    for (auto& sprite : sprites_) {
        sprite = std::make_unique<Sprite>();
        sprite->Create(TextureFactory::UV_CHECKER, { 0.0f,0.0f });
    }

    sprites_[kButton_UI_A]->SetTexture(TextureFactory::UI_A);
    sprites_[kButton_UI_L]->SetTexture(TextureFactory::UI_L);
    sprites_[kButton_UI_LB]->SetTexture(TextureFactory::UI_LB);
    sprites_[kButton_UI_R]->SetTexture(TextureFactory::UI_R);
    sprites_[kButton_UI_RB]->SetTexture(TextureFactory::UI_RB);
    sprites_[kButton_UI_X]->SetTexture(TextureFactory::UI_X);

    sprites_[kButton_UI_A]->SetPosition({ 1080.0f,640.0f });
    sprites_[kButton_UI_L]->SetPosition({ 108.0f,600.0f });
    sprites_[kButton_UI_LB]->SetPosition({ 64.0f,32.0f });
    sprites_[kButton_UI_R]->SetPosition({ 1080.0f,540.0f });
    sprites_[kButton_UI_RB]->SetPosition({ 1080.0f,48.0f });
    sprites_[kButton_UI_X]->SetPosition({ 1048.0f,592.0f });

    sprites_[kButton_UI_RB]->SetAnchorPoint({ 0.5f,0.5f });

    for (auto& sprite : sprites_) {
        sprite->AdjustTextureSize();
    }
}

ButtonSprite::~ButtonSprite()
{
    for (auto& sprite : sprites_) {
        if (sprite) {
            sprite.reset();
        }
     
    }

   
}

void ButtonSprite::Initialize()
{
    isGetThermography_ = false;
    timer_ = 0.0f;
    size_ = { 1.0f,1.0f };
}

void ButtonSprite::Update()
{
    if (isGetThermography_) {

        if (InputBind::IsClickPressR()) {

            scaleTimerDuration_ = 0.0f;
            timer_ = 0.0f;

            sprites_[kButton_UI_RB]->SetColor({ 1.0f,0.0f,0.0f,1.0f });

            size_ = Lerp(size_, { 1.0f,1.0f }, kScaleSpeed_);
            sprites_[kButton_UI_RB]->SetScale(size_);
        } else {
            sprites_[kButton_UI_RB]->SetColor({ 1.0f,1.0f,1.0f,1.0f });

            scaleTimerDuration_ += TimeManager::DeltaTime();
            scaleTimerDuration_ = std::fmod(scaleTimerDuration_, kTimer_);

            if (scaleTimerDuration_ <= kTimer_ * 0.5f) {
                timer_ += TimeManager::DeltaTime();
                timer_ = std::fmod(timer_, 1.0f);
                TransformAni::PoyoPoyo(size_, timer_, 1.0f, 0.0625f);
                sprites_[kButton_UI_RB]->SetScale(size_);
            }
        }
    }

    for (int i = 0; i < kButtonMaxCount; ++i) {
        sprites_[i]->Update();
#ifdef USE_IMGUI
        std::string name = "ButtonSprite" + std::to_string(i);
        DebugUI::CheckSprite(*sprites_[i], name.c_str());
#endif
    }

}

void ButtonSprite::Draw()
{
    for (int i = 0; i < kButtonMaxCount; ++i) {

        if (i == kButton_UI_RB && !isGetThermography_) {
            continue;
        }

        sprites_[i]->Draw();
    }

}
