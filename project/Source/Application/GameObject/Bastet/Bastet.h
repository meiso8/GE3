#pragma once
#include"Object3d.h"
#include"Collider.h"

class ParticleEmitter;
class RaySprite;
class Bastet :public Collider
{
public:
    Bastet();
    void Update();
    void  Draw();
    void Initialize();
    void LoadMap(const std::string& tagName = "MeltStage_BastetStep");
    void OnCollision(Collider* collider) override;
    void RayCastHit(RaySprite& raySprite);
private:
    void CreateParticle();
private:
    std::unique_ptr <Object3d> obj_ = nullptr;
    std::unique_ptr<ParticleEmitter>particleEmitter_ = nullptr;
    std::vector<EulerTransform> footSteps_;
    bool isApper_ = false;
    float footStepTimer_  = 0.0f;
    uint32_t step_ = 0;
    std::string jsonFileName_ = "MeltStage_BastetStep";

};

