#pragma once
#include"../Stage.h"
#include"BackGround/BackGround.h"
#include"../GameObject/Fountain/Fountain.h"
#include"AnubisBlockMap/AnubisBlockMap.h"
#include"Thoth/Thoth.h"

class AnubisStage :public Stage
{

private:

    std::unique_ptr<BackGround>backGround_ = nullptr;
    std::unique_ptr<Fountain>fountain_ = nullptr;
    std::unique_ptr<AnubisBlockMap>anubisBlockMap_ = nullptr;
    std::unique_ptr<Thoth>thoth_ = nullptr;
public:
    AnubisStage();
    void Initialize()override;
    void StageTransitionInitialize()override;
    void Update()override;
    void Draw()override;
    void DrawSprite()override;
    void CheckCollision(CollisionManager& collisionManager)override;

};

