#pragma once

#include "Medjed/Enemy.h"
#include "BeamManager.h"
#include"Player/RaySprite.h"
#include"Player/Player.h"
#include<memory>

#include "ParticleEmitter.h"

class ShotBeamManager
{
public:
	ShotBeamManager(Enemy* enemy, Player* player,BeamManager* beamManager,RaySprite* raySprite);
	void Initialize();
	void Update();
	void RayCastHit();
private:
	void CreateParticleEmitter();
private:
	RaySprite* raySprite_ = nullptr;
	Player* player_ = nullptr;
	Enemy* enemy_ = nullptr;
	BeamManager* beamManager_ = nullptr;
	std::array<std::unique_ptr < ParticleEmitter>,2> beamParticleEmitters_;


	std::unique_ptr < ParticleEmitter> shockEmitter_;
};

