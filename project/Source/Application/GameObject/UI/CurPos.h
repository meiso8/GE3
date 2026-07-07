#pragma once
#include"Sprite.h"
#include<memory>
#include"../Puzzle/Puzzle.h"
#include<vector>
class CurPos
{
private:
    Vector2 centerPos_ = { 0.0f };
    Vector2 screenPos_ = { 0.0f };
    std::unique_ptr<Sprite>  curPos_ = nullptr;
    Vector2 curPosSpeed_ = {0.0f};
    const float kSpeed_ = 12.0f;
    Puzzle* puzzle_ = nullptr;
public:
    CurPos();
    void Update();
    void Draw();
    void Initialize();
    Vector2* GetScreenPosPtr() { return &screenPos_; };
    void SetPuzzle(Puzzle& puzzle) { puzzle_ = &puzzle; };
};

Vector2 SnapCursorToNearestSprite(const Vector2& cursorPos, std::vector<std::unique_ptr<Sprite>>& sprites, float snapThreshold = 70.0f);