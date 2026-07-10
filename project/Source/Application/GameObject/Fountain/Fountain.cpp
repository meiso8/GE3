#include "Fountain.h"
#include"ModelManager.h"
#include"Model.h"
#include"DebugUI.h"
#include"TimeManager.h"

namespace {
    const float splashTime_ = 10.0f;
}

Fountain::Fountain()
{
    object_ = std::make_unique<Object3d>();
    object_->Create();
    object_->SetMeshAndMaterial(ModelManager::GetModel("Fountain.obj"));
    object_->SetTemperature(0.2f);

    waterObject_ = std::make_unique<Object3d>();
    waterObject_->Create();
    waterObject_->SetMeshAndMaterial(ModelManager::GetModel("Water2.obj"));
    waterObject_->SetTemperature(0.1f);

    SetCollisionAttribute(CollisionTag::GetTag("Wall")); // ミイラの衝突属性
    SetCollisionMask(CollisionTag::GetTag("Player") | CollisionTag::GetTag("Enemy")); // プレイヤーや壁と衝突
    //サイズに合わせる
    SetRadius(3.0f);
    SetWorldMatrix(object_->GetWorldTransform());

    CreateParticle();
}

Fountain::~Fountain()
{

}

void Fountain::Initialize()
{
    object_->Initialize();
    object_->SetTranslate({ 0.0f,0.25f,0.0f });
    splashTimer_ = splashTime_;

    waterObject_->Initialize();
    waterObject_->SetTranslate({0.0f,0.3f,0.0f });
    waterObject_->InitWaveData();
    waterObject_->SetColor({ 0.0f,1.0f,1.0f,0.2f });

    object_->SetObjectName("Fountain");
    object_->RegisterObject();
}

void Fountain::Update()
{
    object_->Update();

    waterObject_->GetWaveData(0).frequency = 4.0f;
    waterObject_->GetWaveData(0).amplitude = 0.03f;
    waterObject_->GetWaveData(1).frequency = 4.0f;
    waterObject_->GetWaveData(1).amplitude = 0.03f;

    waterObject_->GetWaveData(1).direction = { 0.0f,0.0f,1.0f };


    waterObject_->Update();

    auto& emitter0 = particleEmitter_[0]->GetEmitter();

    splashTimer_ -= TimeManager::DeltaTime();
    waterObject_->GetWaveData(0).time += TimeManager::DeltaTime()*2.0f;
    //waterObject_->GetWaveData(1).time = splashTimer_;

    if (splashTimer_ <= splashTime_ * 0.5f) {
        emitter0.accelerationField_.acceleration.y = -4.0f;
    } else {
        emitter0.accelerationField_.acceleration.y = -3.0f;
    }

    if (splashTimer_ <= 0.0f) {
        splashTimer_ = splashTime_;
    } 
 
    for (auto& emitter : particleEmitter_) {
        emitter->Update();
    } 
 
    DebugUI::CheckEmitter(particleEmitter_[1]->GetEmitter(),"water");

    ColliderUpdate();
    //StageManager::GetInstance()->SetNestStage("MedjedStage");
}

void Fountain::Draw(Camera& camera)
{
    object_->Draw(camera);
    waterObject_->Draw(camera);
    ColliderDraw(camera);
}

void Fountain::OnCollision(Collider* collider)
{
    if (collider->GetCollisionAttribute() == CollisionTag::GetTag("Player")) {
        // プレイヤーとぶつかったときの処理
        
    }

    OnCollisionCollider();
}

void Fountain::CreateParticle()
{
    for (auto& emitter : particleEmitter_) {
        emitter = std::make_unique<ParticleEmitter>();
        emitter->Initialize();
        //Objectにペアレントする
        emitter->SetParent(object_->GetWorldTransform());
    }
   
    particleEmitter_[0]->SetName("fountain");
    particleEmitter_[1]->SetName("fountain2");

    auto& emitter0 = particleEmitter_[0]->GetEmitter();
    emitter0.isLoop_ = true;
    emitter0.count = 5;
    emitter0.color = { 1.0f,1.0f,1.0f,1.0f };
    emitter0.frequency = 0.01f; 
    emitter0.lifeTime = 1.5f;
    emitter0.blendMode = kBlendModeAdd;
    emitter0.movement = ParticleMovements::kParticleNormal;
    emitter0.velocityAABB = { .min = {-0.2f,3.75f,-0.2f},.max = {0.2f,4.0f,0.2f} };
    emitter0.transform.eTransform_.scale = { 0.1f,0.1f,0.1f };
    emitter0.transform.eTransform_.translate.y = 0.2f;   
    emitter0.accelerationField_.acceleration.y = -4.0f;
    emitter0.accelerationField_.area = { .min = {-25.0f,0.0f,-25.0f},.max = {25.0f,40.0f,25.0f} };

    auto& emitter1 = particleEmitter_[1]->GetEmitter();
    emitter1.isLoop_ = true;
    emitter1.color = { 0.5f,1.0f,1.0f,1.0f };
    emitter1.movement = ParticleMovements::kParticleSphere;
    emitter1.transform.eTransform_.scale = { 0.1f,0.1f,0.1f };
    emitter1.frequency = 0.001f;
    emitter1.velocityAABB = { .min = {0.0f,1.5f,0.0f},.max = {0.0f,2.0f,0.0f} };
    emitter1.radius = 2.5f;
    emitter1.radiusSpeed = { 0.05f };
    emitter1.count = 7;
    emitter1.lifeTime = 1.0f;
    emitter1.polarSpeed = 0.0f;
    emitter1.accelerationField_.acceleration.y = -2.0f;
}
