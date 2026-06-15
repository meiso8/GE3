#pragma once
#include"WorldTransform.h"
#include<cstdint>
#include<string>
#include"Vector4.h"
#include"BlendMode.h"
#include"Particle.h"

class Camera;

struct Emitter
{
    std::string name = "unknown";
    WorldTransform transform;//エミッタのTransfrom
    AABB translateAABB_ = { 0.0f };
    AABB velocityAABB = {0.0f};
    AABB rotateAABB_ = { 0.0f};
    AABB scaleAABB_ = { 0.0f};
    uint32_t count = 3;//発生数
    float frequency = 0.5f;//発生頻度
    float frequencyTime = 0.0f;//頻度用時刻
    Vector4 color = {1.0f,1.0f,1.0f,1.0f};//色
    float startAlpha_ = 1.0f;
    float endAlpha_ = 0.0f;
    BlendMode blendMode = BlendMode::kBlendModeAdd;//ブレンドモード
    ParticleMovements movement = ParticleMovements::kParticleNormal;//動き
    float lifeTime = -1.0f;//生存時間
    //球面座標の半径
    float radius = 1.0f;
    //半径の移動速度
    float radiusSpeed = 0.0f;
    MinMax radiusSpeedMinMax = {0.0f,0.0f};
    //経度の移動速度
    float polarSpeed = 0.0f;
    MinMax polarSpeedMinMax = { 0.0f,0.0f };

};

class ParticleEmitter
{
private:
   static ParticleManager* particleManager_;
   Emitter emitter_{};
public:
public:
    ParticleEmitter();
    ~ParticleEmitter() = default;
    void Initialize();
    void SetEmitterParam(
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
   const MinMax& polarSpeedMinMax);

    void UpdateTimer();
    void InitTimer();
    void SetTimer();
    void UpdateEmitter();
    void Emit();

    Emitter& GetEmitter() { return emitter_; };
    void SetName(const std::string name) { emitter_.name = name; }
    void SetMovement(ParticleMovements& movement) { emitter_.movement; }
    void SetParent(WorldTransform& parent);

    std::unique_ptr <ParticleGroup>& GetGroup();
    static void Create();
    static void Update(Camera& camera);
    static void Draw();
};

