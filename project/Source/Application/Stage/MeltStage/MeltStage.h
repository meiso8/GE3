#pragma once
#include"../Stage.h"
#include"BackGround/BackGround.h"
class ParticleEmitter;

class MeltStage :public Stage
{
private:
    std::unique_ptr<BackGround>backGround_ = nullptr;
    std::array<std::unique_ptr<ParticleEmitter>, 3>particleEmitters_;
public:
    MeltStage();
    void Initialize()override;
    void Update()override;
    void Draw(Camera& camera)override;
    void CheckCollision(CollisionManager& collisionManager)override;
private:
    void CreateParticle();
};

