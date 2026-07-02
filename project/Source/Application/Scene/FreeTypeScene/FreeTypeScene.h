#pragma once
#include"../Engine/FreeTypeManager/Text.h"
#include<string>
#include"Object3d/Object3d.h"
#include"SkyboxObject3d.h"
#include"Particle/ParticleEmitter.h"
#include"../GameObject/Beam/Beam.h"
#include"BaseScene.h"
#include"LevelEditor/LevelEditor.h"
#include <vector>
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
    std::unique_ptr<Beam>beam_ = nullptr;
    std::vector<std::unique_ptr<LevelEditor::ObjectSet>> objects_;

    std::array<std::unique_ptr<ParticleEmitter>, 2>particleEmitters_;
};

