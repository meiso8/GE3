#include "ShotBeamManager.h"



#include"Sound.h"
#include"CollisionManager.h"
#include"InputBind.h"

#include"MakeMatrix.h"
#include"CoordinateTransform.h"
#include"TimeManager.h"

namespace {
    const float kInterval_ = 2.0f;
    float currentTime_ = kInterval_;
 
}

ShotBeamManager::ShotBeamManager(Enemy* enemy, BeamManager* beamManager)
    :enemy_(enemy), beamManager_(beamManager)
{
    beamManager_->SetParentMatrix(&enemy_->GetColliderGroup().at("EnemyEye").matrix_);
}

void ShotBeamManager::Initialize()
{
    currentTime_ = kInterval_;
}

void ShotBeamManager::Update()
{
    if (enemy_->GetPhase() != Enemy::BEAM) {
        Initialize();
        return;
    }

   
    if (enemy_->GetIsShotStart()) {

        currentTime_ -= Time::DeltaTime();

        if (currentTime_ <= 0.0f) {
            
            Vector3* target = enemy_->GetTarget();
            if (beamManager_->ShotBeam(*target)) {
                Initialize();
            }
 
        }  
    }
}

void ShotBeamManager::RayCastHit()
{
    //for (auto& bullet : beamManager_->GetBeams()) {
    //    if (!bullet->GetIsActive()) { continue; }

    //    AABB aabb = GetAABBWorldPos(bullet.get());

    //    if (raySprite.IntersectsAABB(aabb, bullet->GetWorldPosition())) {
    //        bullet->SetColor({ 1.0f,0.0f,0.0f,1.0f });

    //        if (InputBind::IsClick()) {
    //            if (bullet->type_ != Bullet::kPlayer) {
    //                Sound::PlaySE(SoundFactory::CRACKER, 0.5f);
    //                Vector3 shotDirection = raySprite.ray_.diff;
    //                Vector3 shotPosition = bullet->GetWorldPosition();
    //                bullet->Shot(shotPosition, shotDirection, shotSpeed_, shotSize_, Bullet::kPlayer);
    //            }
    //        }
    //    } else {
    //        bullet->SetColor({ 1.0f,1.0f,1.0f,1.0f });
    //    }
    //}
}
