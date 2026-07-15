#include "Particle.h"
#include"DirectXCommon.h"
#include"Camera.h"
#include"MakeMatrix.h"
#include"PSO.h"
#include"PrimitiveFactory/PrimitiveFactory.h"
#include"Log.h"

#include"Collision.h"
#include"CbvSrvUavDescriptorHeap.h"
#include"Model.h"
#include"SpriteCamera.h"
#include"ParticleEmitter.h"
#include"TimeManager.h"
#include"Lerp.h"
#include"Object3d.h"
#include"CommandList.h"

using namespace  Microsoft::WRL;
namespace {
    const float halfPi = Math::kPi * 0.5f;
}

std::unordered_map<std::string, std::unique_ptr <ParticleGroup> >ParticleManager::particleGroups;

// ==========================================================================================================

void ParticleManager::CreateAll()
{
    CreateParticleGroup("particle1", TextureFactory::CIRCLE, Primitive::kPlane, false);
    CreateParticleGroup("people", TextureFactory::UV_CHECKER, Primitive::kPlane, true,1.0f, "people.obj");
    CreateParticleGroup("ring", TextureFactory::GRADATION_LINE, Primitive::kRing);
    CreateParticleGroup("medjedParticle", TextureFactory::UV_CHECKER, Primitive::kPlane, true,1.0f, "people.obj");
    CreateParticleGroup("powerCharge", TextureFactory::CIRCLE, Primitive::kPlane, false);
    CreateParticleGroup("fountain", TextureFactory::WATER_TEXTURE, Primitive::kPlane,false,0.0f);
    CreateParticleGroup("fountain2", TextureFactory::WATER_TEXTURE, Primitive::kPlane, false,0.0f);
    CreateParticleGroup("shockParticle", TextureFactory::CIRCLE, Primitive::kPlane, false);

    CreateParticleGroup("fireBase", TextureFactory::FIRE, Primitive::kPlane, false);
    CreateParticleGroup("fireLight", TextureFactory::FIRE, Primitive::kPlane, false);
    CreateParticleGroup("firePoint", TextureFactory::CIRCLE, Primitive::kPlane, false);

    CreateParticleGroup("footPrints", TextureFactory::FOOTS_PRINTS, Primitive::kPlane, false,0.5f);
}

// ==========================================================================================================


void ParticleManager::Create(RootSignature* rootSignature)
{
    rootSignature_ = rootSignature;
    assert(rootSignature_);

    UpdateFunctions = {
        {kParticleNormal, [this](ParticleGroup& group) { Normal(group); }},
        {kParticleSphere, [this](ParticleGroup& group) { Sphere(group); }},
        {kParticleShock, [this](ParticleGroup& group) { Shock(group); }},
        {kParticleWave, [this](ParticleGroup& group) { Wave(group); }},
    };
}

void ParticleManager::SetCommandListAndSrvDescriptorHeap(ID3D12GraphicsCommandList* commandList, CbvSrvUavDescriptorHeap* srvDescriptorHeap)
{
    commandList_ = commandList;
    assert(commandList_);

    srvDescriptorHeap_ = srvDescriptorHeap;
    assert(srvDescriptorHeap_);
}

Particle MakeNewParticle(
    const AABB& velocityAABB,
    const WorldTransform& transform, const bool useRadialEmission, const int& count, const int& maxCount,
    const Vector4& color, const float& lifeTime, const AABB& translateAABB, const AABB& rotateAABB, const AABB& scaleAABB)
{

    Particle particle;
    Random random(0.0f, 1.0f);
    random.SetMinMax(0.0f, 1.0f);
    particle.lifeTime = (lifeTime < 0.0f) ? random.Get() : lifeTime;

    random.SetMinMax(velocityAABB.min.x, velocityAABB.max.x);
    particle.velocity.x = random.Get();
    random.SetMinMax(velocityAABB.min.y, velocityAABB.max.y);
    particle.velocity.y = random.Get();
    random.SetMinMax(velocityAABB.min.z, velocityAABB.max.z);
    particle.velocity.z = random.Get();

    random.SetMinMax(scaleAABB.min.x, scaleAABB.max.x);
    float scaleX = random.Get();
    random.SetMinMax(scaleAABB.min.y, scaleAABB.max.y);
    float scaleY = random.Get();
    random.SetMinMax(scaleAABB.min.z, scaleAABB.max.z);
    float scaleZ = random.Get();

    particle.transform.scale = transform.eTransform_.scale + Vector3{ scaleX ,scaleY, scaleZ };

    Vector3 newTransform = transform.GetWorldPosition();
    random.SetMinMax(translateAABB.min.x, translateAABB.max.x);
    particle.transform.translate.x = random.Get() + newTransform.x;
    random.SetMinMax(translateAABB.min.y, translateAABB.max.y);
    particle.transform.translate.y = random.Get() + newTransform.y;
    random.SetMinMax(translateAABB.min.z, translateAABB.max.z);
    particle.transform.translate.z = random.Get() + newTransform.z;

    if (useRadialEmission) {
        float longitudeAngle = ((float)count / (float)maxCount) * (2.0f * Math::kPi);
       //// 2. X軸とZ軸の回転量を計算
       float rotateX = std::cos(longitudeAngle);
       float rotateZ = std::sin(longitudeAngle);

       float leanAmount = halfPi;

       // 3. 回転を適用（Y軸は0、または基準の回転）
       particle.transform.rotate.x = (rotateX * halfPi)+ halfPi + transform.eTransform_.rotate.x;
       particle.transform.rotate.y = longitudeAngle + transform.eTransform_.rotate.y;
       particle.transform.rotate.z = -(rotateZ * halfPi) + halfPi + transform.eTransform_.rotate.z;
    } else {
        random.SetMinMax(rotateAABB.min.x, rotateAABB.max.x);
        float rotateX = random.Get();
        random.SetMinMax(rotateAABB.min.y, rotateAABB.max.y);
        float rotateY = random.Get();
        random.SetMinMax(rotateAABB.min.z, rotateAABB.max.z);
        float rotateZ = random.Get();
        particle.transform.rotate = Vector3{ rotateX ,rotateY , rotateZ } + transform.eTransform_.rotate;
    }
   

    particle.currentTime = 0;

    if (color == Vector4{ 0.0f,0.0f,0.0f,0.0f }) {
        random.SetMinMax(0.0f, 1.0f);
        particle.color = { random.Get(), random.Get(), random.Get(),1.0f };
    } else {
        particle.color = color;
    }
   
    return particle;
}


SphericalMove MakeNewSphericalCoordinate(const float& radius, const int& count, const int& maxCount, const float& radiusSpeed, const float& polarSpeed, const MinMax& polarSpeedMinMax, const MinMax& radiusSpeedMinMax)
{
    SphericalMove spherical;
    Random random;
    spherical.coordinate.azimuthal = 0.0f;
    spherical.coordinate.polar = Math::kPi*2.0f / maxCount * count;
    spherical.coordinate.radius = radius;
    random.SetMinMax(polarSpeedMinMax.min, polarSpeedMinMax.max);
    spherical.polarSpeed = polarSpeed + random.Get();
    random.SetMinMax(radiusSpeedMinMax.min, radiusSpeedMinMax.max);
    spherical.radiusSpeed = radiusSpeed + random.Get();
    return spherical;
}

void ParticleManager::CreateParticleGroup(const std::string name, const TextureFactory::Handle& textureHandle, const Primitive::MeshType& topologyType, const bool& useModel, const float temperature, const std::string& modelTag)
{

    assert(!particleGroups.contains(name));
    std::unique_ptr<ParticleGroup> newParticleGroup = std::make_unique<ParticleGroup>();

    InitAccelerationField(*newParticleGroup);

    newParticleGroup->useBillboard = true;
    newParticleGroup->useSpriteCamera = false;
    newParticleGroup->useModel = useModel;
    newParticleGroup->textureSize = { 100.0f,100.0f };

    CreateMaterial(*newParticleGroup,temperature);

    std::map<std::string, MaterialData> materials;

    if (newParticleGroup->useModel) {
        newParticleGroup->model = ModelManager::GetModel(modelTag);
        materials = newParticleGroup->model->GetModelData()->materials;
    }

    //モデルのマテリアルがあれば
    if (!materials.empty()) {

        for (auto& [name, material] : materials) {
            newParticleGroup->materialData.textureData_[TEXTURE_USAGE_DIFFUSE].textureSrvIndex = material.textureData_[TEXTURE_USAGE_DIFFUSE].textureSrvIndex;
        }

    } else {
        newParticleGroup->materialData.textureData_[TEXTURE_USAGE_DIFFUSE].textureSrvIndex = Texture::GetSRVHandle(textureHandle);
    }
  
    newParticleGroup->primitive = std::make_unique<Primitive>();
    assert(newParticleGroup->primitive);

    MeshData meshData = PrimitiveFactory::GetMeshData(topologyType);
    newParticleGroup->primitive->Create(meshData);

    //Instancing用のTransformationMatrixリソースを作成
    newParticleGroup->instancingResource.CreateBufferResource(L"Particle_InstancingResource", sizeof(ParticleForGPU) * kNumMaxInstance);
    //書き込むためのアドレスを取得
    newParticleGroup->instancingResource.Map();
    assert(newParticleGroup->instancingResource.Get());

    for (uint32_t index = 0; index < kNumMaxInstance; ++index) {
        newParticleGroup->instancingResource.data[index].WVP = MakeIdentity4x4();
        newParticleGroup->instancingResource.data[index].World = MakeIdentity4x4();
        newParticleGroup->instancingResource.data[index].color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
    }

    newParticleGroup->instancingResource.UnMap();
    newParticleGroup->instancingResource.Allocate(srvDescriptorHeap_);
    newParticleGroup->instancingResource.CreateSRVforStructuredBuffer(
        srvDescriptorHeap_,
        kNumMaxInstance
    );

    particleGroups.insert(std::make_pair(name, std::move(newParticleGroup)));

    LogFile::Log("Create Particle Group\n");
}

void ParticleManager::Update(Camera& camera)
{
    camera_ = &camera;

    for (auto& [name, group] : particleGroups) {

        if (group->useSpriteCamera) {
            //ビルボードをしない
            group->useBillboard = false;
        }

        UpdateBillBordMatrix(camera);

        UpdateFunctions[group->movement](*group); // ← それぞれの動きに応じて更新！
    }
}

std::list<Particle> EmitParticles(const AABB& velocityAABB, const WorldTransform& transform,const bool useRadialEmission, uint32_t count, const Vector4& color, const float& lifeTime, const AABB& translateAABB, const AABB& rotateAABB, const AABB& scaleAABB)
{
    std::list<Particle>particles;
    for (uint32_t i = 0; i < count; ++i) {
        particles.push_back(MakeNewParticle(velocityAABB, transform,useRadialEmission, i, count, color, lifeTime, translateAABB, rotateAABB, scaleAABB));
    }
    return particles;
}


std::list<SphericalMove> EmitCoordinate(uint32_t count, const float& radius, const float& radiusSpeed, const float& polarSpeed, const MinMax& polarSpeedMinMax, const MinMax& radiusSpeedMinMax)
{
    std::list<SphericalMove>sphericalCoordinates;

    for (uint32_t i = 0; i < count; ++i) {
        sphericalCoordinates.push_back(MakeNewSphericalCoordinate(radius, i, count, radiusSpeed, polarSpeed, polarSpeedMinMax, radiusSpeedMinMax));
    }
    return sphericalCoordinates;

}

void ParticleManager::Emit(Emitter& emitter)
{
    
    assert(particleGroups.contains(emitter.name));

    particleGroups[emitter.name]->particles.splice(particleGroups[emitter.name]->particles.end(), EmitParticles(emitter.velocityAABB, emitter.transform, emitter.useRadialEmission_, emitter.count, emitter.startColor, emitter.lifeTime, emitter.translateAABB_, emitter.rotateAABB_, emitter.scaleAABB_));
    //どのように動くかの設定
    particleGroups[emitter.name]->movement = emitter.movement;
    //放射線にするかどうか
    particleGroups[emitter.name]->useRadialEmission = emitter.useRadialEmission_;
    //ビルボードにするかどうか
    particleGroups[emitter.name]->useBillboard = emitter.useBillboard_;
    
    //位置の設定
    particleGroups[emitter.name]->parentPos_ = &emitter.transform;
    //ブレンドモードの設定
    particleGroups[emitter.name]->blendMode = emitter.blendMode;
    //カラーの設定
    particleGroups[emitter.name]->startColor = emitter.startColor;
    particleGroups[emitter.name]->endColor = emitter.endColor;

    //加速場の設定
    particleGroups[emitter.name]->accelerationField = emitter.accelerationField_;
    //動きによって球面座標をEmittする
    if (emitter.movement == kParticleSphere || emitter.movement == kParticleShock) {
        particleGroups[emitter.name]->sphericalCoordinates.splice(particleGroups[emitter.name]->sphericalCoordinates.end(), EmitCoordinate(emitter.count, emitter.radius, emitter.radiusSpeed, emitter.polarSpeed, emitter.polarSpeedMinMax, emitter.radiusSpeedMinMax));
    }
}


void ParticleManager::Normal(ParticleGroup& group)
{
    group.numInstance = 0;

    for (std::list <Particle>::iterator particleIterator = group.particles.begin(); particleIterator != group.particles.end();) {

        if (group.numInstance < kNumMaxInstance) {

            //寿命に達していたらグループから外す
            if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
                particleIterator = group.particles.erase(particleIterator);
                continue;
            }

            //場の処理
            IsCollisionFieldArea(*particleIterator, group);
            const float deltaTime = TimeManager::DeltaTime();
            //移動処理
            (*particleIterator).transform.translate += (*particleIterator).velocity * deltaTime;
            //経過時間を加算
            (*particleIterator).currentTime += deltaTime;

            worldMatrix = UpdateMatrix(*particleIterator, group);

            //ビュープロジェクション行列
            UpdateWVPMatrix(*camera_, group);

            //データの更新
            UpdateInstancingData(group, *particleIterator);

        }
        ++particleIterator;
    }


}

void ParticleManager::Sphere(ParticleGroup& group)
{
    group.numInstance = 0;
    auto particleIterator = group.particles.begin();
    auto coordIterator = group.sphericalCoordinates.begin();

    while (particleIterator != group.particles.end() && coordIterator != group.sphericalCoordinates.end()) {

        if (group.numInstance < kNumMaxInstance) {

            if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
                particleIterator = group.particles.erase(particleIterator);
                coordIterator = group.sphericalCoordinates.erase(coordIterator);
                continue;
            }

            coordIterator->coordinate.polar += coordIterator->polarSpeed;

            if (coordIterator->coordinate.radius > 0.0f) {
                coordIterator->coordinate.radius -= coordIterator->radiusSpeed;
            } else {
                particleIterator = group.particles.erase(particleIterator);
                coordIterator = group.sphericalCoordinates.erase(coordIterator);
                continue;
            }
   
            const float deltaTime = TimeManager::DeltaTime();
            particleIterator->transform.translate += particleIterator->velocity * deltaTime;
            (*particleIterator).currentTime += deltaTime;


            IsCollisionFieldArea(*particleIterator, group);

            worldMatrix = UpdateSphereMatrix(*particleIterator, *coordIterator, group);

            UpdateWVPMatrix(*camera_, group);

            UpdateInstancingData(group, *particleIterator);

        }

        ++particleIterator;
        ++coordIterator;
    }


}

void ParticleManager::Shock(ParticleGroup& group)
{

    group.numInstance = 0;
    auto particleIterator = group.particles.begin();
    auto coordIterator = group.sphericalCoordinates.begin();

    while (particleIterator != group.particles.end() && coordIterator != group.sphericalCoordinates.end()) {

        if (group.numInstance < kNumMaxInstance) {

            if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
                particleIterator = group.particles.erase(particleIterator);
                coordIterator = group.sphericalCoordinates.erase(coordIterator);
                continue;
            }


            if (coordIterator->coordinate.radius > 8.0f) {
                particleIterator = group.particles.erase(particleIterator);
                coordIterator = group.sphericalCoordinates.erase(coordIterator);
                continue;
            } else {
                coordIterator->coordinate.radius += coordIterator->radiusSpeed;
            }


            const float deltaTime = TimeManager::DeltaTime();

   

            particleIterator->transform.translate += particleIterator->velocity * deltaTime;
            (*particleIterator).currentTime += deltaTime;

            IsCollisionFieldArea(*particleIterator, group);

            worldMatrix = UpdateSphereMatrix(*particleIterator, *coordIterator, group);

            UpdateWVPMatrix(*camera_, group);

            UpdateInstancingData(group, *particleIterator);

        }

        ++particleIterator;
        ++coordIterator;
    }


}

void ParticleManager::Wave(ParticleGroup& group)
{

    group.numInstance = 0;

    for (std::list <Particle>::iterator particleIterator = group.particles.begin(); particleIterator != group.particles.end();) {

        if (group.numInstance < kNumMaxInstance) {

            //寿命に達していたらグループから外す
            if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
                particleIterator = group.particles.erase(particleIterator);
                continue;
            }

            //場の処理
            IsCollisionFieldArea(*particleIterator, group);

            const float deltaTime = TimeManager::DeltaTime();
  


            //移動処理
            (*particleIterator).transform.translate += (*particleIterator).velocity * deltaTime;

            //経過時間を加算
            (*particleIterator).currentTime += deltaTime;

            (*particleIterator).theta += Math::kPi * deltaTime * 2.0f;


            float sinf = std::sinf((*particleIterator).theta);
            Vector3 wavePos = { sinf ,0.0f,sinf };
            Matrix4x4 waveMoveMat = MakeTranslateMatrix(wavePos);

            worldMatrix =UpdateMatrix(*particleIterator, group)* waveMoveMat;


            //ビュープロジェクション行列
            UpdateWVPMatrix(*camera_, group);

            //データの更新
            UpdateInstancingData(group, *particleIterator);

        }
        ++particleIterator;
    }

}


// ==========================================================================================================


void ParticleManager::IsCollisionFieldArea(Particle& particleItr, ParticleGroup& group)
{
    if (IsCollision(group.accelerationField.area, particleItr.transform.translate)) {
        particleItr.velocity += group.accelerationField.acceleration * TimeManager::DeltaTime();
    }
}


void ParticleManager::Draw()
{

    for (const auto& [name, group] : particleGroups) {

        if (group->numInstance > 0) {
            //rootSignatureの設定
            commandList_->SetGraphicsRootSignature(rootSignature_->GetRootSignature(RootSignature::PARTICLE));
            commandList_->SetPipelineState(PSO::GetGraphicsPipelineStateParticle(group->blendMode).Get());
   
            //マテリアルの設定
            commandList_->SetGraphicsRootConstantBufferView(0, group->materialResource.GetGPUVirtualAddress());
            //粒ごとのトランスフォーム
            srvDescriptorHeap_->SetGraphicsRootDescriptorTable(1, group->instancingResource.srvIndex, commandList_);
            //テスクチャ
            srvDescriptorHeap_->SetGraphicsRootDescriptorTable(2, group->materialData.textureData_[TEXTURE_USAGE_DIFFUSE].textureSrvIndex,commandList_);
            //描画!（DrawCall/ドローコール）6個のインデックスを使用しインスタンスを描画。

            if (group->model != nullptr && group->useModel) {

                auto* model = group->model;
                commandList_->IASetPrimitiveTopology(model->GetTopology());
                commandList_->IASetVertexBuffers(0, 1, &model->GetVertexBufferView());
                commandList_->DrawInstanced(UINT(model->GetModelData()->vertices.size()), group->numInstance, 0, 0);

            } else {
          
                auto& primitive = group->primitive;
                commandList_->IASetPrimitiveTopology(primitive->GetTopology());
                commandList_->IASetVertexBuffers(0, 1, &primitive->GetVertexBufferView());

                if (primitive->GetIndexCount() > 0) {
                    commandList_->IASetIndexBuffer(&primitive->GetIndexBufferView());
                    commandList_->DrawIndexedInstanced(primitive->GetIndexCount(), group->numInstance, 0, 0, 0);
                } else {
                    commandList_->DrawInstanced(primitive->GetVertexCount(), group->numInstance, 0, 0); // ラインなどインデックスが無い場合
                }
            }
        }

    }
}

void ParticleManager::InitAccelerationField(ParticleGroup& group)
{
    group.accelerationField.acceleration = { 0.0f,0.0f,0.0f };
    group.accelerationField.area.min = { -1.0f,-1.0f,-1.0f };
    group.accelerationField.area.max = { 1.0f,1.0f,1.0f };
}

void ParticleManager::Finalize()
{

    for (auto& [name, group] : particleGroups) {
        group->instancingResource.Reset();
        group->materialResource.Reset();
    }

    particleGroups.clear();

    LogFile::Log("Finalize ParticleManager\n");

}

// ==========================================================================================================

void ParticleManager::UpdateInstancingData(ParticleGroup& group, Particle& particleItr)
{

    group.instancingResource.Map();

    //データにそれぞれ追加
    group.instancingResource.data[group.numInstance].WVP = worldViewProjectionMatrix;
    group.instancingResource.data[group.numInstance].World = worldMatrix;
    group.instancingResource.data[group.numInstance].color = (particleItr).color;
    float time = ((particleItr).currentTime / (particleItr).lifeTime);
    group.instancingResource.data[group.numInstance].color = Lerp(group.startColor, group.endColor, time);
   
    group.instancingResource.UnMap();

    ++group.numInstance;

}

void ParticleManager::CreateMaterial(ParticleGroup& group,const float temperature)
{
    //マテリアル用のリソースを作る。
    group.materialResource.CreateBufferResource(L"ParticleGroup_MaterialResource\n");

    //書き込むためのアドレスを取得
    HRESULT result = group.materialResource.Map();
    group.materialResource.data->color = { 1.0f,1.0f,1.0f,1.0f };
    group.materialResource.data->lightMode = Object3d::LightMode::kLightModeNone;
    group.materialResource.data->uvTransform = MakeIdentity4x4();
    group.materialResource.data->shininess = 50.0f;
    group.materialResource.data->environmentCoefficient = 0.0f;
    //体温 
    group.materialResource.data->temperature = temperature;

}


void ParticleManager::Reset(const std::string& name)
{
    if (particleGroups.contains(name)) {
        auto& group = particleGroups[name];
        group->particles.clear();
        group->sphericalCoordinates.clear();
        group->numInstance = 0;

        group->instancingResource.Map();

        for (uint32_t i = 0; i < kNumMaxInstance; ++i) {
            group->instancingResource.data[i].WVP = MakeIdentity4x4();
            group->instancingResource.data[i].World = MakeIdentity4x4();
            group->instancingResource.data[i].color = Vector4{ 1.0f, 1.0f, 1.0f, 0.0f }; // アルファ0で非表示に
        }

        group->instancingResource.UnMap();
    }
}

void ParticleManager::ResetAll()
{
    for (auto& [name, group] : particleGroups) {
        Reset(name);
    }
}

std::unordered_map<std::string, std::unique_ptr<ParticleGroup>>& ParticleManager::GetParticleGroups()
{
    return particleGroups;
}

// ======================================//行列の更新//====================================================================

void ParticleManager::UpdateBillBordMatrix(Camera& camera)
{
    billboardMatrix = Math::GetBillBordMatrix(camera.GetWorldMatrix());
}

void ParticleManager::UpdateWVPMatrix(Camera& camera, ParticleGroup& group)
{

    if (group.useSpriteCamera) {
        worldViewProjectionMatrix = Multiply(worldMatrix, SpriteCamera::GetViewProjectionMatrix());
    } else {
        worldViewProjectionMatrix = Multiply(worldMatrix, camera.GetViewProjectionMatrix());
    }

}

Matrix4x4 ParticleManager::UpdateMatrix(Particle& particleItr, ParticleGroup& group)
{
    //ビルボード処理
    if (group.useBillboard) {
        return UpdateWorldMatrixForBillBord(particleItr, group);
    } else {
        return UpdateWorldMatrix(particleItr, group);
    }
}

Matrix4x4  ParticleManager::UpdateSphereMatrix(Particle& particleItr, SphericalMove& sphericalMove, ParticleGroup& group) {

    Vector3 spherePos =/* group.parentPos_->GetWorldPosition() + */TransformCoordinate(sphericalMove.coordinate);
    Matrix4x4 sphereMoveMat = MakeTranslateMatrix(spherePos);
    Matrix4x4 child = UpdateMatrix(particleItr, group);
    return child * sphereMoveMat;
}

Matrix4x4 ParticleManager::UpdateWorldMatrixForBillBord(Particle& particleItr, ParticleGroup& group)
{
    Matrix4x4 scaleMatrix = MakeScaleMatrix(particleItr.transform.scale);
    Matrix4x4 translateMatrix = MakeTranslateMatrix(particleItr.transform.translate);
    Matrix4x4 rotateMatrix = MakeRotateXYZMatrix(particleItr.transform.rotate) * billboardMatrix;
    return scaleMatrix * rotateMatrix * translateMatrix;
}

Matrix4x4 ParticleManager::UpdateWorldMatrix(Particle& particleItr, ParticleGroup& group)
{

    if (group.useSpriteCamera) {
        Vector3 translate = particleItr.transform.translate;
        translate = translate * group.textureSize;
        translate.x *= -1.0f;

        return MakeAffineMatrix(particleItr.transform.scale * group.textureSize, particleItr.transform.rotate, translate);
    } else {

        return MakeAffineMatrix(particleItr.transform.scale, particleItr.transform.rotate, particleItr.transform.translate);
    }

}

