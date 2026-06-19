#pragma once
#include"../Stage.h"
#include"BackGround/BackGround.h"
class ParticleEmitter;

class AnubisStage :public Stage
{

private:

    std::unique_ptr<BackGround>backGround_ = nullptr;
    std::unique_ptr<ParticleEmitter>particleEmitters_;
public:
    AnubisStage();
    void Initialize()override;
    void Update()override;
    void Draw(Camera& camera)override;
    void DrawSprite()override;
    void CheckCollision(CollisionManager& collisionManager)override;
private:
    void CreateParticle();
};

