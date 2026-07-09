#pragma once
#include"Lights/Light.h"
#include<cstdint>
#include<cassert>
#include"../ResourceManager/ResourceManager.h"

class CbvSrvUavDescriptorHeap;
class PointLightManager
{
public:
    void Finalize();
    PointLightManager()= default;
    PointLightManager(CbvSrvUavDescriptorHeap* srvDescriptorHeap);
    static uint32_t GetSrvIndex() { return resource_.srvIndex; };
    static void InitData(const uint32_t& index);
    static void InitDatas();
    static PointLight& GetData(const uint32_t& index) { assert(index < kMaxData_); return resource_.data[index]; };
public:
    static const uint32_t kMaxData_ = 20;
private:
    static SRVResource<PointLight> resource_;

};

