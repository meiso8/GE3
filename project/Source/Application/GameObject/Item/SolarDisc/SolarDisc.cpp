#include "SolarDisc.h"


void SolarDisc::Init()
{
    isAnimEnd_ = false;
    isGet_ = false;
    isUsed_ = false;
    aniTimer_ = 0.0f;
    object_->Initialize();
    object_->SetTranslate({-24.0f,0.1f,-24.0f});
    object_->SetTemperature(0.5f);
    startPos_ = { 0.0f };
    endPos_ = { 0.0f };
}

void SolarDisc::Use()
{
    aniTimer_ = 0.0f;
    isUsed_ = true;
}
