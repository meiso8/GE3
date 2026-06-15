#include "PauseScreen.h"
#include"Input.h"
#include"Easing.h"
#include"Window.h"
#include<algorithm>
#include"DebugUI.h"
#include"InputBind.h"
#include"Collision.h"
#include"Puzzle/SlidePuzzleSystem.h"
#include"Sound.h"
#include"TimeManager.h"
#include"Memo/MemoManager.h"
#include"RenderTexture/RenderTexture.h"



bool PauseScreen::isActive_ = false;
bool PauseScreen::isPause_ = false;
bool PauseScreen::isBackToTitle = false;

PauseScreen::PauseScreen()
{
    for (int i = 0; i < kMaxLayer; ++i) {
        sprites_[i] = std::make_unique<Sprite>();
    }

    float width = static_cast<float>(Window::GetClientWidth());
    float height = static_cast<float>(Window::GetClientHeight());

    sprites_[kBlackScreen]->Create(TextureFactory::WHITE_1X1, { 0.0f,0.0f }, { 0.0f,0.0f,0.0f,0.75f });
    sprites_[kBlackScreen]->SetSize({ width,height });

    sprites_[kBackToGame]->Create(TextureFactory::BUTTON_BACK_TO_GAME, { 0.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f });
    pos_[kBackToGame] = { 256.0f + 128.0f,  528.0f + sprites_[kBackToGame]->GetSize().y * 0.5f };
    sprites_[kBackToGame]->SetAnchorPoint({ 0.5f,0.5f });

    sprites_[kBackToTitle]->Create(TextureFactory::BUTTON_BACK_TO_TITL, { 0.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f });
    pos_[kBackToTitle] = { width - sprites_[kBackToTitle]->GetSize().x * 0.5f - 256.0f,  528.0f + sprites_[kBackToTitle]->GetSize().y * 0.5f };
    sprites_[kBackToTitle]->SetAnchorPoint({ 0.5f,0.5f });

    for (int i = kBackToGame; i < sprites_.size(); ++i) {
        sprites_[i]->SetPosition(pos_[i]);
    }

}
void PauseScreen::Initialize()
{
    isPause_ = false;
    
    isShowMenu_ = false;
    isLookGameItem_ = false;

    isBackToTitle = false;
    isActive_ = false;
    pauseTimer_ = 0.0f;
    scaleTheta_ = 0.0f;

    selectButtonNum_ = kBackToGame;

    for (int i = kBackToGame; i < kMaxLayer; ++i) {
        sprites_[i]->SetScale({ 1.0f,1.0f });
    }
}
void PauseScreen::Update()
{

    if (InputBind::IsTriggerMenuKey()) {
        isShowMenu_ = !isShowMenu_;
    }


    //タイトル
    isLookGameItem_ = SlidePuzzleSystem::IsActive() || MemoManager::isLookItem_;

    if (isLookGameItem_ ||isShowMenu_) {

        RenderTexture::GetInstance()->GetMaterialGaussianFilter()->sigma = 4.0f;
        RenderTexture::GetInstance()->GetMaterialGaussianFilter()->kernel = 10;

        if (!isPause_) {
            isPause_ = true;
            pauseTimer_ = 0.0f;
            isActive_ = true;
            selectButtonNum_ = kBackToGame;
            Input::SetShowCursor(true);

            float width = static_cast<float>(Window::GetClientWidth());
            float height = static_cast<float>(Window::GetClientHeight());

            Input::SetCursorPosition({ width * 0.5f, height * 0.5f });
        }

    } else {

        RenderTexture::GetInstance()->GetMaterialGaussianFilter()->sigma = 1.0f;
        RenderTexture::GetInstance()->GetMaterialGaussianFilter()->kernel = 0;

        isPause_ = false;
        isActive_ = false;
        Input::SetShowCursor(false);
#ifdef _DEVELOP
        Input::SetShowCursor(true);
#endif

    }

    for (int i = 0; i < sprites_.size(); ++i) {
        std::string world = std::to_string(i);
        DebugUI::CheckSprite(*sprites_[i], world.c_str());
    }

    if (!isActive_) { return; }

    TimerUpdate();

    //アイテムを見ているときトそうでないときで変更する
    Vector2 pos = pos_[kBackToGame];

    if (isLookGameItem_) {
       pos.x = 640.0f;
    }

    sprites_[kBackToGame]->SetPosition(pos);

    if (isPause_) {

        SelectButton();
    } else {

        for (int i = kBackToGame; i < kMaxLayer; ++i) {
            sprites_[i]->SetScale({ 1.0f,1.0f });
        }

        isActive_ = false;

    }

}

void PauseScreen::TimerUpdate()
{
    if (pauseTimer_ == 1.0f) {
        return;
    }

    pauseTimer_ += Time::DeltaTime();
    pauseTimer_ = std::clamp(pauseTimer_, 0.0f, 1.0f);
}

void PauseScreen::SelectButton()
{

    for (int i = kBackToGame; i < kMaxLayer; ++i) {
        sprites_[i]->SetColor({ 1.0f,1.0f,1.0f,1.0f });
    }


    for (int i = kBackToGame; i < kMaxLayer; ++i) {
       
        if (isLookGameItem_&& i == kBackToTitle) {
            continue;
        }

        if (IsCollision(*sprites_[i], *curPos_)) {
            
            selectButtonNum_ = i;
            
            sprites_[i]->SetColor({ 1.0f,0.0f,0.0f,1.0f });

            if (InputBind::IsClick()) {
                
                Sound::PlaySE(SoundFactory::FALL);

                switch (selectButtonNum_)
                {
                case kBackToGame:
                    isActive_ = true;
                    isPause_ = false;
                    isShowMenu_ = false;
                    pauseTimer_ = 0.0f;
                    SlidePuzzleSystem::SetActive(false);
                    MemoManager::isLookItem_ = false;
                    break;
                case kBackToTitle:
                    isBackToTitle = true;
                    break;
                }

            }
        }
    }





}

void PauseScreen::Draw()
{
    if (!isActive_) { return; }

    Sprite::PreDraw();

    sprites_[kBlackScreen]->Draw();

    for (int i = kBackToGame; i < kMaxLayer; ++i) {
        //タイトルに戻る
        if (isLookGameItem_ && i == kBackToTitle) {
            continue;
        }
        sprites_[i]->Draw();
    }


}
