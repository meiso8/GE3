#pragma once
#include"../Stage.h"
#include"BackGround/BackGround.h"

class MeltStage :public Stage
{
private:
    std::unique_ptr<BackGround>backGround_ = nullptr;
public:
    MeltStage();
    void Initialize()override;
    void Update()override;
    void Draw(Camera& camera)override;
    void CheckCollision(CollisionManager& collisionManager)override;
};

