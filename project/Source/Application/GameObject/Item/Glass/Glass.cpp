#include "Glass.h"
#include"TimeManager.h"

void Glass::Init()
{
    isAnimEnd_ = false;
    isGet_ = false;
    isUsed_ = false;
    isMelt_ = false;

    aniTimer_ = 0.0f;
    object_->Initialize();

    object_->SetTranslate({ 8.5f,1.111f,-5.0f });
    object_->SetTemperature(0.2f);

    auto& material = object_->GetMaterial();
    material.refraction = 0.667f;
    material.glassFactor = 1.0f;
    material.lightMode = Object3d::LightMode::kLightModeLReflectance;

    auto& melt = object_->GetMeltData();
    melt.size = 0.4f;
    melt.thickness = 0.0f;
    melt.time = 0.0f;

    startPos_ = { 0.0f };
    endPos_ = { 0.0f };

    screenEndSize_ = 0.03125f * 0.75f;
}

void Glass::Update()
{

    if (isUsed_) {

        //溶ける
        auto& melt = object_->GetMeltData();
        melt.time += TimeManager::DeltaTime();
        melt.time = std::clamp(melt.time, 0.0f, 20.0f);

        if (melt.time >= 7.5f) {
            isMelt_ = true;
        }

        float time = melt.time / 20.0f;
        time = std::min(time, 1.0f);

        //温度の設定
        object_->SetTemperature(time);

        //アニメーションタイマー
        UpdateAniTimer(2.0f);
        float localTimer = aniTimer_ * 0.5f;
        //トランスフォーム
        object_->SetRotate({ 0.0f,0.0f,0.0f });
        object_->SetTranslate(Lerp(startPos_, endPos_, localTimer));
    }

    //色
    object_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
    object_->Update();


}

void Glass::Use()
{    //アニメーションタイマーをリセットする
    aniTimer_ = 0.0f;
    isUsed_ = true;
    isAnimEnd_ = false;
    isGet_ = false;

}
