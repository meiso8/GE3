#pragma once
#include"WorldTransform.h"
class LightingManager
{
public:
    void Initialize();
    void UpdatePointLight();
    void DirectionalLightUpdate();
    WorldTransform& GetPlayerHandPos() { return playerHandPos_; }
    void SetDirection(Vector3* direction) { direction_ = direction; }
    static bool GetIsPointLightOn() { return isPointLightOn_; }
    static void setIsPointLightOn(const bool flag) { isPointLightOn_ = flag; }
private:
    WorldTransform playerHandPos_;
    Vector3* direction_;
    static bool isPointLightOn_;
};

