#include "FreeTypeScene.h"
#include"Input.h"
#include"AABB.h"
#include"DrawGrid.h"
#include"DebugUI.h"
#include"Model.h"

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

    cubeMesh_ = std::make_unique<Primitive>();
    cubeMesh_->Create(PrimitiveGenerator::CreateCube());

    cylinder_ = std::make_unique<Primitive>();
    cylinder_->Create(PrimitiveGenerator::CreateCylinder(),TextureFactory::GRADATION_LINE);

    object3d_ = std::make_unique<Object3d>();
    object3d_->Create();
    object3d_->SetMesh(cylinder_.get());
    object3d_->GetMaterial().environmentCoefficient = 0.5f;

    object3d2_ = std::make_unique<Object3d>();
    object3d2_->Create();
    object3d2_->SetMesh(cubeMesh_.get());

    levelEditor_ = std::make_unique<LevelEditor>();
    levelEditor_->Load("test");
    //オブジェクトをセットする
    levelEditor_->CreateObject(objects_);

}

void FreeTypeScene::Initialize()
{
    ParticleManager::ResetAll();

    camera_->Initialize();
    camera_->nearZ_ = 1.0f;

    currentCamera_ = camera_.get();

    //player_->Init();
    auto* levelData = levelEditor_->GetLevelData();
    //if (!levelData->players.empty()) {
    //    auto& playerData = levelData->players[0];
    //    player_->SetBodyPos(playerData.transform.translate);
    //    player_->SetBodyRotate(playerData.transform.rotate);
    //    player_->SetBodyScale(playerData.transform.scale);
    //}
    //player_->Update();

    for (auto& enemyData : levelData->enemies) {
        std::unique_ptr<Object3d> enemy = std::make_unique<Object3d>();
        enemy->Create();
        enemy->Initialize();
        enemy->SetMesh(ModelManager::GetModel(enemyData.fileName));
        enemy->worldTransform_.translate_ = enemyData.transform.translate;
        enemy->worldTransform_.rotate_ = enemyData.transform.rotate;
        enemy->worldTransform_.scale_ = enemyData.transform.scale;
        enemies_.push_back(std::move(enemy));
    }

    CreateParticle();
}

void FreeTypeScene::Update()
{
    //デバック
    text_.Debug();

    if (inputText_ == U"next") {
        sceneChange_->SetState(SceneChange::kFadeIn, 30);
        SceneManager::SetNestScene("Title");
    }

    if (Input::IsTriggerKey(DIK_SPACE)) {
        sceneChange_->SetState(SceneChange::kFadeIn, 30);
        SceneManager::SetNestScene("Title");
    }
    currentCamera_->UpdateMatrix();
    DirectXCommon::GetInstance()->SetRenderTextureCamera(currentCamera_);
#ifdef _DEVELOP

    if (Input::IsTriggerKey(DIK_F1)) {
        SwitchCamera();
    }

    DebugUI::CheckCamera(*currentCamera_);

    DebugUI::CheckObject3d(*object3d_, "Cylinder");
    DebugUI::CheckObject3d(*object3d2_, "Cube");
    DebugUI::CheckParticle(*particleEmitters_[0], "Emitter0");
    DebugUI::CheckParticle(*particleEmitters_[1], "Emitter1");
    DebugUI::CheckSRVIndex();

#endif //_DEVELOP


    object3d_->Update();
 
    object3d_->GetUVTranslate().x += 0.1f;
    object3d_->UpdateUV();
    object3d2_->Update();

    for (auto& enemy : enemies_) {
        enemy->Update();
    }
    for (auto& obj : objects_) {
        obj->obj_->SetTemperature(1.0f);
        obj->obj_->Update();
    }


    for (int i = 0; i < particleEmitters_.size(); ++i) {
        particleEmitters_[i]->UpdateTimer();
        particleEmitters_[i]->UpdateEmitter();

    }

    // 共通更新
    ParticleManager::GetInstance()->Update(*currentCamera_);




}

void FreeTypeScene::DrawSprite() {

    Sprite::PreDraw();
    text_.Draw();
    pressSpaceText_.Draw();
    sceneChange_->Draw();

};

void FreeTypeScene::DrawModel()
{

#ifdef _DEVELOP
    // デバッグカメラ
    DrawGrid::Draw(*currentCamera_);
#endif //_DEVELOP

    for (auto& enemy : enemies_) {
        enemy->Draw(*currentCamera_);
    }




    //object3d2_->Draw(*currentCamera_);
    for (auto& obj : objects_) {

        obj->obj_->Draw(*currentCamera_);
    }

    skyBoxObj_->Draw(*currentCamera_);

    object3d_->DrawForEffect(*currentCamera_);
    ParticleManager::GetInstance()->Draw();

   
}

void FreeTypeScene::CreateParticle()
{
    for (int i = 0; i < particleEmitters_.size(); ++i) {
        particleEmitters_[i] = std::make_unique<ParticleEmitter>();
        particleEmitters_[i]->Initialize();
    }

    ParticleManager::GetInstance()->Create();

    particleEmitters_[0]->SetName("particle1");

    auto& emitter0 = particleEmitters_[0]->GetEmitter();

    emitter0.count = 8;
    emitter0.color = { 1.0f,1.0f,1.0f,1.0f };
    emitter0.transform.scale_ = { 0.05f,1.0f,1.0f };
    emitter0.transform.rotate_ = { 0.0f,0.0f,0.0f };
    emitter0.transform.translate_ = { 0.5f,0.5f,-0.5f };

    emitter0.frequencyTime = 0.0f;
    emitter0.frequency = 2.0f;
    emitter0.lifeTime = 2.0f;
    emitter0.blendMode = kBlendModeAdd;
    emitter0.movement = ParticleMovements::kParticleNormal;
    emitter0.rotateAABB_ = { .min = {0.0f,0.0f,-3.14f},.max = {0.0f,0.0f,3.14f} };
    emitter0.scaleAABB_ = { .min = {0.0f,0.4f,0.0f},.max = {0.0f,1.5f,1.0f} };

    auto& group = ParticleManager::GetInstance()->GetParticleGroup(emitter0.name);
    group->accelerationField.acceleration.y = 0.0f;
    group->accelerationField.area = { .min = {-1.0f,-1.0f,-1.0f},.max = {1.0f,1.0f,1.0f} };
    group->useBillboard = true;

    particleEmitters_[1]->SetName("ring");

    auto& emitter1 = particleEmitters_[1]->GetEmitter();

    emitter1.count = 1;
    emitter1.color = { 1.0f,1.0f,1.0f,1.0f };
    emitter1.transform.scale_ = { 0.5f,0.5f,0.5f };
    emitter1.transform.rotate_ = { 0.0f,0.0f,0.0f };
    emitter1.transform.translate_ = { 0.5f,0.5f,0.0f };

    emitter1.frequencyTime = 0.0f;
    emitter1.frequency = 2.0f;
    emitter1.lifeTime = 2.0f;
    emitter1.blendMode = kBlendModeAdd;
    emitter1.movement = ParticleMovements::kParticleNormal;
    emitter1.rotateAABB_ = { .min = {0.0f,-3.14f,0.0f},.max = {0.0f,3.14f,0.0f} };
    
    auto& group1 = ParticleManager::GetInstance()->GetParticleGroup(emitter1.name);
    group1->accelerationField.acceleration.y = 0.0f;
    group1->accelerationField.area = { .min = {-1.0f,-1.0f,-1.0f},.max = {1.0f,1.0f,1.0f} };
    group1->useBillboard = false;
}
