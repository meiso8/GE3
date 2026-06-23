#pragma once
#include "ModelData.h"
#include"commandList.h"
#include"WorldTransform.h"
#include"MaterialResource.h"
#include"Transform.h"
#include"RootSignature.h"
#include "BlendState.h"
#include"AccelerationField.h"
#include"SphericalCoordinate.h"
#include"Texture.h"
#include"ModelManager.h"
#include"Random.h"
#include<unordered_map>
#include<list>
#include<memory>
#include<cstdint>
#include<memory>
#include"Primitive.h"

class Camera;
class ShaderResourceView;
struct Emitter;

struct Particle {
    EulerTransform transform;
    Vector3 velocity;
    Vector4 color;

    float lifeTime;
    float currentTime;
};

struct MinMax {
    float min;
    float max;
};

struct SphericalMove {
    SphericalCoordinate coordinate;
    float radiusSpeed;
    float polarSpeed;
};


struct ParticleForGPU
{
    float32_t4x4 WVP;
    float32_t4x4 World;
    float4 color;
};

enum ParticleMovements {
    kParticleNormal,
    kParticleSphere,
    kParticleShock
};

struct ParticleGroup {
    MaterialData materialData;
    std::unique_ptr < MaterialResource> materialResource;
    std::list<Particle>particles;
    std::list<SphericalMove>sphericalCoordinates;
    uint32_t instanceSrvIndex;
    Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
    uint32_t numInstance;//インスタンス数
    ParticleForGPU* instancingData;
    Vector3 textureSize;
    bool useModel = false;
    bool useBillboard = true;
    bool useSpriteCamera = false;
    bool useRadialEmission = false;
    Model* model = nullptr;
    std::unique_ptr<Primitive> primitive = nullptr;
    const WorldTransform* parentPos_ = nullptr;
    ParticleMovements movement = kParticleNormal;
    AccelerationField accelerationField;
    BlendMode blendMode = BlendMode::kBlendModeAdd;
    float startAlpha_ = 1.0f;
    float endAlpha_ = 0.0f;
};

std::list<SphericalMove> EmitCoordinate(uint32_t count, const float& radius, const float& radiusSpeed, const float& polarSpeed, const MinMax& polarSpeedMinMax, const MinMax& radiusSpeedMinMax);

std::list<Particle> EmitParticles(
    const AABB& velocityAABB, 
    const WorldTransform& transform, 
    const bool useRadialEmission,
    uint32_t count,
    const Vector4& color, 
    const float& lifeTime, 
    const AABB& translateAABB,
    const AABB& rotateAABB,
    const AABB& scaleAABB
);
Particle MakeNewParticle(
    const AABB& velocityAABB,
    const WorldTransform& transform, const bool useRadialEmission, const int& count, const int& maxCount,
    const Vector4& color, const float& lifeTime, const AABB& translateAABB, const AABB& rotateAABB, const AABB& scaleAABB);

SphericalMove MakeNewSphericalCoordinate(const float& radius, const int& count, const int& maxCount, const float& radiusSpeed, const float& polarSpeed, const MinMax& polarSpeedMinMax, const MinMax& radiusSpeedMinMax);

class ParticleManager
{
public:
    static const uint32_t kNumMaxInstance = 1000;//インスタンス数


private:

    RootSignature* rootSignature_ = nullptr;
    static ID3D12GraphicsCommandList* commandList_;
    static std::unordered_map<std::string, std::unique_ptr <ParticleGroup>>particleGroups;

    Matrix4x4 backToFrontMatrix;
    Matrix4x4 billboardMatrix;
    Matrix4x4 worldMatrix;
    Matrix4x4 worldViewProjectionMatrix;

    Camera* camera_ = nullptr;

private:
    //コンストラク・タデストラクタの隠ぺい
    ParticleManager() = default;
    ~ParticleManager() = default;
public:

    void CreateAll();
    //コピーコンストラクタの封印
    ParticleManager(ParticleManager&) = delete;
    //コピー代入演算子の封印
    ParticleManager& operator=(ParticleManager&) = delete;
    void Create();
    static ParticleManager* GetInstance() {
        static ParticleManager instance;
        return &instance;
    };
    static void Reset(const std::string& name);
    static void ResetAll();
    static void Emit(Emitter& emitter);

    std::unordered_map<std::string, std::unique_ptr <ParticleGroup>>& GetParticleGroups();
    std::unique_ptr <ParticleGroup>& GetParticleGroup(const std::string& name) {
        assert(particleGroups.contains(name));
        return particleGroups[name];
    };
    void CreateParticleGroup(const std::string name, const TextureFactory::Handle& textureHandle,const Primitive::TopologyType& topoligyType, const bool& useModel = false, const float temperature = 1.0f, const std::string& modelFileName = "Box.obj");

    void Update(Camera& camera);
    void Draw();
    void InitAccelerationField(ParticleGroup& group);
    void Finalize();

protected:
    void UpdateBillBordMatrix(Camera& camera, ParticleGroup& group);
    Matrix4x4 UpdateMatrix(Particle& particleItr, ParticleGroup& group);
private:
    //メンバ関数ポインタテーブル
    std::unordered_map<ParticleMovements, std::function<void(ParticleGroup&)>> UpdateFunctions;
    void Normal(ParticleGroup& group);
    void Sphere(ParticleGroup& group);
    void Shock(ParticleGroup& group);

    void IsCollisionFieldArea(Particle& particleItr, ParticleGroup& group);
    Matrix4x4 UpdateWorldMatrixForBillBord(Particle& particleItr, ParticleGroup& group);
    Matrix4x4 UpdateWorldMatrix(Particle& particleItr, ParticleGroup& group);
    Matrix4x4 UpdateSphereMatrix(Particle& particleItr, SphericalMove& sphericalMove, ParticleGroup& group);
    void UpdateWVPMatrix(Camera& camera, ParticleGroup& group);
    void UpdateInstancingData(ParticleGroup& group, Particle& particleItr);

};

