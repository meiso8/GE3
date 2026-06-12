#pragma once
#include"Medjed/Enemy.h"
#include "BulletManager.h"
#include "ShotBulletManager.h"
#include"RhythmManager.h"
#include<memory>
#include"../GameObject/Beam/BeamManager.h"
#include"../GameObject/Beam/ShotBeamManager.h"
#include"Player/RaySprite.h"

class Player;
class RhythmBullet
{
private:
    std::unique_ptr<RhythmManager>rhythmManager_ = nullptr;
    std::unique_ptr<BulletManager>bulletManager_ = nullptr;
    std::unique_ptr<ShotBulletManager>shotBulletManager_ = nullptr;

    std::unique_ptr<BeamManager>beamManager_ = nullptr;
    std::unique_ptr<ShotBeamManager>shotBeamManager_ = nullptr;
public:
    RhythmBullet(Enemy* enemy,Player* player, RaySprite* raySprite);
    void SetSound(const SoundFactory::TAG tag);
    void Initialize();
    void Update();
    void Draw(Camera& camera);

    BulletManager* GetBulletManager() { return bulletManager_.get(); };
    ShotBulletManager* GetShotBulletManager() { return shotBulletManager_.get();}
    ShotBeamManager* GetShotBeamManager() { return shotBeamManager_.get(); }


};

