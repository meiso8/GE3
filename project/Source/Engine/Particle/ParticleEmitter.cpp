#include "ParticleEmitter.h"
#include"TimeManager.h"

ParticleManager* ParticleEmitter::particleManager_ = nullptr;

ParticleEmitter::ParticleEmitter()
{
    //Initialize();

}

void ParticleEmitter::SetParticleManager(ParticleManager* particleManager)
{
    particleManager_ = particleManager;
    assert(particleManager_);
}

void ParticleEmitter::Initialize()
{
    emitter_.isLoop_ = false;
    emitter_.useRadialEmission_ = false;
    emitter_.useBillboard_ = true;
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

    emitter_.startColor = { 1.0f,1.0f,1.0f,1.0f };
    emitter_.endColor = { 1.0f,1.0f,1.0f,0.0f };

    emitter_.blendMode = kBlendModeAdd;
    emitter_.movement = ParticleMovements::kParticleNormal;
    emitter_.lifeTime = -1.0f;
    emitter_.radius = 5.0f;
    const float deltaTime = TimeManager::DeltaTime();
    emitter_.radiusSpeed = deltaTime;
    emitter_.polarSpeed = deltaTime;
    emitter_.polarSpeedMinMax = { 0.0f,0.0f };
    emitter_.radiusSpeedMinMax = { 0.0f,0.0f };

    emitter_.accelerationField_.acceleration = { 0.0f,0.0f,0.0f };
    emitter_.accelerationField_.area.min = { -1.0f,-1.0f,-1.0f };
    emitter_.accelerationField_.area.max = { 1.0f,1.0f,1.0f };
}

void ParticleEmitter::UpdateTimer()
{
    if (!emitter_.isLoop_) {
        emitter_.frequencyTime = 0.0f;
        return;
    }

    emitter_.frequencyTime += TimeManager::DeltaTime();

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
    particleManager_->Emit(emitter_);
}

void ParticleEmitter::SetParent(WorldTransform& parent)
{
    emitter_.transform.Parent(parent);

}
