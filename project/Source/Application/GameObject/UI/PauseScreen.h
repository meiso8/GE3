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

private:
    void TimerUpdate();
    void SelectButton();
private:

    static bool isActive_;
    static bool isPause_;
    static bool isBackToTitle_;

    float pauseTimer_ = 0.0f;
    int selectButtonNum_ = kBackToGame;

    bool isShowMenu_ = false;
    bool isLookGameItem_ = false;
    float scaleTheta_ = 0.0f;
    Vector2* curPos_ = nullptr;
    std::array < Vector2, kMaxLayer> pos_;
    std::array<std::unique_ptr<Sprite>, kMaxLayer> sprites_;
public:

    static void SetisActive(const bool flag) { isActive_ = flag; };
    static void SetIsPause(const bool flag) { isPause_ = flag; };
    static void SetIsBackToTitle(const bool flag) { isBackToTitle_ = flag; };

    static bool GetIsActive() { return isActive_; }
    static bool GetIsPause() { return isPause_; }
    static bool GetIsBackToTitle() { return isBackToTitle_; }


    PauseScreen();
    ~PauseScreen();
    void Initialize();
    void Update();
    void Draw();
    void SetCurPosPtr(Vector2* pos) { curPos_ = pos; };
};

