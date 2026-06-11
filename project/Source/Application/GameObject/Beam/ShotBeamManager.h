#pragma once

#include "Medjed/Enemy.h"
#include "BeamManager.h"

class ShotBeamManager
{
public:

	ShotBeamManager(Enemy* enemy, BeamManager* beamManager);

	void Initialize();
	void Update();
	void RayCastHit();
private:

	Enemy* enemy_ = nullptr;
	BeamManager* beamManager_ = nullptr;
};

