#include "ResultScene.h"
#include"Input.h"
#include"InputBind.h"
#include"Sound.h"
#include<algorithm>
#include"TimeManager.h"
#include"SceneManager.h"
#include "ResultMedjed/ResultMedjed.h"  
#include"SkyboxObject3d.h"
#include"Sprite.h"
#include"Player/Player.h"

ResultScene::~ResultScene() {}

ResultScene::ResultScene()
{

    skipSprite_ = std::make_unique<Sprite>();
    skipSprite_->Create(TextureFactory::Handle::SKIP, { 1280.0f - 128.0f - 64.0f, 720.0f - 64.0f });
    skipSprite_->SetAnchorPoint({ 0.5f, 0.5f });

    symbolSprite_ = std::make_unique<Sprite>();
    symbolSprite_->Create(TextureFactory::Handle::PUZZLE, { 640.0f, 360.0f });
    symbolSprite_->SetAnchorPoint({ 0.5f, 0.5f });

    // 切り替えるテクスチャの順番
    textureSequence_ = {
        TextureFactory::Handle::ENDING2,
        TextureFactory::Handle::BD_HUNEFER,
        TextureFactory::Handle::ENDING1,
         TextureFactory::Handle::PUZZLE,
    };

    creditSprite_ = std::make_unique<Sprite>();
    creditSprite_->Create(TextureFactory::Handle::CREDIT, { 640.0f, 360.0f });
    creditSprite_->SetAnchorPoint({ 0.5f, 0.5f });
    medjed_ = std::make_unique<ResultMedjed>();

    skyBoxObj_ = std::make_unique<SkyboxObject3d>();
    skyBoxObj_->Create();



}


void ResultScene::Initialize()
{

    camera_->Initialize();
    currentCamera_ = camera_.get();

    sceneChange_->Initialize();
    sceneChange_->SetState(SceneChange::kFadeOut, 1.0f);

    creditSprite_->SetUVTranslate({ 0.0f, 0.0f, 0.0f });
    timer_ = 0.0f;

    currentIndex_ = 0;
    timer_ = 0.0f;
    fadeSpeed_ = 2.0f;
    Sound::StopAllSound();
    lookTimer_ = 0.0f;

    isSkipDraw_ = false;

    medjed_->Initialize();

    skyBoxObj_->Initialize();

    player_ = std::make_unique<Player>();
}

void ResultScene::Update()
{
    Sound::PlayBGM(SoundFactory::BGM_SandCity);

#ifdef _RELEASE
    //リリースビルド時にスキップできるようにする。
    if (InputBind::IsClick()) {

        Sound::PlaySE(SoundFactory::FALL);

        if (lookTimer_ >= 10.0f) {
            sceneChange_->SetState(SceneChange::kFadeIn, 0.5f);
            SceneManager::SetNextScene("Title");
        } else {

            if (lookTimer_ <= 10.0f) {
                lookTimer_ = 10.0f;
                isSkipDraw_ = true;
            }
        }
    }
#endif
    player_->Thermography();

    symbolSprite_->Update();
    const float deltaTime = TimeManager::DeltaTime();
    lookTimer_ += deltaTime;
    timer_ += deltaTime;
    bool switchTex = timer_ >= switchInterval_;
    if (switchTex) {
        timer_ = 0.0f;
        currentIndex_ = (currentIndex_ + 1) % textureSequence_.size();
        symbolSprite_->SetTexture(textureSequence_[currentIndex_]);
        symbolSprite_->AdjustTextureSize();

    }

    if (timer_ <= 0.5f) {
        // αを徐々に増やす 
        alpha_ += fadeSpeed_ * deltaTime;
    }

    if (timer_ >= 4.5f) {
        // αを徐々に 減らす
        alpha_ -= fadeSpeed_ * deltaTime;
    }

    alpha_ = std::clamp(alpha_, 0.0f, 0.5f);
    // 色を設定（白＋α） 
    symbolSprite_->SetColor({ 1.0f, 1.0f, 1.0f, alpha_ });

    // UVのYを上にスクロール 
    creditSprite_->GetUVTranslate().y += scrollSpeed_;
    creditSprite_->Update();


    currentCamera_->UpdateMatrix();

    medjed_->Update(switchTex);

#ifdef _DEBUG
    ImGui::SliderFloat("scrollSpeed", &scrollSpeed_, -1.0f, 1.0f);
    ImGui::SliderFloat("fadeSpeed_", &fadeSpeed_, -1.0f, 1.0f);
#endif // _DEBUG


}

void ResultScene::DrawSprite()
{
    sceneChange_->Draw();
    Sprite::PreDraw();
    symbolSprite_->Draw();
    creditSprite_->Draw();

    if (isSkipDraw_) {
        skipSprite_->Draw();
    }

}



void ResultScene::DrawModel()
{
    skyBoxObj_->Draw(*currentCamera_);

    medjed_->Draw(*currentCamera_);
}

void ResultScene::SceneChangeUpdate()
{
    sceneChange_->Update();
}
