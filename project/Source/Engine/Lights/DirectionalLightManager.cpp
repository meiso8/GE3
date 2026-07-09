#include "DirectionalLightManager.h"
#include"DirectXCommon.h"
#include"Log.h"

Resource<DirectionalLight> DirectionalLightManager::directionalLightResource;

DirectionalLightManager::DirectionalLightManager()
{    //平行光源用のResourceを作成する
    directionalLightResource.CreateBufferResource(L"Directional_Light_Resource");

    //書き込むためのアドレスを取得
    directionalLightResource.Map();

    //デフォルト値はとりあえず以下のようにしておく   
    directionalLightResource.data->color = { 1.0f,230.0f / 255.0f,200.0f / 255.0f,1.0f };

    directionalLightResource.data->direction = { 0.0f,-1.0f,0.0f };//向きは正規化する
    directionalLightResource.data->intensity = 1.0f;
    //書き込み終了！
    directionalLightResource.UnMap();

    LogFile::Log("Create DirectionalLightManage");
}

void DirectionalLightManager::Finalize()
{

    directionalLightResource.Reset();
    LogFile::Log("Finalize DirectionalLightManage");
}

void DirectionalLightManager::SetGraphicsRootConstantBufferView(const UINT rootParameterIndex, ID3D12GraphicsCommandList* commandList)
{
    //LightのCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, directionalLightResource.Get()->GetGPUVirtualAddress());

}
