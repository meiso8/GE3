#pragma once

#include"Player/RaySprite.h"
#include"RhythmManager.h"

class Enemy;
class BulletManager;

class ShotBulletManager
{
public:
	ShotBulletManager(Enemy* enemy, BulletManager* bulletManager, RhythmManager* rhythmManager,RaySprite* raySprite);
	void Initialize();
	void Update();
	void RayCastHit();

private:
	Enemy* enemy_ = nullptr;
	BulletManager* bulletManager_ = nullptr;
	RhythmManager* rhythmManager_ = nullptr;
	RaySprite* raySprite_ = nullptr;

	float shotSpeed_ = 0.3f;
	float shotSize_ = 1.5f;
	float currentTime_ = 0.0f;

};
