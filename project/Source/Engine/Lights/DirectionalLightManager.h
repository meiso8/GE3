#pragma once
#include<d3d12.h>
#include<wrl.h>
#include"Lights/Light.h"
class CommandList;
class DirectionalLightManager
{
public:
    DirectionalLightManager();
    void Finalize();
    static void SetGraphicsRootConstantBufferView(const UINT rootParameterIndex, ID3D12GraphicsCommandList* commandList);
    static DirectionalLight* GetDirectionalLightData() { return directionalLightData; }
private:
    static Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightResource;
    static DirectionalLight* directionalLightData;
};

