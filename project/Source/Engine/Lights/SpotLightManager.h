#pragma once
#include<d3d12.h>
#include<wrl.h>
#include"Lights/Light.h"
#include<stdint.h>
class CommandList;
class SrvDescriptorHeap;
class SpotLightManager
{
public:
     void Finalize();
     SpotLightManager() = default;
    SpotLightManager(SrvDescriptorHeap* srvDescriptorHeap);
    static void InitData(const uint32_t index);
    static void InitDatas();
    static SpotLight& GetData(const uint32_t& index) { return spotLightData_[index]; }
    static uint32_t GetSrvIndex() { return srvIndex_; }
public:
    static const uint32_t kMaxData_ = 20;
private:
    static Microsoft::WRL::ComPtr <ID3D12Resource> spotLightResource_;
    static SpotLight* spotLightData_;

    static uint32_t srvIndex_;
};

