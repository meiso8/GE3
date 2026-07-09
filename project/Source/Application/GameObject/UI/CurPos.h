#pragma once
#include<memory>
#include<vector>
#include"Vector2.h"
#include"Sprite.h"

class SlidePuzzleSystem;

class CurPos
{
private:
    Vector2 centerPos_ = { 0.0f };
    Vector2 screenPos_ = { 0.0f };
    std::unique_ptr<Sprite>  curPos_ = nullptr;
    Vector2 curPosSpeed_ = {0.0f};
    const float kSpeed_ = 12.0f;
    SlidePuzzleSystem* slidePuzzleSystem_ = nullptr;
public:
    CurPos();
    void Update();
    void Draw();
    void Initialize();
    Vector2* GetScreenPosPtr() { return &screenPos_; };
    void SetSlidePuzzleSystem(SlidePuzzleSystem* puzzle) { slidePuzzleSystem_ = puzzle; };
};

Vector2 SnapCursorToNearestSprite(const Vector2& cursorPos, std::vector<std::unique_ptr<Sprite>>& sprites, float snapThreshold = 70.0f);