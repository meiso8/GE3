#include "ParticleEmitter.h"
#include"Input.h"
#include"Camera.h"
#include"TimeManager.h"
 ParticleManager* ParticleEmitter::particleManager_ = nullptr;

ParticleEmitter::ParticleEmitter()
{
    Initialize();

}
void ParticleEmitter::Initialize()
{
    emitter_.count = 3;
    emitter_.frequency = 0.5f;
    emitter_.frequencyTime = 0.0f;
    emitter_.transform.rotate_ = { 0.0f,0.0f,0.0f };
    emitter_.transform.scale_ = { 1.0f,1.0f,1.0f };
    emitter_.transform.translate_ = { 0.0f,0.0f,0.0f };

    emitter_.translateAABB_ = { 0.0f };
    emitter_.velocityAABB = { 0.0f};
    emitter_.rotateAABB_ = { 0.0f };
    emitter_.scaleAABB_ = { 0.0f };

    emitter_.color = { 1.0f,1.0f,1.0f,1.0f };
    emitter_.startAlpha_ = 1.0f;
    emitter_.endAlpha_ = 0.0f;

    emitter_.blendMode = kBlendModeAdd;
    emitter_.movement = ParticleMovements::kParticleNormal;
    emitter_.lifeTime = -1.0f;
    emitter_.radius = 5.0f;
    const float deltaTime = Time::DeltaTime();
    emitter_.radiusSpeed = deltaTime;
    emitter_.polarSpeed = deltaTime;
    emitter_.polarSpeedMinMax = { 0.0f,0.0f };
    emitter_.radiusSpeedMinMax = { 0.0f,0.0f };

}
void ParticleEmitter::SetEmitterParam(
    const Vector3& translate,
    const Vector3& scale,
    const Vector3& rotate,
    const  AABB& translateAABB,
    const AABB& velocityAABB,
    const AABB& rotateAABB,
    const AABB& scaleAABB,
    const uint32_t& count,
    const float& frequency,
    const float& frequencyTime,
    const Vector4& color,
    const BlendMode& blendMode,
    const ParticleMovements& movement,
    const float& lifeTime,
    const float& radius,
    const float& radiusSpeed,
    const MinMax& radiusSpeedMinMax,
    const float& polarSpeed,
    const MinMax& polarSpeedMinMax)
{
    //ペアレントなし
    emitter_.transform.scale_ = scale;//エミッタのTransfrom
    emitter_.transform.translate_ = translate;//エミッタのTransfrom
    emitter_.transform.rotate_ = rotate;//エミッタのTransfrom

    emitter_.translateAABB_ = translateAABB;
    emitter_.velocityAABB = velocityAABB;
    emitter_. rotateAABB_ = rotateAABB;
    emitter_. scaleAABB_ = scaleAABB;
    emitter_.count = count;//発生数
    emitter_. frequency = frequency;//発生頻度
    emitter_. frequencyTime = frequencyTime;//頻度用時刻
    emitter_.color = color;//色
    emitter_.blendMode = blendMode;//ブレンドモード
    emitter_.movement = movement;//動き
    emitter_. lifeTime = lifeTime;//生存時間
    //球面座標の半径
    emitter_. radius = radius;
    //半径の移動速度
    emitter_. radiusSpeed = radiusSpeed;
    emitter_.radiusSpeedMinMax = radiusSpeedMinMax;
    //経度の移動速度
    emitter_. polarSpeed = polarSpeed;
    emitter_.polarSpeedMinMax = polarSpeedMinMax;

}
void ParticleEmitter::UpdateTimer()
{
    emitter_.frequencyTime += Time::DeltaTime();

    if (emitter_.frequency <= emitter_.frequencyTime) {
        emitter_.frequencyTime -= emitter_.frequency;
        Emit();
    }
}
void ParticleEmitter::InitTimer()
{
    emitter_.frequencyTime = 0.0f;
}

void ParticleEmitter::SetTimer()
{
    emitter_.frequencyTime = emitter_.frequency;
}
void ParticleEmitter::UpdateEmitter()
{
    WorldTransformUpdate(emitter_.transform);
}

std::unique_ptr <ParticleGroup>& ParticleEmitter::GetGroup()
{
    return particleManager_->GetParticleGroup(emitter_.name);
}

void ParticleEmitter::Create()
{
    particleManager_ = ParticleManager::GetInstance();
    particleManager_->Create();
}

void ParticleEmitter::Update(Camera& camera)
{
    particleManager_->Update(camera);
}

void ParticleEmitter::Emit()
{
    particleManager_->Emit(emitter_);
}

void ParticleEmitter::Draw()
{
    particleManager_->Draw();
}

void ParticleEmitter::SetParent(WorldTransform& parent)
{
    emitter_.transform.Parent(parent);

}
