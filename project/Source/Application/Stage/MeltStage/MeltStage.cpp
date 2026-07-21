#include "MeltStage.h"
#include "ParticleEmitter.h"
#include"DebugUI.h"

MeltStage::MeltStage()
{
    backGround_ = std::make_unique<BackGround>();
    meltBlockMap_ = std::make_unique<MeltBlockMap>();
    bastet_ = std::make_unique<Bastet>();
    CreateParticle();
}

void MeltStage::Initialize()
{

    //ステージのロード
    LoadAndCreateObject("MeltStage_objectEditor");
    //ライトの初期化
    lightingManager_->Initialize();
    //パーティクルのリセット
    ParticleManager::ResetAll();
   
    backGround_->Initialize();

    //メルトブロック

    meltBlockMap_->Initialize();

    //少し手前側に移動する
    player_->Init({ 0.0f, 0.0f, -5.0f });
    bastet_->LoadMap("MeltStage_BastetStep");
    bastet_->Initialize();
}

void MeltStage::Update()
{
    for (auto& emitter : particleEmitters_) {
        emitter->Update();
    }

    backGround_->Update();
    meltBlockMap_->Update();


    if (meltBlockMap_->IsClear()) {
        //なぞ解きをクリアしたら
   
        bastet_->Update();

    }



    //オブジェクトの更新
    UpdateObject();

#ifdef USE_IMGUI

    DebugUI::CheckEmitter(particleEmitters_[0]->GetEmitter(), "base");
    DebugUI::CheckEmitter(particleEmitters_[1]->GetEmitter(), "light");
    DebugUI::CheckEmitter(particleEmitters_[2]->GetEmitter(), "firePoint");
#endif // !USE_IMGUI
}

void MeltStage::Draw(Camera& camera)
{
    backGround_->Draw(camera);
    meltBlockMap_->Draw(camera);


    //オブジェクトの描画
    DrawObject(camera);

    if (meltBlockMap_->IsClear()) {
        //なぞ解きをクリアしたら

        bastet_->Draw(camera);
    }

}

void MeltStage::CheckCollision(CollisionManager& collisionManager)
{

    meltBlockMap_->RayCastHit(*player_->GerRaySprite());

    if (meltBlockMap_->IsClear()) {
        //なぞ解きに正解したらコライダーなど追加
        bastet_->RayCastHit(*player_->GerRaySprite());
        collisionManager.AddCollider(bastet_.get());
    }
    // 壁との当たり判定
    for (auto& [type, object] : backGround_->GetBuilding()->GetFieldPoses()) {
        collisionManager.AddCollider(object.get());
    }

    //マップ
    for (auto& block : meltBlockMap_->GetMap()) {
        collisionManager.AddCollider(block.get());
    }


    if (meltBlockMap_->IsClear()) {
        //なぞ解きをクリアしたら
        collisionManager.AddCollider(bastet_.get());
    }

 

    //コライダーを追加する
    AddObjectCollision(collisionManager);
}

void MeltStage::CreateParticle()
{
    for (int i = 0; i < particleEmitters_.size(); ++i) {
        particleEmitters_[i] = std::make_unique<ParticleEmitter>();
        particleEmitters_[i]->Initialize();

    }


    auto& emitter0 = particleEmitters_[0]->GetEmitter();
    emitter0.count = 8;
    emitter0.transform.eTransform_.translate = { -10.0f, 0.5f,0.0f };
    emitter0.startColor = { 1.0f,45.0f / 256.0f,0.0f,1.0f };
    emitter0.endColor = { 1.0f,0.0f,0.0f,0.0f };
    emitter0.frequency = 0.25f;
    emitter0.frequencyTime = 0.25f;
    emitter0.rotateAABB_.min = { 0.0f,0.0f,0.0f };
    emitter0.rotateAABB_.max = { 0.0f,0.0f,0.0f };
    emitter0.lifeTime = 1.0f;
    emitter0.blendMode = kBlendModeNormal;
    emitter0.movement = ParticleMovements::kParticleSphere;
    emitter0.radius = 0.45f;
    emitter0.isLoop_ = true;
    emitter0.accelerationField_.acceleration.y = 5.0f;
    emitter0.accelerationField_.area = { .min = {-25.0f,0.0f,-25.0f},.max = {25.0f,40.0f,25.0f} };

    auto& emitter1 = particleEmitters_[1]->GetEmitter();
    emitter1 = emitter0;
    emitter1.blendMode = kBlendModeAdd;
    emitter1.radius = 0.9f;
    emitter1.frequency = 0.125f;
    emitter0.frequencyTime = 0.0f;

    emitter1.startColor = { 1.0f,0.0f,0.0f,1.0f };
    emitter1.endColor = { 1.0f,120.0f / 256.0f,0.0f,0.0f };

    particleEmitters_[0]->SetName("fireBase");
    particleEmitters_[1]->SetName("fireLight");
    particleEmitters_[2]->SetName("firePoint");

    auto& emitter2 = particleEmitters_[2]->GetEmitter();
    emitter2.count = 1;
    emitter2.transform.eTransform_.translate = { -10.0f, 2.0f,0.0f };
    emitter2.transform.eTransform_.scale = { 0.05f, 0.05f,0.05f };
    emitter2.velocityAABB = { .min = {0.0f,-0.1f,0.0f},.max = {0.0f,0.1f,0.0f} };
    emitter2.translateAABB_ = { .min = {-1.0f,0.0f,-1.0f},.max = {1.0f,0.0f,1.0f} };
    emitter2.startColor = { 1.0f,0.0f,0.0f,1.0f };
    emitter2.endColor = { 1.0f,120.0f / 256.0f,0.0f,0.0f };

    emitter2.frequency = 0.25f;
    emitter2.frequencyTime = 0.125f;

    emitter2.rotateAABB_.min = { 0.0f,0.0f,0.0f };
    emitter2.rotateAABB_.max = { 0.0f,0.0f,0.0f };
    emitter2.lifeTime = 1.0f;
    emitter2.blendMode = kBlendModeNormal;
    emitter2.movement = ParticleMovements::kParticleWave;
    emitter2.radius = 0.45f;
    emitter2.isLoop_ = true;
    emitter2.accelerationField_.acceleration.y = 5.0f;
    emitter2.accelerationField_.area = { .min = {-25.0f,0.0f,-25.0f},.max = {25.0f,40.0f,25.0f} };
}
