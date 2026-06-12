#include "ShotBeamManager.h"

#include"Sound.h"
#include"CollisionManager.h"
#include"InputBind.h"

#include"MakeMatrix.h"
#include"CoordinateTransform.h"
#include"TimeManager.h"
#include"DebugUI.h"

namespace {
    const float kInterval_ = 2.0f;
    float currentTime_ = kInterval_;
    float tMin_ = 0.0f;
    float tMax_ = 1.0f;
}

ShotBeamManager::ShotBeamManager(Enemy* enemy, Player* player, BeamManager* beamManager)
    :enemy_(enemy), player_(player), beamManager_(beamManager)
{

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

            Vector3 target = player_->GetEyePos();

            Matrix4x4* enemyEyeMat = &enemy_->GetColliderGroup().at("EnemyEye").matrix_;

            if (beamManager_->ShotBeam(target, enemyEyeMat, Beam::kEnemy)) {
                Initialize();
            }

        }

    }

}
bool IntersectsAABB(const Ray& ray, const AABB& aabb, const Vector3& pos, const float kMaxDistance)
{
    if (RayIntersectsAABB(ray, aabb, tMin_, tMax_)) {
        float dist = Distance(ray.origin, pos);
        if (dist <= kMaxDistance) {
            return true;
        }
    }
    return false;
}
void ShotBeamManager::RayCastHit(RaySprite& raySprite)
{
    AABB playerAABB = GetAABBWorldPos(player_);
    float min = 0.0f;



    for (auto& beam : beamManager_->GetBeams()) {

        if (!beam->GetIsActive()) { continue; }

        Ray ray = beam->GetRay();
        float length = Length(ray.diff);

        //アイテムがあれば　跳ね返し攻撃が出来るように作成していく予定

        if (IntersectsAABB(ray, playerAABB, player_->GetBodyWorldTransform().GetWorldPosition(), length)) {

            Sound::PlaySE(SoundFactory::FALL, 0.5f);

            if (InputBind::IsClick()) {

                if (beam->GetBeamType() != Beam::kPlayer) {
                    //rayのオリジンから　rayの方向にLength分shotする
                    Vector3 target = raySprite.ray_.origin + raySprite.ray_.diff * length;
                    //親なし
                    beam->Shot(target, Beam::kPlayer, raySprite.ray_.origin, nullptr);
                }

            } else {

                player_->OnCollisionEnemy();
            }

        }
    }
}

