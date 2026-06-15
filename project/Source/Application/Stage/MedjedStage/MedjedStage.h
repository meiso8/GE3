#pragma once
#include"../Stage/Stage.h"
#include"Medjed/MedjedManager.h"
#include"Bullet/RhythmBullet.h"
#include"Player/Player.h"
#include"CollisionManager.h"
#include"../GameObject/BackGround/BackGround.h"
#include"Memo/MemoManager.h"
#include<memory>
class ParticleEmitter;

class MedjedStage :public Stage
{
private:

    enum Particels {
        kSky_Particle,
        kMedjed_Particle,
        kMaxParticles,
    };

    std::unique_ptr<MedjedManager>medjedManager_ = nullptr;
    std::unique_ptr<RhythmBullet>rhythmBullet_ = nullptr;
    std::unique_ptr<BackGround>backGround_ = nullptr;
    std::array<std::unique_ptr<ParticleEmitter>, kMaxParticles>particleEmitters_;


public:
    const bool IsClear();
    MedjedStage();
    void Initialize()override;
    void Update()override;
    void Draw(Camera& camera)override;
    void DrawSprite()override;
    void CheckCollision(CollisionManager& collisionManager)override;
  
    const bool& FindMedjed() { return medjedManager_->GetIsFindMedjed(); }
    Medjed* GetMedjed() { return medjedManager_->GetMedjed(); };
    Enemy* GetEnemy() { return medjedManager_->GetEnemy(); };
    const bool& GetEnemyApper() {return medjedManager_->GetIsApperMedjed(); }
    void UpdateEmitter(const Particels& particles);
private:
    void CreateParticle();
    void TransitionScene();
};

