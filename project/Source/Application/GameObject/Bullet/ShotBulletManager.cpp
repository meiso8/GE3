#include "ShotBulletManager.h"

#include "Medjed/Enemy.h"
#include "BulletManager.h"

#include"Sound.h"
#include"CollisionManager.h"
#include"InputBind.h"

#include"MakeMatrix.h"
#include"CoordinateTransform.h"
#include"TimeManager.h"

ShotBulletManager::ShotBulletManager(Enemy* enemy, BulletManager* bulletManager, RhythmManager* rhythmManager)
    :enemy_(enemy), bulletManager_(bulletManager), rhythmManager_(rhythmManager)
{
}

void ShotBulletManager::Initialize() {
    currentTime_ = 0.0f;
}

// Update内で時間を進める
void ShotBulletManager::Update() {

    currentTime_ += Time::DeltaTime();

    if (currentTime_ >= rhythmManager_->kEndSoundTime_) {
        Initialize();
    }

    if (enemy_->GetPhase() != Enemy::FIREBALL) {
        //ファイヤーボールの時にショットする
        return;
    }

    if (enemy_->GetIsShotStart()) {

        if (rhythmManager_->IsOnBeat(currentTime_)) {

            Vector3 toTarget = enemy_->GetToTarget();
            Sound::PlaySE(SoundFactory::FIRE_BALL);

            if (rand() % 2 == 0) {

                const int bulletCount = 5;
                const float spreadAngle = std::numbers::pi_v<float> / 6.0f; // ±30度の範囲

                int select = rand() % bulletCount;

                for (int i = 0; i < bulletCount; ++i) {
                    float angleOffset = spreadAngle * ((float)i / (bulletCount - 1) - 0.5f); // -0.5〜+0.5
                    Matrix4x4 rotY = MakeRotateYMatrix(angleOffset);
                    Vector3 shotDirection = CoordinateTransform(toTarget, rotY);
                    shotDirection = Normalize(shotDirection);

                    Vector3 shotPosition = enemy_->GetWorldTransform().GetWorldPosition() + shotDirection * 0.5f;
                    shotPosition.y += 0.5f;
                    bulletManager_->ShotBullet(shotPosition, shotDirection, shotSpeed_, shotSize_, select == i ? Bullet::kEnemyHot : Bullet::kEnemyCold);
                }
            } else {

                Vector3 shotPosition = enemy_->GetWorldTransform().GetWorldPosition() + toTarget * 0.5f;
                shotPosition.y += 0.5f;
                bulletManager_->ShotBullet(shotPosition, toTarget, shotSpeed_, shotSize_, rand() % 2 == 0 ? Bullet::kEnemyHot : Bullet::kEnemyCold);
            }


        }
    }
}

void ShotBulletManager::RayCastHit(RaySprite& raySprite)
{

    for (auto& bullet : bulletManager_->GetBullets()) {

        if (!bullet->isActive_ && bullet->GetType() != Bullet::kPlayerCold && bullet->GetType() != Bullet::kPlayerHot) { continue; }

        AABB aabb = GetAABBWorldPos(bullet.get());

        if (raySprite.IntersectsAABB(aabb, bullet->GetWorldPosition())) {

            bullet->SetColor({ 1.0f,0.5f,0.5f,1.0f });

            if (InputBind::IsClick()) {

                Sound::PlaySE(SoundFactory::CRACKER, 0.5f);
                Vector3 shotDirection = raySprite.ray_.diff;
                Vector3 shotPosition = bullet->GetWorldPosition();
                bullet->Shot(shotPosition, shotDirection, shotSpeed_, shotSize_, Bullet::kEnemyCold ? Bullet::kPlayerCold : Bullet::kPlayerHot);

            }

        } else {
            bullet->SetColor({ 1.0f,1.0f,1.0f,1.0f });
        }
    }
}
