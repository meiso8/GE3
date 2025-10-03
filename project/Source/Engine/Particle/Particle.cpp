#include "Particle.h"
#include"Multiply.h"
#include"CreateBufferResource.h"
#include"Camera/Camera.h"
#include"MakeIdentity4x4.h"
#include"GetCPUDescriptorHandle.h"
#include"GetGPUDescriptorHandle.h"
#include"MyEngine.h"

using namespace  Microsoft::WRL;

void Particle::Create(const ComPtr<ID3D12Device>& device, CommandList& commandList, RootSignature& rootSignature)
{
    commandList_ = &commandList;
    rootSignature_ = &rootSignature;
    CreateModelData(device);
    CreateTransformationMatrix(device);

    //マテリアルリソースを作成 //ライトなし
    materialResource_.CreateMaterial(device, MaterialResource::LIGHTTYPE::NONE);

}


void Particle::CreateModelData(const ComPtr<ID3D12Device>& device)
{
    modelData_.vertices.push_back({ .position = {1.0f,1.0f,0.0f,1.0f},.texcoord = {0.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });//左上
    modelData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f}, .texcoord = {1.0f,0.0f}, .normal = {0.0f,0.0f,1.0f} });//右上
    modelData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f}, .texcoord = {0.0f,1.0f}, .normal = {0.0f,0.0f,1.0f} });//左下
    modelData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f}, .texcoord = {0.0f,1.0f}, .normal = {0.0f,0.0f,1.0f} });//左下
    modelData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f}, .texcoord = {1.0f,0.0f}, .normal = {0.0f,0.0f,1.0f} });//右上
    modelData_.vertices.push_back({ .position = {-1.0f,-1.0f,0.0f,1.0f}, .texcoord = {1.0f,1.0f}, .normal = {0.0f,0.0f,1.0f} });//右下
    modelData_.material.textureFilePath = "./resources/uvChecker.png";

}

void Particle::CreateTransformationMatrix(const ComPtr<ID3D12Device>& device)
{

    transforms.resize(kNumInstance);

    //Instancing用のTransformationMatrixリソースを作成
    instancingResource = CreateBufferResource(device, sizeof(TransformationMatrix) * kNumInstance);
    //書き込むためのアドレスを取得
    instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));

    for (uint32_t index = 0; index < kNumInstance; ++index) {
        instancingData[index].WVP = MakeIdentity4x4();
        instancingData[index].World = MakeIdentity4x4();
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
    instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
    instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    instancingSrvDesc.Buffer.FirstElement = 0;
    instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    instancingSrvDesc.Buffer.NumElements = kNumInstance;
    instancingSrvDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);
    instancingSrvHandleCPU = GetCPUDescriptorHandle(MyEngine::GetInstance()->GetSrvDescriptorHeap().Get(), MyEngine::descriptorSizeSRV, 4);
    instancingSrvHandleGPU = GetGPUDescriptorHandle(MyEngine::GetInstance()->GetSrvDescriptorHeap().Get(), MyEngine::descriptorSizeSRV, 4);
    device->CreateShaderResourceView(instancingResource.Get(), &instancingSrvDesc, instancingSrvHandleCPU);

    for (uint32_t index = 0; index < kNumInstance; ++index) {
        transforms[index].scale = { 1.0f,1.0f,1.0f };
        transforms[index].rotate = { 0.0f,0.0f,0.0f };
        transforms[index].translate = { index * 0.1f,index * 0.1f,index * 0.1f };
    }

}


void Particle::Draw(Camera& camera)
{

    for (uint32_t index = 0; index < kNumInstance; ++index) {
        Matrix4x4 worldMatrix = MakeAffineMatrix(transforms[index].scale, transforms[index].rotate, transforms[index].translate);
        Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, camera.GetViewProjectionMatrix());
        instancingData[index].WVP = worldViewProjectionMatrix;
    }

    //rootSignatureの設定
    commandList_->GetComandList()->SetGraphicsRootSignature(rootSignature_->GetRootSignature(1).Get());
    //マテリアルの設定
    commandList_->GetComandList()->SetGraphicsRootConstantBufferView(0, materialResource_.GetMaterialResource()->GetGPUVirtualAddress());
    //粒ごとのトランスフォーム
    commandList_->GetComandList()->SetGraphicsRootConstantBufferView(1, instancingResource->GetGPUVirtualAddress());

    //テスクチャ
    commandList_->GetComandList()->SetGraphicsRootDescriptorTable(2, instancingSrvHandleGPU);

    //描画!（DrawCall/ドローコール）6個のインデックスを使用しインスタンスを描画。
    commandList_->GetComandList()->DrawIndexedInstanced(UINT(modelData_.vertices.size()), kNumInstance, 0, 0, 0);

}
