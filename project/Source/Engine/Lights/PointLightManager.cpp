#include "PointLightManager.h"
#include"DirectXCommon.h"
#include"SrvDescriptorHeap.h"
#include"CommandList.h"
#include"Log.h"

Microsoft::WRL::ComPtr <ID3D12Resource> PointLightManager::pointLightResource_;
PointLight* PointLightManager::pointLightData_;
uint32_t PointLightManager::srvIndex_;

void PointLightManager::InitData(const uint32_t& index)
{
    pointLightData_[index].color = { 0.0f,0.0f,0.0f,1.0f };
    pointLightData_[index].intensity = 1.0f;
    pointLightData_[index].position = { 0.0f,0.0f,0.0f };
    pointLightData_[index].radius = 3.0f;
    pointLightData_[index].decay = 0.5f;


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
    if (pointLightResource_) {
        pointLightResource_.Reset();
    }

    LogFile::Log("Finalize PointLightManager");
}

PointLightManager::PointLightManager(SrvDescriptorHeap* srvDescriptorHeap)
{
    pointLightResource_ =
        DirectXCommon::CreateBufferResource(sizeof(PointLight) * kMaxData_);
    //書き込むためのアドレスを取得
    pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));

    srvIndex_ = srvDescriptorHeap->Allocate();
    srvDescriptorHeap->CreateSRVforStructuredBuffer(srvIndex_, pointLightResource_.Get(), UINT(kMaxData_), sizeof(PointLight));

    //データを初期化する
    InitDatas();

    LogFile::Log("Create PointLightManager");
}
