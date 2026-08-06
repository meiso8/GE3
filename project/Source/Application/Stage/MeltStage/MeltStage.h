#pragma once
#include"../Stage.h"
#include"BackGround/BackGround.h"
#include"MeltBlockMap/MeltBlockMap.h"
#include"Bastet/Bastet.h"

class ParticleEmitter;

class MeltStage :public Stage
{
private:
    std::unique_ptr<BackGround>backGround_ = nullptr;
    std::unique_ptr<MeltBlockMap>meltBlockMap_ = nullptr;
    std::array<std::unique_ptr<ParticleEmitter>, 3>particleEmitters_;
    std::unique_ptr<Bastet>bastet_ = nullptr;
public:
    MeltStage();
    void Initialize()override;
    void StageTransitionInitialize()override;
    void Update()override;
    void Draw(Camera& camera)override;
    void CheckCollision(CollisionManager& collisionManager)override;
private:
    void CreateParticle();
};

