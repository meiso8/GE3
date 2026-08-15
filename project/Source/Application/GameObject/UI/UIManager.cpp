#include "UIManager.h"
#include"Window.h"
#include"ItemManager/ItemManager.h"
#include"Input.h"

UIManager::UIManager()
{
    effectSprite_ = std::make_unique<Sprite>();
    effectSprite_->Create(TextureFactory::WHITE_1X1, { 0.0f,0.0f }, { 1.0f,0.75f,0.75f,1.0f });
    effectSprite_->SetSize({ 1280.0f,720.0f });

    buttonSprite_ = std::make_unique<ButtonSprite>();

    curPos_ = std::make_unique<CurPos>();
    curPos_->Initialize();

    pauseScreen_ = std::make_unique<PauseScreen>();
    pauseScreen_->SetCurPosPtr(curPos_->GetScreenPosPtr());
}

UIManager::~UIManager()
{
    hpGages_.clear();

}

void UIManager::Initialize()
{
    for (const auto& [type, gage] : hpGages_) {
        gage->Initialize();
    }

    buttonSprite_->Initialize();
    pauseScreen_->Initialize();
    curPos_->Initialize();
}

void UIManager::UpdateGage()
{
    for (const auto& [type, gage] : hpGages_) {
        gage->Update();
    }


}

void UIManager::UpdatePauseScreen()
{

    //太陽円盤によってサーモ可能かどうか
    buttonSprite_->SetIsGetThermography(ItemManager::IsGetSolarDisc());

    buttonSprite_->Update();

    if (pauseScreen_->GetIsActive()) {
        curPos_->Update();
    } else {
        curPos_->Initialize();
#ifdef _RELEASE
        if(!Input::IsPressKey(DIK_ESCAPE)) {
            Input::SetCursorPosition(*curPos_->GetScreenPosPtr());
        }
   
#endif // _RELEASE

    }

    pauseScreen_->Update();

}

void UIManager::DrawHPGage()
{
    for (const auto& [type, gage] : hpGages_) {
        gage->Draw();
    }

}

void UIManager::DrawPauseScreen()
{
    pauseScreen_->Draw();


}

void UIManager::DrawCurPos()
{
    Sprite::PreDraw();

    buttonSprite_->Draw();

    if (pauseScreen_->GetIsActive()) {
        curPos_->Draw();
    }

}

void UIManager::DrawEffect()
{
    Sprite::PreDraw(kBlendModeMultiply);
    effectSprite_->Draw();
}

void UIManager::CreateHpGage(HPs& enemyHp, HPs& playerHp)
{
    hpGages_.emplace(GageType::kEnemy, std::make_unique<HPGage>());
    hpGages_[kEnemy]->SetHpPtr(&enemyHp);
    hpGages_[kEnemy]->Setting({ 640.0f,16.0f }, { 640.0f,32.0f }, { 0.5f,0.0f });

    hpGages_.emplace(GageType::kPlayer, std::make_unique<HPGage>());
    hpGages_[kPlayer]->SetHpPtr(&playerHp);
    hpGages_[kPlayer]->Setting({ 640.0f,16.0f }, { 640.0f,static_cast<float>(Window::GetClientHeight()) - 136.0f }, { 0.5f,0.0f });

}


