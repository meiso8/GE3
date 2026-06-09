#include"TimeManager.h"
#include "DirectXCommon.h"

const float Time::DeltaTime()
{
    return DirectXCommon::GetDeltaTime();
}
