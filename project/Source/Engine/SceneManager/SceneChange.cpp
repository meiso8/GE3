#include "SceneChange.h"
#include"Window.h"
#include"Easing.h"
#include"ImGuiClass.h"
#include"TimeManager.h"
#include"PostProcessManager/PostProcessManager.h"

std::unordered_map<SceneChange::State, PFunc>SceneChange::StatesUpdate_ =
{
    {State::kFadeIn, &SceneChange::FadeIn},
    {State::kFadeOut, &SceneChange::FadeOut},

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

    float time = 1.0f - timer_ / endTime_;

    
    sprite_->SetColor({ 0.0f,0.0f,0.0f,time });

    auto* grayScaleaterial = PostProcessManager::GetInstance()->
        GetPostEffectMaterial(PostProcessManager::kModel)->
        GetMaterialGrayScale();
    grayScaleaterial->useEffect = true;
    grayScaleaterial->type = PostEffectMaterial::RenderTextureEffectType::kGrayScale;
    grayScaleaterial->effectVol = EaseOutQuadT(time);
}

void SceneChange::FadeIn()
{
    float time = timer_ / endTime_;
    sprite_->SetColor({ 0.0f,0.0f,0.0f,time });

    auto* grayScaleaterial = PostProcessManager::GetInstance()->
        GetPostEffectMaterial(PostProcessManager::kModel)->
        GetMaterialGrayScale();
    grayScaleaterial->useEffect = true;
    grayScaleaterial->type = PostEffectMaterial::RenderTextureEffectType::kGrayScale;
    grayScaleaterial->effectVol = EaseInQuadT(time);
}



void SceneChange::SwitchScene()
{
    if (state_ == kSceneStart || state_ == kSceneEnd) {
        state_ = kUnKnown;

    }
    if (state_ == kFadeOut ) {
        state_ = kSceneStart;
        sprite_->SetColor({ 0.0f,0.0f,0.0f,0.0f});
        auto* grayScaleaterial = PostProcessManager::GetInstance()->
            GetPostEffectMaterial(PostProcessManager::kModel)->
            GetMaterialGrayScale();
        grayScaleaterial->useEffect = false;

    }
    if (state_ == kFadeIn ) {
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
    if (timer_ >= endTime_) { return; }

    timer_+= TimeManager::DeltaTime();

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
