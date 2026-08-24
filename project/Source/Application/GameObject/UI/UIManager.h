#pragma once
#include"HPGage.h"
#include"HPIcon.h"
#include"PauseScreen.h"
#include"CurPos.h"
#include"ButtonSprite/ButtonSprite.h"

#include<unordered_map>
class SlidePuzzleSystem;

class UIManager
{
    enum GageType {
        kEnemy,
        kPlayer,
    };
public:
    UIManager();
    ~UIManager();
    void Initialize();
    void UpdateGage();
    void UpdatePauseScreen();
    void DrawHPGage();
    void DrawPauseScreen();
    void DrawCurPos();
    void DrawEffect();
    void CreateHpGage(HPs& enemyHp, HPs& playerHp);
    Vector2* GetCurPosPtr() {return curPos_->GetScreenPosPtr(); };
private:
    std::unique_ptr<PauseScreen> pauseScreen_;
    std::unordered_map<GageType, std::unique_ptr<HPGage>>hpGages_;
    std::unique_ptr<Sprite> effectSprite_ = nullptr;
    std::unique_ptr<CurPos>  curPos_ = nullptr;
    std::unique_ptr<ButtonSprite>  buttonSprite_ = nullptr;

};

