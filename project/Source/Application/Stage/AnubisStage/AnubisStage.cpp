#include "AnubisStage.h"
#include"Sound.h"
#include"../StageManager.h"
#include "ParticleEmitter.h"
#include"DebugUI.h"

AnubisStage::AnubisStage()
{
    backGround_ = std::make_unique<BackGround>();
    CreateParticle();
}

void AnubisStage::Initialize()
{
    memoManager_->GenerateMemos({ TextureFactory::MEMO1, TextureFactory::MEMO3,TextureFactory::MEMO4,TextureFactory::BOOK4 });
    // ミイラ前に移動
    player_->Init({ 0.0f, 0.0f, -5.0f });

    Sound::StopAllSound();

    backGround_->Initialize();
    itemManager_->Init();

    Sound::bgmVolume_ = 0.1f;
}

void AnubisStage::Update()
{
    Sound::PlayBGM(SoundFactory::BGM_Sun);

    backGround_->Update();

    //ステージをMedjedStageにする
    //StageManager::GetInstance()->SetNestStage("MedjedStage");
    DebugUI::CheckParticle(*particleEmitters_, "Fountain");
}

void AnubisStage::Draw(Camera& camera)
{
    backGround_->Draw(camera);
}


void AnubisStage::DrawSprite()
{

}

void AnubisStage::CheckCollision(CollisionManager& collisionManager)
{
    // 壁との当たり判定
    for (auto& [type, object] : backGround_->GetBuilding()->GetFieldPoses()) {
        collisionManager.AddCollider(object.get());
    }
}

void AnubisStage::CreateParticle()
{

    particleEmitters_ = std::make_unique<ParticleEmitter>();
    particleEmitters_->SetName("medjedParticle");

    auto& emitter0 = particleEmitters_->GetEmitter();
    emitter0.count = 8;
    emitter0.color = { 1.0f,0.75f,0.75f,1.0f };
    emitter0.frequencyTime = 0.25f;
    emitter0.lifeTime = 6.0f;
    emitter0.blendMode = kBlendModeMultiply;
    emitter0.movement = ParticleMovements::kParticleSphere;
    emitter0.radius = 3.0f;
    float pi = std::numbers::pi_v<float>;
    emitter0.rotateAABB_ = { .min = {-pi ,-pi ,-pi } ,.max = { pi, pi, pi} };
    auto& group = ParticleManager::GetInstance()->GetParticleGroup(emitter0.name);
    group->accelerationField.acceleration.y = 5.0f;
    group->accelerationField.area = { .min = {-25.0f,0.0f,-25.0f},.max = {25.0f,40.0f,25.0f} };

}
