#pragma once
#include"Object3d.h"
#include"Collider.h"

class ParticleEmitter;

class Bastet :public Collider
{
public:
    Bastet();
    void Update();
    void  Draw(Camera& camera);
    void Initialize();
    void OnCollision(Collider* collider) override;
private:
    void CreateParticle();
private:
    std::unique_ptr <Object3d> obj_ = nullptr;
    std::unique_ptr<ParticleEmitter>particleEmitter_ = nullptr;
    std::vector<EulerTransform> footSteps_;
    bool isApper_ = false;
    float footStepTimer_  = 0.0f;
    uint32_t step_ = 0;

};

