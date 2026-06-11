#pragma once
#include<d3d12.h>
#include<wrl.h>
#include"Lights/Light.h"

class DirectionalLightManager
{
public:
    static void Create();
    static void Finalize();
    static void SetGraphicsRootConstantBufferView(const UINT rootParameterIndex);
    static DirectionalLight* GetDirectionalLightData() { return directionalLightData; }
private:
    static Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightResource;
    static DirectionalLight* directionalLightData;
};

