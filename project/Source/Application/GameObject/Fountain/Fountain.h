#pragma once
#include"Collider.h"
#include <memory>
#include"Object3d.h"
#include "ParticleEmitter.h"
#include<array>

class Fountain :public Collider {
public:
    Fountain();
    void Initialize();
    void Update();
    void Draw(Camera& camera);
    void OnCollision(Collider* collider) override;
private:
    void CreateParticle();
private:
    std::unique_ptr<Object3d> object_;
    std::unique_ptr<Object3d> waterObject_;
    std::array<std::unique_ptr<ParticleEmitter>,2>particleEmitter_;
    float splashTimer_ = 0.0f;

};

