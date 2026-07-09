#include "BeamManager.h"
namespace {
	const int kMaxBeams = 10;
}

BeamManager::BeamManager()
{
	for (int i = 0; i < kMaxBeams; i++) {
		beams_.emplace_back(std::make_unique<Beam>());
		beams_[i]->Initialize();
	}
}

BeamManager::~BeamManager()
{
	beams_.clear();
}

void BeamManager::Initialize()
{
	for (auto& beam : beams_) {
		beam->Initialize();
	}
}


void BeamManager::Update()
{
	for (auto& beam : beams_) {
		beam->Update();
	}
}

void BeamManager::Draw(Camera* camera)
{
	for (auto& beam : beams_) {
		beam->Draw(camera);
	}
}

bool BeamManager::ShotBeam(const Vector3& target,Matrix4x4* parent,const Beam::BeamType  beamType)
{
	bool isShot = false;

	for (auto& bullet : beams_) {
		if (!bullet->GetIsActive()) {
			//始点なし　親を設定する
			isShot = bullet->Shot(target, beamType,{0.0f,0.0f,0.0f},parent);
			break;
		}
	}

	return isShot;
}

