#include "SceneChange.h"
#include"Window.h"
#include"Easing.h"
#include"ImGuiClass.h"
#include"TimeManager.h"

float SceneChange::timer_ = 0.0f;
float SceneChange::endTime_ = 1.0f;

std::unordered_map<SceneChange::State, PFunc>SceneChange::StatesUpdate_ =
{
    {State::kFadeIn, &SceneChange::FadeIn},
    {State::kFadeOut, &SceneChange::FadeOut},
    {State::kWipeIn, &SceneChange::WipeIn},
    {State::kWipeOut, &SceneChange::WipeOut},
};

SceneChange::~SceneChange()
{
}

SceneChange::SceneChange()
{
    sprite_ = std::make_unique<Sprite>();
    sprite_->Create(TextureFactory::WHITE_1X1, { 0.0f,0.0f }, { 0.0f,0.0f,0.0f,1.0f });
    sprite_->SetSize(Vector2{ static_cast<float>(Window::GetClientWidth()), static_cast<float>(Window::GetClientHeight()) });
}

void SceneChange::SetState(State state, const float endTime)
{
    if (state == kUnKnown) { return; }
    if (state == state_) { return; }
    state_ = state;
    timer_ = 0.0f;
    endTime_ = endTime;
}

void SceneChange::Initialize()
{
    timer_ = 0.0f;
    state_ = kUnKnown;
}

void SceneChange::FadeOut()
{
    sprite_->SetScale({ 1.0f,1.0f });
    sprite_->SetColor({ 0.0f,0.0f,0.0f,1.0f - timer_ / endTime_ });
}

void SceneChange::FadeIn()
{
    sprite_->SetScale({ 1.0f,1.0f });
    sprite_->SetColor({ 0.0f,0.0f,0.0f,timer_ / endTime_ });
}

void SceneChange::WipeOut()
{
    sprite_->SetColor({ 0.0f,0.0f,0.0f,1.0 });
    sprite_->SetScale(Easing::EaseOutSine(Vector2{ 1.0f,1.0f }, { 0.0f,1.0f }, timer_ / endTime_));
}

void SceneChange::WipeIn()
{
    sprite_->SetColor({ 0.0f,0.0f,0.0f,1.0 });
    sprite_->SetScale(Easing::EaseOutSine(Vector2{ 0.0f,1.0f }, { 1.0f,1.0f }, timer_ / endTime_));
}


void SceneChange::SwitchScene()
{
    if (state_ == kSceneStart || state_ == kSceneEnd) {
        state_ = kUnKnown;
    }
    if (state_ == kFadeOut || state_ == kWipeOut) {
        state_ = kSceneStart;
        sprite_->SetColor({ 0.0f,0.0f,0.0f,0.0f});
    }
    if (state_ == kFadeIn || state_ == kWipeIn) {
        state_ = kSceneEnd;
    }
}

void SceneChange::Update()
{
    TimerUpdate();

    if (state_ == kUnKnown || state_ == kSceneStart || state_ == kSceneEnd) { return; }

    (this->*StatesUpdate_[state_])();
}

void SceneChange::TimerUpdate()
{
    if (timer_ == endTime_) { return; }

    timer_+= Time::DeltaTime();

    if (timer_ >= endTime_) {
        timer_ = endTime_;
        SwitchScene();
    }

}

void SceneChange::Draw()
{
    if (state_ == kUnKnown) { return; }

    if (sprite_ == nullptr) {
        return;
    }
    sprite_->PreDraw();
    sprite_->Draw();
}
