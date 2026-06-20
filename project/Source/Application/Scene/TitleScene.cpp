#include "TitleScene.h"
#include"Input.h"
#include"SampleScene.h"
#include"MakeMatrix.h"
#include"DebugUI.h"
#include"Sound.h"
#include"Quaternion/Quaternion.h"
#include"CoordinateTransform.h"
#include"InputBind.h"
#include"Window.h"
#include"TimeManager.h"
#include"Log.h"
TitleScene::TitleScene()
{
    //タイトル画像 
    titleSprite_ = std::make_unique<Sprite>();
    titleSprite_->Create(TextureFactory::Handle::TITLE, { 640.0f, 256.0f });
    titleSprite_->SetAnchorPoint({ 0.5f, 0.5f });

    // スタートボタン 
    startButton_ = std::make_unique<Sprite>();
    startButton_->Create(TextureFactory::Handle::BUTTON_START, { 640.0f,448.0f });
    startButton_->SetAnchorPoint({ 0.5f, 0.5f });
    // Exitボタン 
    exitButton_ = std::make_unique<Sprite>();
    exitButton_->Create(TextureFactory::Handle::BUTTON_EXIT, { 640.0f, 528.0f });
    exitButton_->SetAnchorPoint({ 0.5f, 0.5f });

    for (auto& sprite : test3Sprites_) {
        sprite = std::make_unique<Sprite>();
        sprite->Create(TextureFactory::Handle::TEST3, { 0.0f, 360.0f });
        sprite->SetSize({ static_cast<float>(Window::GetClientWidth()), static_cast<float>(Window::GetClientHeight()) * 0.5f });
        sprite->SetUVScale({ 0.5f,1.0f,1.0f });
    }

    for (int i = 0; i < test3Sprites_.size(); ++i) {
        test3Sprites_[i]->SetUVTranslate({ 0.0f, 0.0f, 0.0f });
        test3Sprites_[i]->SetPosition({ 0.0f, test3Sprites_[i]->GetSize().y * i });

    }

}

TitleScene::~TitleScene()
{
}

void TitleScene::Initialize()
{
    timer_ = 0.0f;
    for (int i = 0; i < test3Sprites_.size(); ++i) {
        test3Sprites_[i]->SetUVTranslate({ 0.0f, 0.0f, 0.0f });
    }

    isHoverStartButton_ = false;
    isHoverExitButton_ = false;
    sceneChange_->Initialize();
    sceneChange_->SetState(SceneChange::kFadeOut, 1.0f);

    Sound::bgmVolume_ = 0.5f;
    Sound::StopAllSound();

    titleSprite_->SetScale({ 1.0f,1.0f });
}

void TitleScene::Update()
{
    Sound::PlayBGM(SoundFactory::BGM_ArabRuins);

    timer_ += Time::DeltaTime();
    float scale = sinf(timer_) * 0.125f+1.25f;
    titleSprite_->SetScale({ scale,scale });

    test3Sprites_[0]->GetUVTranslate().x += 0.001f;
    test3Sprites_[1]->GetUVTranslate().x -= 0.001f;

    for (auto& sprite : test3Sprites_) {
        sprite->Update();
    }
    Vector2 mousePos = Input::GetCursorPosition();
    Vector2 stickPos = { 0.0f,0.0f };
    Input::IsControllerStickPosMoveTrigger(BUTTON_LEFT, 0, &stickPos);

    if (stickPos.y > 0.1f|| IsCollision(mousePos, *startButton_)) {
        isHoverExitButton_ = false;

        if (!isHoverStartButton_) {
            Sound::PlaySE(SoundFactory::SWITCH_ON);
            isHoverStartButton_ = true;

        }
    }

    if (stickPos.y < -0.1f || IsCollision(mousePos, *exitButton_)) {
        isHoverStartButton_ = false;

        if (!isHoverExitButton_) {
            Sound::PlaySE(SoundFactory::SWITCH_ON);
            isHoverExitButton_ = true;

        }
    }


    if (isHoverStartButton_) {
        startButton_->SetColor({ 1.0f,0.0f,0.0f,1.0f });
        if (InputBind::IsClick()) {
            Sound::PlaySE(SoundFactory::FALL);
            sceneChange_->SetState(SceneChange::kFadeIn, 0.5f);
            SceneManager::SetNextScene("Sample");
        }
    } else {
        startButton_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
    }



    if (isHoverExitButton_) {
        exitButton_->SetColor({ 1.0f,0.0f,0.0f,1.0f });
        if (InputBind::IsClick()) {
            // アプリケーション終了 
            PostQuitMessage(0); // Windows APIでウィンドウを閉じる 
        }
    } else {
        exitButton_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
    }
}

void TitleScene::DrawModel()
{
}

void TitleScene::DrawSprite()
{
    Sprite::PreDraw();
    for (auto& sprite : test3Sprites_) {
        sprite->Draw();
    }
    titleSprite_->Draw();
    startButton_->Draw();
    exitButton_->Draw();

    sceneChange_->Draw();
}


void TitleScene::SceneChangeUpdate()
{


    sceneChange_->Update();
}

