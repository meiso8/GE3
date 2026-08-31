#pragma once
#include"../Stage.h"

class Entrance :public Stage
{
private:


public:
    void Initialize()override;
    void StageTransitionInitialize()override;
    void Update()override;
    void Draw()override;
    void CheckCollision(CollisionManager& collisionManager)override;
};

