#include "ResultMedjed.h"
#include"ModelManager.h"
#include"Model.h"

ResultMedjed::ResultMedjed()
{
  aniObj_ = std::make_unique<AnimationObject3d>();
  auto* modle = ModelManager::GetModel("medjed.gltf");

  aniObj_ = std::make_unique<AnimationObject3d>();
  aniObj_->Create();
  aniObj_->SetMeshAndMaterial(modle);
  aniObj_->SetTemperature(1.0f);
  aniObj_->SetModelAndLoadAnimation(modle);

  anilists_ = {
  "Jump",
  "Nod",
  "Round",
  "Step",
  "Swing",
  "Walk"
  };
}

void ResultMedjed::Initialize()
{
    preAnimNum_ = 0;
    currentAnimNum_ = 0;
    aniObj_->Initialize();
    aniObj_->SetTranslate({ 0.0f,-3.0f,1.0f });
    aniObj_->SetScale({ 4.0f,4.0f,4.0f });
    aniObj_->SetRotate({ 0.0f,Math::kPi,0.0f });
    aniObj_->SetMaskVol(0.0f);
    aniObj_->SetObjectName("ResultMedjed");
    aniObj_->RegisterObject();
    //アニメーションをセットするよ
    aniObj_->SetAnimation(anilists_[currentAnimNum_]);
}

void ResultMedjed::Update(const bool isResetAnim)
{
    if (isResetAnim) {
        
        while (preAnimNum_ == currentAnimNum_) {
            currentAnimNum_ = rand() % anilists_.size();

        }
        //アニメーションをセットするよ
        aniObj_->SetAnimation(anilists_[currentAnimNum_]);
        preAnimNum_ = currentAnimNum_;
    }

    aniObj_->UpdateAniTimer();
    aniObj_->Update();

}

void ResultMedjed::Draw(Camera& camera)
{
    aniObj_->Draw(camera);

}
