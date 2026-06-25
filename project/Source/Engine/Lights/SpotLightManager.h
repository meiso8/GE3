#pragma once
#include<d3d12.h>
#include<wrl.h>
#include"Lights/Light.h"
#include<stdint.h>
class CommandList;
class SpotLightManager
{
public:
     void Finalize();
     SpotLightManager();
    static void SetGraphicsRootDescriptorTable(const UINT rootParameterIndex, ID3D12GraphicsCommandList* commandList);
    static void InitData(const uint32_t index);
    static void InitDatas();
    static SpotLight& GetData(const uint32_t& index) { return spotLightData_[index]; }
   
public:
    static const uint32_t kMaxData_ = 20;
private:
    static Microsoft::WRL::ComPtr <ID3D12Resource> spotLightResource_;
    static SpotLight* spotLightData_;

    static uint32_t srvIndex_;
};

