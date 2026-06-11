#include "SpotLightManager.h"
#include"DirectXCommon.h"
#include"SRVmanager/SrvManager.h"
#include<numbers>
#include"DebugUI.h"

SpotLight* SpotLightManager::spotLightData_ = nullptr;
Microsoft::WRL::ComPtr <ID3D12Resource> SpotLightManager::spotLightResource_ = nullptr;
uint32_t SpotLightManager::srvIndex_;

void SpotLightManager::Create()
{
    //スポットライトのResourceを作成する
    spotLightResource_ =
        DirectXCommon::CreateBufferResource(sizeof(SpotLight) * kMaxData_);
    //書き込むためのアドレスを取得
    spotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData_));

    //srvIndexを取得する
    srvIndex_ = SrvManager::Allocate();
    SrvManager::CreateSRVforStructuredBuffer(srvIndex_, spotLightResource_.Get(), UINT(kMaxData_), sizeof(SpotLight));

    InitDatas();
}

void SpotLightManager::Finalize()
{
    if (spotLightResource_ != nullptr) {
        spotLightResource_->Unmap(0, nullptr);
        spotLightResource_.Reset();
        spotLightResource_ = nullptr;
    }

}

void SpotLightManager::SetGraphicsRootDescriptorTable(const UINT rootParameterIndex)
{
    //SpotLightのDescriptorTableの設定をする
    SrvManager::SetGraphicsRootDescriptorTable(rootParameterIndex, srvIndex_);
}

void SpotLightManager::InitData(const uint32_t& index)
{
    //デフォルト値はとりあえず以下のようにしておく   
    spotLightData_[index].color = {0.0f,0.0f,0.0f,1.0f };
    spotLightData_[index].position = { 0.0f,0.0f,0.0f };//位置
    spotLightData_[index].intensity = { 0.0f };//輝度
    spotLightData_[index].direction = { 0.0f,0.0f,1.0f };//スポットライトの方向
    spotLightData_[index].distance = { 1.0f };//ライトの届く範囲
    spotLightData_[index].decay = 0.1f;
    spotLightData_[index].cosAngle = cosf(std::numbers::pi_v<float> / 8.0f);//スポットライトの余弦
    spotLightData_[index].cosFalloffStart = 2.0f;
}

void SpotLightManager::InitDatas()
{
    for (uint32_t i = 0; i < kMaxData_; ++i) {
        InitData(i);
    }
}
