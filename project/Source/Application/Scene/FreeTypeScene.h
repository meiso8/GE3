#pragma once
#include "SceneManager.h"
#include"../Engine/FreeTypeManager/Text.h"
#include<string>
#include"Object3d/Object3d.h"
#include"Mesh/Skybox/Skybox.h"
#include"Particle/ParticleEmitter.h"
#include"../GameObject/Player/Player.h"

class FreeTypeScene : public BaseScene
{
public:
    FreeTypeScene();
    void Initialize()override;
    void Update()override;
    void DrawModel()override;
    void DrawSprite()override;
private:
    void CreateParticle();
private:
    std::u32string inputText_;
    Text text_;
    uint32_t handle_;
    Text pressSpaceText_;
    std::unique_ptr<Sprite>sprite_ = nullptr;
    std::unique_ptr<SkyboxObject3d> skyBoxObj_ = nullptr;
    std::unique_ptr<Object3d> object3d_ = nullptr;
    std::unique_ptr<Object3d> object3d2_ = nullptr;

    std::unique_ptr<Primitive> cubeMesh_ = nullptr;
    std::unique_ptr<Primitive>ringMesh_ = nullptr;
    std::array<std::unique_ptr<ParticleEmitter>, 1>particleEmitters_;

   /* std::unique_ptr<Player>player_ = nullptr;*/
    std::vector<std::unique_ptr<Object3d>> enemies_;
    int srvIndex = 0;


};

