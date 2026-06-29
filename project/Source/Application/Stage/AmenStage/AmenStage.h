#pragma once

#include"../Stage.h"

#include"Puzzle/SlidePuzzleSystem.h"

#include<memory>
#include"Memo/MemoManager.h"
#include"AmenRa.h"
#include"BackGround/BackGround.h"

class AmenStage :public Stage
{

private:
    std::unique_ptr<SlidePuzzleSystem>slidePuzzleSystem_ = nullptr;
    std::unique_ptr<BackGround>backGround_ = nullptr;
    std::unique_ptr<AmenRa>amenRa_ = nullptr;

public:
    const bool IsClear();
    AmenStage();
    void Initialize()override;
    void Update()override;
    void Draw(Camera& camera)override;
    void DrawSprite()override;
    void CheckCollision(CollisionManager& collisionManager)override;
};

