#pragma once
#include"../Stage.h"
#include"InformationRoom/InformationRoom.h"
class InformationStage :public Stage
{
private:
    //部屋
    std::unique_ptr<InformationRoom>room_ = nullptr;

public:
    InformationStage();
    void Initialize()override;
    void StageTransitionInitialize()override;
    void Update()override;
    void Draw(Camera& camera)override;
    void CheckCollision(CollisionManager& collisionManager)override;

};

