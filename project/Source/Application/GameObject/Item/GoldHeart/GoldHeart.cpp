#include "GoldHeart.h"
#include"Lerp.h"
#include"DebugUI.h"
void GoldHeart::Init()
{
    isAnimEnd_ = false;
    isUsed_ = false;
    isGet_ = false;
    aniTimer_ = 0.0f;
    object_->Initialize();
    //鏡面反射する
    object_->SetEnvironmentCoefficient(1.0f);
    auto& transform = object_->GetTransform();
    transform.translate.y = -1.2f;
    //熱々
    object_->SetTemperature(1.0f);
    startPos_ = { 0.0f };
    endPos_ = { 0.0f };
}

void GoldHeart::Update()
{
    if (isUsed_) {
        UpdateAniTimer(2.0f);
        float localTimer = aniTimer_ * 0.5f;
        object_->SetTranslate(Lerp(startPos_, endPos_, localTimer));
    }

    object_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
    object_->Update();
}

void GoldHeart::Use()
{
    //アニメーションタイマーをリセットする
    aniTimer_ = 0.0f;
    isUsed_ = true;

    //ミイラにはめ込む
}
