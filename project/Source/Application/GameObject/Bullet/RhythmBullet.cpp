#include "RhythmBullet.h"


RhythmBullet::RhythmBullet()
{
    rhythmManager_ = std::make_unique<RhythmManager>();
    bulletManager_ = std::make_unique<BulletManager>();

    beamManager_ = std::make_unique<BeamManager>();

}

void RhythmBullet::SetSound(const SoundFactory::TAG tag)
{
    rhythmManager_->SetSound(tag);
}

void RhythmBullet::Initialize()
{

    rhythmManager_->Initialize();
    bulletManager_->Initialize();
    shotBulletManager_->Initialize();

    beamManager_->Initialize();
    shotBeamManager_->Initialize();

}

void RhythmBullet::Update()
{

    rhythmManager_->Update();
    //弾
    bulletManager_->Update();
    shotBulletManager_->Update();
    //ビーム
    beamManager_->Update();
    shotBeamManager_->Update();
}

void RhythmBullet::Draw(Camera& camera)
{
    bulletManager_->Draw(camera);
    beamManager_->Draw(&camera);
}

void RhythmBullet::SetEnemy(Enemy* enemy)
{
    shotBulletManager_ = std::make_unique<ShotBulletManager>(enemy, bulletManager_.get(), rhythmManager_.get());
    shotBeamManager_ = std::make_unique<ShotBeamManager>(enemy, beamManager_.get());
}

