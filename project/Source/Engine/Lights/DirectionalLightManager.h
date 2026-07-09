#pragma once
#include<d3d12.h>
#include<wrl.h>
#include"Lights/Light.h"
#include"ResourceManager/ResourceManager.h"

class CommandList;
class DirectionalLightManager
{
public:
    DirectionalLightManager();
    void Finalize();
    static void SetGraphicsRootConstantBufferView(const UINT rootParameterIndex, ID3D12GraphicsCommandList* commandList);
    static DirectionalLight* GetDirectionalLightData() { return directionalLightResource.data; }
private:
    static Resource<DirectionalLight> directionalLightResource;
};

