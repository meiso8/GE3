#include "PointLightManager.h"
#include"DirectXCommon.h"
#include"CbvSrvUavDescriptorHeap.h"
#include"CommandList.h"
#include"Log.h"

SRVResource<PointLight> PointLightManager::resource_;


void PointLightManager::InitData(const uint32_t& index)
{
    resource_.data[index].color = { 0.0f,0.0f,0.0f,1.0f };
    resource_.data[index].intensity = 1.0f;
    resource_.data[index].position = { 0.0f,0.0f,0.0f };
    resource_.data[index].radius = 3.0f;
    resource_.data[index].decay = 0.5f;


    LogFile::Log("Init PointLightData");
}

void PointLightManager::InitDatas()
{
    for (uint32_t i = 0; i < kMaxData_; ++i) {
        InitData(i);
    }
}

void PointLightManager::Finalize()
{
    resource_.UnMap();
    resource_.Reset();

    LogFile::Log("Finalize PointLightManager");
}

PointLightManager::PointLightManager(CbvSrvUavDescriptorHeap* srvDescriptorHeap)
{
    resource_.CreateBufferResource(L"pointLightResource",sizeof(PointLight) * kMaxData_);
    //書き込むためのアドレスを取得
    resource_.Map();
    resource_.Allocate(srvDescriptorHeap);
    resource_.CreateSRVforStructuredBuffer(srvDescriptorHeap, UINT(kMaxData_), sizeof(PointLight));

    //データを初期化する
    InitDatas();

    LogFile::Log("Create PointLightManager");
}
