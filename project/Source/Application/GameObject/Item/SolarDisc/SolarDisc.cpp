#include "SolarDisc.h"


void SolarDisc::Init()
{
    isGetAnimEnd_ = false;
    isGet_ = false;
    isUsed_ = false;
    aniTimer_ = 0.0f;
    object_->Initialize();
    object_->SetTranslate({-23.0f,0.0f,-23.0f});
    object_->SetTemperature(0.5f);
    startPos_ = { 0.0f };
    endPos_ = { 0.0f };
}

void SolarDisc::Use()
{
    aniTimer_ = 0.0f;
    isUsed_ = true;
}
