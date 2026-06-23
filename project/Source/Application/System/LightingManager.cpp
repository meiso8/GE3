#include "LightingManager.h"
#include"Lights/DirectionalLightManager.h"
#include"Lights/PointLightManager.h"
#include"Lights/SpotLightManager.h"
#include"TimeManager.h"
#include"DebugUI.h"

bool LightingManager::isPointLightOn_ = false;

void LightingManager::Initialize()
{
    DirectionalLightManager::GetDirectionalLightData()->direction = { 0.0f,-1.0f,0.0f };
    DirectionalLightManager::GetDirectionalLightData()->intensity = 0.0f;

    PointLightManager::GetData(0).color = { 160.0f / 255.0f,1.0f,1.0f,1.0f };
    PointLightManager::GetData(0).intensity = 1.0f;
    PointLightManager::GetData(0).position = { 0.0f,15.0f,0.0f };
    PointLightManager::GetData(0).radius = 30.0f;
    PointLightManager::GetData(0).decay = 0.3f;

    SpotLightManager::GetData(0).color = { 1.0f,1.0f,1.0f,1.0f };
     SpotLightManager::GetData(0).position = { 0.0f,0.0f,0.0f };//位置
     SpotLightManager::GetData(0).intensity = { 25.0f };//輝度
     SpotLightManager::GetData(0).direction = { 0.0f,0.0f,1.0f };//スポットライトの方向
     SpotLightManager::GetData(0).distance = 0.1f;
     SpotLightManager::GetData(0).decay = 0.5f;
     SpotLightManager::GetData(0).cosAngle = cosf(std::numbers::pi_v<float> / 8.0f);//スポットライトの余弦
     SpotLightManager::GetData(0).cosFalloffStart = 2.0f;

    playerHandPos_.Initialize();
    isPointLightOn_ = true;
}

void LightingManager::UpdatePointLight()
{

    if (isPointLightOn_) {
        WorldTransformUpdate(playerHandPos_);
        SpotLightManager::GetData(0).color = { 1.0f,1.0f,1.0f,1.0f };
        SpotLightManager::GetData(0).position = playerHandPos_.GetWorldPosition();
        SpotLightManager::GetData(0).direction = *direction_;
    } else {                      
        SpotLightManager::GetData(0).color = { 0.0f,0.0f,0.0f,0.0f };
    }

}

void LightingManager::DirectionalLightUpdate()
{
    if (DirectionalLightManager::GetDirectionalLightData()->intensity < 1.0f) {
        DirectionalLightManager::GetDirectionalLightData()->intensity += TimeManager::DeltaTime() * 2.0f;
    } else {
        DirectionalLightManager::GetDirectionalLightData()->intensity = 1.0f;
        isPointLightOn_ = false;
        PointLightManager::InitData(0);
    }

}

