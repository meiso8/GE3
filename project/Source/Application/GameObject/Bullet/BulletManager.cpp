#include "BulletManager.h"

namespace {
	const int kMaxBullets = 16;
}

BulletManager::BulletManager() {
	for (int i = 0; i < kMaxBullets; i++) {
		bullets_.emplace_back(std::make_unique<Bullet>());
		bullets_[i]->Initialize();
	}
}

BulletManager::~BulletManager() {

}

void BulletManager::Initialize() {
	for (auto& bullet : bullets_) {
		bullet->Initialize();
	}
}

void BulletManager::Update() {
	
	for (auto& bullet : bullets_) {
		bullet->Update();
	}
}

void BulletManager::Draw() {
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}
}


void BulletManager::ShotBullet(const Vector3& position, const Vector3& direction, const float speed, const float size,const Bullet::BulletType& type) {
	for (auto& bullet : bullets_) {
		if (!bullet->GetIsActive()) {
			bullet->Shot(position, direction, speed, size,type);
			break;
		}
	}
}
