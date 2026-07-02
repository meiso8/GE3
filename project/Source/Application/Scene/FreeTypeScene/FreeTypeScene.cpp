#include "FreeTypeScene.h"
#include"Input.h"
#include"AABB.h"

#include"DebugUI.h"
#include"Model.h"
#include"ObjectManager/ObjectManager.h"
#include"FreeTypeManager/FreeTypeManager.h"
#include "SceneManager.h"

FreeTypeScene::FreeTypeScene()
{

    handle_ = FreeTypeManager::CreateFace("Resource/Fonts/NotoSansEgyptianHieroglyphs-Regular.ttf", 0);
    FreeTypeManager::SetPixelSizes(handle_, 32, 32);
    text_.Initialize(handle_);
    pressSpaceText_.Initialize(handle_);

    text_.SetString(U"\U00013000ButtobiEngine");
    text_.SetPosition({ 640, 360 });
    text_.SetColor({ 1, 0, 0, 1 });
    text_.SetAlign(Text::TextAlign::Center);
    text_.SetBlendMode(BlendMode::kBlendModeNormal);

    pressSpaceText_.SetString(U"SPACE");
    pressSpaceText_.SetPosition({ 640, 360 + 128 });
    pressSpaceText_.SetColor({ 1, 0, 0, 1 });
    pressSpaceText_.SetAlign(Text::TextAlign::Center);
    pressSpaceText_.SetBlendMode(BlendMode::kBlendModeNormal);
    sprite_ = std::make_unique<Sprite>();
    sprite_->Create(TextureFactory::NUMBERS, { 0,0 });
    sprite_->SetSize({ 1280,720 });

    skyBoxObj_ = std::make_unique<SkyboxObject3d>();
    skyBoxObj_->Create();

    beam_ = std::make_unique<Beam>();


}

void FreeTypeScene::Initialize()
{

    ParticleManager::ResetAll();

    camera_->Initialize();

    currentCamera_ = camera_.get();

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
    emitter0.useBillboard_ = true;

    emitter0.accelerationField_.acceleration.y = 0.0f;
    emitter0.accelerationField_.area = { .min = {-1.0f,-1.0f,-1.0f},.max = {1.0f,1.0f,1.0f} };

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
    emitter1.useBillboard_ = false;

    emitter1.accelerationField_.acceleration.y = 0.0f;
    emitter1.accelerationField_.area = { .min = {-1.0f,-1.0f,-1.0f},.max = {1.0f,1.0f,1.0f} };

}
