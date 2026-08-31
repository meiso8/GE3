#pragma once
#include"Collider.h"
#include <memory>
#include"Object3d.h"
#include "ParticleEmitter.h"
#include<array>

class Fountain :public Collider {
public:
    Fountain();
    ~Fountain();
    void Initialize();
    void Update();
    void Draw();
    void OnCollision(Collider* collider) override;
private:
    void CreateParticle();
private:
    std::unique_ptr<Object3d> object_ = nullptr;
    std::unique_ptr<Object3d> waterObject_ = nullptr;
    std::array<std::unique_ptr<ParticleEmitter>,2>particleEmitter_;
    float splashTimer_ = 0.0f;

};

