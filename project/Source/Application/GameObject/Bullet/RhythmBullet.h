#pragma once
#include"Medjed/Enemy.h"
#include "BulletManager.h"
#include "ShotBulletManager.h"
#include"RhythmManager.h"
#include<memory>

class RhythmBullet
{
private:
    std::unique_ptr<RhythmManager>rhythmManager_ = nullptr;
    std::unique_ptr<BulletManager>bulletManager_ = nullptr;
    std::unique_ptr<ShotBulletManager>shotBulletManager_ = nullptr;
public:
    RhythmBullet();
    void SetSound(const SoundFactory::TAG tag);
    void Initialize();
    void Update();
    void Draw(Camera& camera);
    void SetEnemy(Enemy* enemy);
    BulletManager* GetBulletManager() { return bulletManager_.get(); };
    ShotBulletManager* GetShotBulletManager() { return shotBulletManager_.get();}
};

