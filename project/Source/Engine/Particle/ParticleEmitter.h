#pragma once
#include"WorldTransform.h"
#include<cstdint>
#include<string>
#include"Vector4.h"
#include"BlendMode.h"
#include"Particle.h"
#include"AccelerationField.h"

class Camera;

struct Emitter
{
    std::string name = "unknown";
    bool isLoop_ = false;
    bool useRadialEmission_ = false;
    bool useBillboard_ = true;

    WorldTransform transform;//エミッタのTransfrom
    AABB translateAABB_ = { 0.0f };
    AABB velocityAABB = { 0.0f };
    AABB rotateAABB_ = { 0.0f };
    AABB scaleAABB_ = { 0.0f };
    uint32_t count = 3;//発生数
    float frequency = 0.5f;//発生頻度
    float frequencyTime = 0.0f;//頻度用時刻

    Vector4 startColor = { 1.0f,1.0f,1.0f,1.0f };//色
    Vector4 endColor = { 1.0f,1.0f,1.0f,0.0f };

    BlendMode blendMode = BlendMode::kBlendModeAdd;//ブレンドモード
    ParticleMovements movement = ParticleMovements::kParticleNormal;//動き
    float lifeTime = -1.0f;//生存時間
    //球面座標の半径
    float radius = 1.0f;
    //半径の移動速度
    float radiusSpeed = 0.0f;
    MinMax radiusSpeedMinMax = { 0.0f,0.0f };
    //経度の移動速度
    float polarSpeed = 0.0f;
    MinMax polarSpeedMinMax = { 0.0f,0.0f };
    AccelerationField accelerationField_;

};

class ParticleManager;

class ParticleEmitter
{
private:
    Emitter emitter_{};
    static ParticleManager* particleManager_;
public:
    ParticleEmitter();
    ~ParticleEmitter() = default;
    void Initialize();
    void Update();
    void Emit();
    Emitter& GetEmitter() { return emitter_; };
    void SetName(const std::string name) { emitter_.name = name; }
    void SetMovement(ParticleMovements& movement) { emitter_.movement; }
    void SetParent(WorldTransform& parent);
    static void SetParticleManager(ParticleManager* particleManager);
private:
    void UpdateTimer();

};

