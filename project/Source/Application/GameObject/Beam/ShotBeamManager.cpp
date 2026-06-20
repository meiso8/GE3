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

    DebugUI::CheckEmitter(chargeParticleEmitter_->GetEmitter());

#endif // !USE_IMGUI

    if (enemy_->GetPhase() != Enemy::BEAM) {
        currentTime_ = 0.0f;
        return;
    }

    if (enemy_->GetIsShotStart()) {

        currentTime_ -= Time::DeltaTime();

        if (currentTime_ <= 0.0f) {

            Vector3 target = raySprite_->ray_.origin;

            Matrix4x4* enemyEyeMatL = &enemy_->GetEyeMats().at("eye_L");
            Matrix4x4* enemyEyeMatR = &enemy_->GetEyeMats().at("eye_R");
            if (beamManager_->ShotBeam(target, enemyEyeMatL, Beam::kEnemy) && beamManager_->ShotBeam(target, enemyEyeMatR, Beam::kEnemy)) {
                Sound::PlaySE(SoundFactory::BEAM);
                Initialize();
            }

        } else {
            //ショットが開始されていないときはすぐに
           chargeParticleEmitter_->Update();
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
    AABB playerAABB = GetAABBWorldPos(player_->GetEyeCollider());
    float min = 0.0f;



    for (auto& beam : beamManager_->GetBeams()) {

        if (!beam->GetIsActive()) { continue; }

        Ray ray = beam->GetRay();
        float length = Length(ray.diff);

        //アイテムがあれば　跳ね返し攻撃が出来るように作成していく予定

        if (IntersectsAABB(ray, playerAABB, player_->GetEyeCollider()->GetWorldTransform().GetWorldPosition(), length)) {

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

    chargeParticleEmitter_ = std::make_unique<ParticleEmitter>();
    chargeParticleEmitter_->SetName("powerCharge");
    chargeParticleEmitter_->Initialize();

    auto& emitter2 = chargeParticleEmitter_->GetEmitter();
    emitter2.transform.Parent(enemy_->GetWorldTransform());
    emitter2.count = 10;
    emitter2.color = { 1.0f,0.5f,0.0f,1.0f };
    emitter2.frequencyTime = 0.2f;
    emitter2.lifeTime = 1.0f;
    emitter2.blendMode = kBlendModeScreen;
    emitter2.movement = ParticleMovements::kParticleSphere;
    emitter2.radius = 7.0f;
    emitter2.radiusSpeed = 0.5f;
    emitter2.polarSpeed = 3.0f;
    emitter2.transform.scale_ = { 0.25f,0.25f,0.25f };
    emitter2.transform.translate_.y = 1.35f;
    emitter2.translateAABB_ = { .min = {0.0f,-0.5f,0.0f},.max = {0.0f,0.5f,0.0f} };
    //emitter2.rotateAABB_ = { .min = {-pi ,-pi ,-pi } ,.max = { pi, pi, pi} };
    emitter2.velocityAABB = { .min = {0.0f,-1.0f,0.0f},.max = {0.0f,2.0f,0.0f} };
    auto& powerChargeGroup = ParticleManager::GetInstance()->GetParticleGroup(emitter2.name);
    powerChargeGroup->accelerationField.acceleration.y = -0.25f;
    powerChargeGroup->accelerationField.area = { .min = {-10.0f,-1.0f,-10.0f},.max = {10.0f,10.0f,10.0f} };
    powerChargeGroup->useBillboard = true;
}

