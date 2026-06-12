#pragma once

#include "Medjed/Enemy.h"
#include "BeamManager.h"
#include"Player/RaySprite.h"
#include"Player/Player.h"

class ShotBeamManager
{
public:

	ShotBeamManager(Enemy* enemy, Player* player,BeamManager* beamManager);
	void Initialize();
	void Update();
	void RayCastHit(RaySprite& raySprite);
private:
	Player* player_ = nullptr;
	Enemy* enemy_ = nullptr;
	BeamManager* beamManager_ = nullptr;
};

