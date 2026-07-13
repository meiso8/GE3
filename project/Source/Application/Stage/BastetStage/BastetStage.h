#pragma once
#include"../Stage.h"
#include"BackGround/BackGround.h"
#include"Bastet/Bastet.h"

class ParticleEmitter;

class BastetStage :public Stage
{
private:
    std::unique_ptr<BackGround>backGround_ = nullptr;
    std::unique_ptr<Bastet>bastet_ = nullptr;
    /* std::array<std::unique_ptr<ParticleEmitter>, 2>particleEmitters_;*/
public:
    BastetStage();
    void Initialize()override;
    void Update()override;
    void Draw(Camera& camera)override;
    void CheckCollision(CollisionManager& collisionManager)override;
private:
    //void CreateParticle();
};
