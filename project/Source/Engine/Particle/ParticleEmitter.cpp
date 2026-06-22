#include "ParticleEmitter.h"
#include"Input.h"
#include"Camera.h"
#include"TimeManager.h"

ParticleEmitter::ParticleEmitter()
{
    //Initialize();

}
void ParticleEmitter::Initialize()
{
    emitter_.isLoop_ = false;
    emitter_.useRadialEmission_ = false;
    emitter_.count = 3;
    emitter_.frequency = 0.5f;
    emitter_.frequencyTime = 0.0f;
    emitter_.transform.eTransform_.rotate = { 0.0f,0.0f,0.0f };
    emitter_.transform.eTransform_.scale = { 1.0f,1.0f,1.0f };
    emitter_.transform.eTransform_.translate = { 0.0f,0.0f,0.0f };

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

void ParticleEmitter::UpdateTimer()
{
    if (!emitter_.isLoop_) {
        emitter_.frequencyTime = 0.0f;
        return;
    }

    emitter_.frequencyTime += Time::DeltaTime();

    if (emitter_.frequency <= emitter_.frequencyTime) {
        emitter_.frequencyTime = 0.0f;
        Emit();
    }
}

void ParticleEmitter::Update()
{
    UpdateTimer();
    WorldTransformUpdate(emitter_.transform);
}

void ParticleEmitter::Emit()
{
    ParticleManager::GetInstance()->Emit(emitter_);
}

std::unique_ptr <ParticleGroup>& ParticleEmitter::GetGroup()
{
    return ParticleManager::GetInstance()->GetParticleGroup(emitter_.name);
}

void ParticleEmitter::SetParent(WorldTransform& parent)
{
    emitter_.transform.Parent(parent);

}
