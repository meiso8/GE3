#pragma once
#include"Bullet.h"
#include <vector>
class BulletManager
{
public:
	BulletManager();
	~BulletManager();
	void Initialize();
	void Update();
	void Draw();
	void ShotBullet(const Vector3& position, const Vector3& direction, const float speed, const float size, const Bullet::BulletType& type);
	const std::vector<std::unique_ptr<Bullet>>& GetBullets() const {
		return bullets_;
	};

private:
	std::vector<std::unique_ptr<Bullet>> bullets_;
}; 
