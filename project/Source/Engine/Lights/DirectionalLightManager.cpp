#include "DirectionalLightManager.h"
#include"DirectXCommon.h"

DirectionalLight* DirectionalLightManager::directionalLightData = nullptr;
Microsoft::WRL::ComPtr <ID3D12Resource> DirectionalLightManager::directionalLightResource = nullptr;

void DirectionalLightManager::Create()
{
    //平行光源用のResourceを作成する
    directionalLightResource = DirectXCommon::CreateBufferResource(sizeof(DirectionalLight));
    //書き込むためのアドレスを取得
    directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
    //デフォルト値はとりあえず以下のようにしておく   
    directionalLightData->color = { 1.0f,230.0f / 255.0f,200.0f / 255.0f,1.0f };

    directionalLightData->direction = { 0.0f,-1.0f,0.0f };//向きは正規化する
    directionalLightData->intensity = 1.0f;
    //書き込み終了！
    directionalLightResource->Unmap(0, nullptr);
}

void DirectionalLightManager::Finalize()
{
    if (directionalLightResource) {
        directionalLightResource.Reset();
    }

}

void DirectionalLightManager::SetGraphicsRootConstantBufferView(const UINT rootParameterIndex)
{
    //LightのCBufferの場所を設定
    DirectXCommon::GetCommandList()->SetGraphicsRootConstantBufferView(rootParameterIndex, directionalLightResource->GetGPUVirtualAddress());

}
