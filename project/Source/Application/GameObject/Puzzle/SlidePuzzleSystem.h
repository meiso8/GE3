#pragma once

#include"Player/RaySprite.h"
#include<memory>
#include"Puzzle.h"
#include"Camera.h"
#include"PuzzleObj.h"

class SlidePuzzleSystem {
public:
    SlidePuzzleSystem();
    ~SlidePuzzleSystem();
    void Initialize();
    void Update(const Vector2& screenPos);
    void Draw(Camera& camera);
    void DrawUI();
    void RayCastHit(RaySprite& ray);
    bool GetIsGameEnd() {
        return isEnd_;
    };
    static bool IsActive() { return isActive_; };
    static void SetActive(bool active) { isActive_ = active; };
    PuzzleObj* GetPuzzleObj() {
        return puzzleObj_.get();
    };
     static Puzzle* GetPuzzle() { return puzzle_.get(); };

private:
    static std::unique_ptr<Puzzle>puzzle_;
    std::unique_ptr<PuzzleObj>puzzleObj_ = nullptr;

    static bool isActive_;
    static bool isEnd_;
};
