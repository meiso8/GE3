#include"BaseScene.h"

BaseScene::BaseScene()
{
    sceneChange_ = std::make_unique<SceneChange>();
    sceneChange_->Initialize();
    sceneChange_->SetState(SceneChange::kFadeOut, 1.0f);

    camera_ = std::make_unique<Camera>();
}

void BaseScene::Initialize()
{

}

void BaseScene::Update()
{

}


void BaseScene::SceneChangeUpdate()
{

    sceneChange_->Update();
}

void BaseScene::SwitchCamera()
{
    isDebugCameraActive_ = !isDebugCameraActive_;
    currentCamera_ = (isDebugCameraActive_) ? DebugCamera::GetInstance() : camera_.get();
}
