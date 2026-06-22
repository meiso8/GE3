#include "FreeTypeScene.h"
#include"Input.h"
#include"AABB.h"

#include"DebugUI.h"
#include"Model.h"
#include"Object3d/ObjectManager/ObjectManager.h"

FreeTypeScene::FreeTypeScene()
{

    handle_ = FreeTypeManager::CreateFace("Resource/Fonts/NotoSansEgyptianHieroglyphs-Regular.ttf", 0);
    FreeTypeManager::SetPixelSizes(handle_, 32, 32);
    text_.Initialize(handle_);
    pressSpaceText_.Initialize(handle_);

    text_.SetString(U"\U00013000ButtobiEngine");
    text_.SetPosition({ 640, 360 });
    text_.SetColor({ 1, 0, 0, 1 });
    text_.SetAlign(TextAlign::Center);
    text_.SetBlendMode(BlendMode::kBlendModeNormal);

    pressSpaceText_.SetString(U"SPACE");
    pressSpaceText_.SetPosition({ 640, 360 + 128 });
    pressSpaceText_.SetColor({ 1, 0, 0, 1 });
    pressSpaceText_.SetAlign(TextAlign::Center);
    pressSpaceText_.SetBlendMode(BlendMode::kBlendModeNormal);
    sprite_ = std::make_unique<Sprite>();
    sprite_->Create(TextureFactory::NUMBERS, { 0,0 });
    sprite_->SetSize({ 1280,720 });

    skyBoxObj_ = std::make_unique<SkyboxObject3d>();
    skyBoxObj_->Create();

    cylinder_ = std::make_unique<Primitive>();
    cylinder_->Create(PrimitiveGenerator::CreateCylinder());

    object3d_ = std::make_unique<Object3d>();

    object3d_->Create();
    object3d_->SetMeshAndMaterial(cylinder_.get());

    object3d_->SetTextureHandle(TextureFactory::GRADATION_LINE);
    object3d_->GetMaterial().environmentCoefficient = 0.5f;

    beam_ = std::make_unique<Beam>();
   
    levelEditor_ = std::make_unique<LevelEditor>();
    levelEditor_->Load("test");
    //オブジェクトをセットする
    levelEditor_->CreateObject(objects_);


    auto* levelData = levelEditor_->GetLevelData();

    for (auto& enemyData : levelData->enemies) {
        std::unique_ptr<Object3d> enemy = std::make_unique<Object3d>();
        enemy->Create();
        enemy->SetMeshAndMaterial(ModelManager::GetModel(enemyData.fileName));
        enemy->Initialize();
        enemy->SetTransform(enemyData.transform);
        enemies_.push_back(std::move(enemy));
    }


}

void FreeTypeScene::Initialize()
{

    ParticleManager::ResetAll();

    camera_->Initialize();

    currentCamera_ = camera_.get();

    //player_->Init();

    //if (!levelData->players.empty()) {
    //    auto& playerData = levelData->players[0];
    //    player_->SetBodyPos(playerData.transform.translate);
    //    player_->SetBodyRotate(playerData.transform.rotate);
    //    player_->SetBodyScale(playerData.transform.scale);
    //}
    //player_->Update();

    beam_->Initialize();
    sceneChange_->Initialize();
    sceneChange_->SetState(SceneChange::kFadeOut, 1.0f);
    CreateParticle();
}

void FreeTypeScene::Update()
{
    //デバック
    text_.Debug();

    if (inputText_ == U"next") {
        sceneChange_->SetState(SceneChange::kFadeIn, 1.0f);
        SceneManager::SetNextScene("Title");
    }

    if (Input::IsTriggerKey(DIK_SPACE)) {
        sceneChange_->SetState(SceneChange::kFadeIn, 1.0f);
        SceneManager::SetNextScene("Title");
    }
    currentCamera_->UpdateMatrix();
 
#ifdef _DEVELOP

    if (Input::IsTriggerKey(DIK_1)) {
        SwitchCamera();
    }

    DebugUI::CheckEmitter(particleEmitters_[0]->GetEmitter());
    DebugUI::CheckEmitter(particleEmitters_[1]->GetEmitter()); 

#endif //_DEVELOP

    beam_->Update();

    for (auto& obj : objects_) {
        obj->obj_->SetTemperature(1.0f);
    }

    for (int i = 0; i < particleEmitters_.size(); ++i) {
        particleEmitters_[i]->Update();
    }

}

void FreeTypeScene::DrawSprite() {

    Sprite::PreDraw();
    text_.Draw();
    pressSpaceText_.Draw();
    sceneChange_->Draw();

};

void FreeTypeScene::DrawModel()
{

    skyBoxObj_->Draw(*currentCamera_);
    object3d_->Draw(*currentCamera_,BlendMode::kBlendModeAdd,CullMode::kCullModeNone,MaskMode::kZero);
    beam_->Draw(currentCamera_);


}

void FreeTypeScene::CreateParticle()
{
    for (int i = 0; i < particleEmitters_.size(); ++i) {
        particleEmitters_[i] = std::make_unique<ParticleEmitter>();
        particleEmitters_[i]->Initialize();
    }

    particleEmitters_[0]->SetName("particle1");

    auto& emitter0 = particleEmitters_[0]->GetEmitter();

    emitter0.count = 8;
    emitter0.color = { 1.0f,1.0f,1.0f,1.0f };
    emitter0.transform.eTransform_.scale = { 0.05f,1.0f,1.0f };
    emitter0.transform.eTransform_.rotate = { 0.0f,0.0f,0.0f };
    emitter0.transform.eTransform_.translate = { 0.5f,0.5f,-0.5f };

    emitter0.frequencyTime = 0.0f;
    emitter0.frequency = 2.0f;
    emitter0.lifeTime = 2.0f;
    emitter0.blendMode = kBlendModeAdd;
    emitter0.movement = ParticleMovements::kParticleNormal;
    emitter0.rotateAABB_ = { .min = {0.0f,0.0f,-3.14f},.max = {0.0f,0.0f,3.14f} };
    emitter0.scaleAABB_ = { .min = {0.0f,0.4f,0.0f},.max = {0.0f,1.5f,1.0f} };
    emitter0.isLoop_ = true;

    auto& group = ParticleManager::GetInstance()->GetParticleGroup(emitter0.name);
    group->accelerationField.acceleration.y = 0.0f;
    group->accelerationField.area = { .min = {-1.0f,-1.0f,-1.0f},.max = {1.0f,1.0f,1.0f} };
    group->useBillboard = true;

    particleEmitters_[1]->SetName("ring");

    auto& emitter1 = particleEmitters_[1]->GetEmitter();

    emitter1.count = 1;
    emitter1.color = { 1.0f,1.0f,1.0f,1.0f };
    emitter1.transform.eTransform_.scale = { 0.5f,0.5f,0.5f };
    emitter1.transform.eTransform_.rotate = { 0.0f,0.0f,0.0f };
    emitter1.transform.eTransform_.translate = { 0.5f,0.5f,0.0f };

    emitter1.frequencyTime = 0.0f;
    emitter1.frequency = 2.0f;
    emitter1.lifeTime = 2.0f;
    emitter1.blendMode = kBlendModeAdd;
    emitter1.movement = ParticleMovements::kParticleNormal;
    emitter1.rotateAABB_ = { .min = {0.0f,-3.14f,0.0f},.max = {0.0f,3.14f,0.0f} };
    emitter1.isLoop_ = true;

    auto& group1 = ParticleManager::GetInstance()->GetParticleGroup(emitter1.name);
    group1->accelerationField.acceleration.y = 0.0f;
    group1->accelerationField.area = { .min = {-1.0f,-1.0f,-1.0f},.max = {1.0f,1.0f,1.0f} };
    group1->useBillboard = false;
}
