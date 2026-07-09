#pragma once
#include<d3d12.h>
#include<wrl.h>
#include"Lights/Light.h"
#include<stdint.h>
#include"../ResourceManager/ResourceManager.h"
class CommandList;
class CbvSrvUavDescriptorHeap;
class SpotLightManager
{
public:
     void Finalize();
     SpotLightManager() = default;
    SpotLightManager(CbvSrvUavDescriptorHeap* srvDescriptorHeap);
    static void InitData(const uint32_t index);
    static void InitDatas();
    static SpotLight& GetData(const uint32_t& index) { return resource_.data[index]; }
    static uint32_t GetSrvIndex() { return resource_.srvIndex; }
public:
    static const uint32_t kMaxData_ = 20;
private:
    static SRVResource<SpotLight> resource_;
};

