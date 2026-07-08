#include "SpotLightManager.h"
#include"DirectXCommon.h"
#include"SrvDescriptorHeap.h"
#include"Log.h"
#include"Transform.h"
SpotLight* SpotLightManager::spotLightData_ = nullptr;
Microsoft::WRL::ComPtr <ID3D12Resource> SpotLightManager::spotLightResource_ = nullptr;
uint32_t SpotLightManager::srvIndex_;

void SpotLightManager::Finalize()
{
    if (spotLightResource_) {
        spotLightResource_.Reset();
    }
    LogFile::Log("Finalize  SpotLightManager");
}

SpotLightManager::SpotLightManager(CbvSrvUavDescriptorHeap *srvDescriptorHeap)

{    //スポットライトのResourceを作成する
    spotLightResource_ =
        DirectXCommon::CreateBufferResource(sizeof(SpotLight) * kMaxData_);
    spotLightResource_->SetName(L"spotLightResource");
    //書き込むためのアドレスを取得
    spotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData_));

    //srvIndexを取得する
    srvIndex_ = srvDescriptorHeap->Allocate();
    srvDescriptorHeap->CreateSRVforStructuredBuffer(srvIndex_, spotLightResource_.Get(), UINT(kMaxData_), sizeof(SpotLight));

    InitDatas();

    LogFile::Log("Create  SpotLightManager");
}

void SpotLightManager::InitData(const uint32_t index)
{
    //デフォルト値はとりあえず以下のようにしておく   
    spotLightData_[index].color = {0.0f,0.0f,0.0f,1.0f };
    spotLightData_[index].position = { 0.0f,0.0f,0.0f };//位置
    spotLightData_[index].intensity = { 0.0f };//輝度
    spotLightData_[index].direction = { 0.0f,0.0f,1.0f };//スポットライトの方向
    spotLightData_[index].distance = { 1.0f };//ライトの届く範囲
    spotLightData_[index].decay = 0.1f;
    spotLightData_[index].cosAngle = cosf(Math::kPi / 8.0f);//スポットライトの余弦
    spotLightData_[index].cosFalloffStart = 2.0f;

    LogFile::Log("Init SpotLight Data");
}

void SpotLightManager::InitDatas()
{
    for (uint32_t i = 0; i < kMaxData_; ++i) {
        InitData(i);
    }
}
