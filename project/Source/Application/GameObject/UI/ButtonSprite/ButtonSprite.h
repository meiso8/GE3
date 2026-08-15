#pragma once
#include<Sprite.h>
#include<memory>
#include<array>

class ButtonSprite
{

public:
    enum Button {
        kButton_UI_A,
        kButton_UI_L,
        kButton_UI_LB,
        kButton_UI_R,
        kButton_UI_RB,
        kButton_UI_X,
        kButtonMaxCount,
    };
public:
    ButtonSprite();
    ~ButtonSprite();
    void Initialize();
    void Update();
    void Draw();
    void SetIsGetThermography(const bool flag) { isGetThermography_ = flag; }
private:
    std::array<std::unique_ptr<Sprite>,kButtonMaxCount> sprites_;
    bool isGetThermographyFirst_ = false;
    bool isGetThermography_ = false;
    float scaleTimerDuration_ = 0.0f;
    float timer_ = 0.0f;
    Vector2 size_ = { 0.0f };
   
};

