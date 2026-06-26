#include "ShotBeamManager.h"

#include"Sound.h"
#include"CollisionManager.h"
#include"InputBind.h"

#include"MakeMatrix.h"
#include"TimeManager.h"
#include"DebugUI.h"


namespace {
    const float kInterval_ = 2.0f;
    float currentTime_ = kInterval_;
    float tMin_ = 0.0f;
    float tMax_ = 1.0f;
}

ShotBeamManager::ShotBeamManager(Enemy* enemy, Player* player, BeamManager* beamManager, RaySprite* raySprite)
    :enemy_(enemy), player_(player), beamManager_(beamManager), raySprite_(raySprite)
{
    CreateParticleEmitter();
}

void ShotBeamManager::Initialize()
{
    currentTime_ = kInterval_;
}

void ShotBeamManager::Update()
{

#ifdef USE_IMGUI
    DebugUI::CheckEmitter(shockEmitter_->GetEmitter(), "shockEmitter");
#endif // !USE_IMGUI
    auto& emitter0 = beamParticleEmitters_[0]->GetEmitter();
    auto& emitter1 = beamParticleEmitters_[1]->GetEmitter();
    auto& emitter3 = shockEmitter_->GetEmitter();
    if (enemy_->GetPhase() != Enemy::BEAM) {
        currentTime_ = 0.0f;

        emitter0.frequencyTime = 0.0f;
        emitter1.frequencyTime = 0.0f;
        emitter3.frequencyTime = 0.0f;
        return;
    }

    //shockEmitter_->Update();

    if (enemy_->GetIsShotStart()) {

        currentTime_ -= TimeManager::DeltaTime();

        Matrix4x4* enemyEyeMatL = &enemy_->GetEyeMats().at("eye_L");
        Matrix4x4* enemyEyeMatR = &enemy_->GetEyeMats().at("eye_R");
        emitter0.transform.eTransform_.translate = Math::GetWorldTransformByMatrix(*enemyEyeMatL);
        emitter1.transform.eTransform_.translate = Math::GetWorldTransformByMatrix(*enemyEyeMatR);

        for (auto& emitter : beamParticleEmitters_) {
            emitter->Update();
        }

        if (currentTime_ <= 0.0f) {

            Vector3 target = raySprite_->ray_.origin;

            if (beamManager_->ShotBeam(target, enemyEyeMatL, Beam::kEnemy) && beamManager_->ShotBeam(target, enemyEyeMatR, Beam::kEnemy)) {
                Sound::PlaySE(SoundFactory::BEAM);
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
void ShotBeamManager::RayCastHit()
{
    float min = 0.0f;

    //中心点を考慮した座標を取得してくる
    Vector3 pos = player_->GetEyeCollider()->GetWorldTransform().GetWorldPosition();

    AABB aabbWorld = { .min = {-0.25f,-0.25f,-0.25f},.max = {0.25f,0.25f,0.25f} };
    aabbWorld.min += pos;
    aabbWorld.max += pos;

    for (auto& beam : beamManager_->GetBeams()) {

        if (!beam->GetIsActive()) { continue; }

        Ray ray = beam->GetRay();
        float length = Length(ray.diff);

        //アイテムがあれば　跳ね返し攻撃が出来るように作成していく予定

        if (IntersectsAABB(
            ray,
            aabbWorld,
            player_->GetEyeCollider()->GetWorldTransform().GetWorldPosition(),
            length
        )) {

            raySprite_->OnCollisionColor();

            if (InputBind::IsClick()) {


                if (beam->GetBeamType() != Beam::kPlayer) {

                    Sound::PlaySE(SoundFactory::FALL, 0.5f);

                    //rayのオリジンから　rayの方向にLength分shotする
                    Vector3 target = raySprite_->ray_.origin + raySprite_->ray_.diff * length;
                    //親なし
                    beam->Shot(target, Beam::kPlayer, raySprite_->ray_.origin, nullptr);
                }

            } else {

                player_->OnCollisionEnemy();
            }

        }
    }
}

void ShotBeamManager::CreateParticleEmitter()
{

    for (auto& emitter : beamParticleEmitters_) {
        emitter = std::make_unique<ParticleEmitter>();
        emitter->Initialize();
        emitter->SetName("particle1");
    }

    auto& emitter0 = beamParticleEmitters_[0]->GetEmitter();

    emitter0.count = 8;
    emitter0.color = { 1.0f,1.0f,1.0f,1.0f };
    emitter0.transform.eTransform_.scale = { 0.025f,0.5f,0.5f };
    emitter0.transform.eTransform_.rotate = { 0.0f,0.0f,0.0f };
    emitter0.transform.eTransform_.translate = { 0.0f,0.0f,0.0f };

    emitter0.frequencyTime = 0.01f;
    emitter0.frequency = 0.1f;
    emitter0.lifeTime = 0.1f;
    emitter0.blendMode = kBlendModeAdd;
    emitter0.movement = ParticleMovements::kParticleNormal;
    emitter0.rotateAABB_ = { .min = {0.0f,0.0f,-3.14f},.max = {0.0f,0.0f,3.14f} };
    emitter0.scaleAABB_ = { .min = {0.0f,0.4f,0.0f},.max = {0.0f,1.5f,1.0f} };
    emitter0.isLoop_ = true;

    auto& emitter1 = beamParticleEmitters_[1]->GetEmitter();
    emitter1 = emitter0;

    emitter1.accelerationField_.area = { .min = {-1.0f,-1.0f,-1.0f},.max = {1.0f,1.0f,1.0f} };
    emitter1.useBillboard_ = true;

    shockEmitter_ = std::make_unique<ParticleEmitter>();
    shockEmitter_->SetName("shockParticle");
    shockEmitter_->Initialize();

    auto& emitter3 = shockEmitter_->GetEmitter();

    emitter3.isLoop_ = true;
    emitter3.useRadialEmission_ = true;
    emitter3.transform.eTransform_.scale = { 0.1f,0.2f,0.1f };
    emitter3.transform.eTransform_.translate = { 0.0f,0.2f,0.0f };
    emitter3.radius = 5.0f;
    emitter3.radiusSpeed = 0.2f;
    emitter3.count = 20;
    emitter3.lifeTime = 1.0f;
    emitter3.movement = ParticleMovements::kParticleSphere;
    emitter3.polarSpeed = 0.0f;
    emitter3.transform.Parent(enemy_->GetWorldTransform());
    emitter3.useBillboard_ = false;
}

