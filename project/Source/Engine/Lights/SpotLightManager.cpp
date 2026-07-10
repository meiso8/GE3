#include "SpotLightManager.h"
#include"DirectXCommon.h"

#include"Log.h"
#include"Transform.h"

SRVResource<SpotLight> SpotLightManager::resource_;

void SpotLightManager::Finalize()
{
    resource_.UnMap();
    resource_.Reset();

    LogFile::Log("Finalize  SpotLightManager");
}

SpotLightManager::SpotLightManager(CbvSrvUavDescriptorHeap* srvDescriptorHeap)

{    //スポットライトのResourceを作成する
    resource_.CreateBufferResource(L"spotLightResource", sizeof(SpotLight) * kMaxData_);
    //書き込むためのアドレスを取得
    resource_.Map();
    //srvIndexを取得する
    resource_.Allocate(srvDescriptorHeap);
    //構造バッファーの作成
    resource_.CreateSRVforStructuredBuffer(srvDescriptorHeap,UINT(kMaxData_), sizeof(SpotLight));

    InitDatas();

    LogFile::Log("Create  SpotLightManager");
}

void SpotLightManager::InitData(const uint32_t index)
{
    //デフォルト値はとりあえず以下のようにしておく   
    resource_.data[index].color = { 0.0f,0.0f,0.0f,1.0f };
    resource_.data[index].position = { 0.0f,0.0f,0.0f };//位置
    resource_.data[index].intensity = { 0.0f };//輝度
    resource_.data[index].direction = { 0.0f,0.0f,1.0f };//スポットライトの方向
    resource_.data[index].distance = { 1.0f };//ライトの届く範囲
    resource_.data[index].decay = 0.1f;
    resource_.data[index].cosAngle = cosf(Math::kPi / 8.0f);//スポットライトの余弦
    resource_.data[index].cosFalloffStart = 2.0f;

    LogFile::Log("Init SpotLight Data");
}

void SpotLightManager::InitDatas()
{
    for (uint32_t i = 0; i < kMaxData_; ++i) {
        InitData(i);
    }
}
