#pragma once
#include"BaseScene.h"
#include "Sprite.h"
#include<memory>
#include<array>

class TitleScene :public BaseScene
{
public:
    TitleScene();
    ~TitleScene() override;
    void Initialize()override;
    void Update()override;
    void DrawModel()override;
    void DrawSprite()override;
   void SceneChangeUpdate()override;
private:
    std::unique_ptr<Sprite> titleSprite_;
    std::unique_ptr<Sprite> startButton_;
    std::unique_ptr<Sprite> exitButton_;
    std::array<std::unique_ptr<Sprite> ,2>test3Sprites_;
    bool isHoverStartButton_ = false;
    bool isHoverExitButton_ = false;
    float timer_ = 0.0f;
};

