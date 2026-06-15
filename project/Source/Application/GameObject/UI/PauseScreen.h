#pragma once
#include"Sprite.h"
#include<memory>
#include<array>

class PauseScreen
{

private:
    enum Layer {
        kBlackScreen,
        kBackToGame,
        kBackToTitle,
        kMaxLayer,
    };

    void TimerUpdate();
    void SelectButton();


    float pauseTimer_ = 0.0f;
    int selectButtonNum_ = kBackToGame;

    bool isShowMenu_ = false;
    bool isLookGameItem_ = false;
    float scaleTheta_ = 0.0f;
    Vector2* curPos_ = nullptr;
    std::array < Vector2, kMaxLayer> pos_;
    std::array<std::unique_ptr<Sprite>, kMaxLayer> sprites_;
public:
    static bool isActive_;
    static bool isPause_;
    static bool isBackToTitle;




    PauseScreen();
    void Initialize();
    void Update();
    void Draw();
    void SetCurPosPtr(Vector2* pos) { curPos_ = pos; };
};

