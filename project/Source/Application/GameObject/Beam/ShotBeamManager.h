#pragma once

#include "Medjed/Enemy.h"
#include "BeamManager.h"
#include"Player/RaySprite.h"
#include"Player/Player.h"

class ShotBeamManager
{
public:

	ShotBeamManager(Enemy* enemy, Player* player,BeamManager* beamManager,RaySprite* raySprite);
	void Initialize();
	void Update();
	void RayCastHit();
private:
	RaySprite* raySprite_ = nullptr;
	Player* player_ = nullptr;
	Enemy* enemy_ = nullptr;
	BeamManager* beamManager_ = nullptr;
};

