#include "BeamManager.h"
namespace {
	const int kMaxBeams = 5;
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

bool BeamManager::ShotBeam(const Vector3& target)
{
	bool isShot = false;

	for (auto& bullet : beams_) {
		if (!bullet->GetIsActive()) {
			isShot = bullet->Shot(target);
			break;
		}
	}

	return isShot;
}

void BeamManager::SetParentMatrix(Matrix4x4* parent)
{
	for (auto& beam : beams_) {
		beam->SetParent(parent);
	}
}
