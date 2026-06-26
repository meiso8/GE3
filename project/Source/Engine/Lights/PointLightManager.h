#pragma once

#include<wrl.h>
#include<d3d12.h>
#include"Lights/Light.h"
#include<cstdint>
#include<cassert>
class SrvDescriptorHeap;
class PointLightManager
{
public:
    void Finalize();
    PointLightManager()= default;
    PointLightManager(SrvDescriptorHeap* srvDescriptorHeap);
    static uint32_t GetSrvIndex() { return srvIndex_; };
    static void InitData(const uint32_t& index);
    static void InitDatas();
    static PointLight& GetData(const uint32_t& index) { assert(index < kMaxData_); return pointLightData_[index]; };
public:
    static const uint32_t kMaxData_ = 20;
private:
    static Microsoft::WRL::ComPtr <ID3D12Resource> pointLightResource_;
    static PointLight* pointLightData_;

    static uint32_t srvIndex_;
};

