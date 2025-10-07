#include "Particle.h"
#include"Multiply.h"
#include"DirectXCommon.h"
#include"Camera/Camera.h"
#include"MakeIdentity4x4.h"

#include"MyEngine.h"


using namespace  Microsoft::WRL;

void Particle::Create()
{
    rootSignature_ = MyEngine::GetRootSignature();


    CreateModelData();
    CreateTransformationMatrix();

    //マテリアルリソースを作成 //ライトなし
    materialResource_.CreateMaterial(MaterialResource::LIGHTTYPE::NONE);

    vertexBufferResource_ = DirectXCommon::CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());
    vertexBufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexBufferData_));
    std::copy(modelData_.vertices.begin(), modelData_.vertices.end(), vertexBufferData_);

    vertexBufferView_.BufferLocation = vertexBufferResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

}


void Particle::CreateModelData()
{
    modelData_.vertices.push_back({ .position = {1.0f,1.0f,0.0f,1.0f},.texcoord = {0.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });//左上
    modelData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f}, .texcoord = {1.0f,0.0f}, .normal = {0.0f,0.0f,1.0f} });//右上
    modelData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f}, .texcoord = {0.0f,1.0f}, .normal = {0.0f,0.0f,1.0f} });//左下
    modelData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f}, .texcoord = {0.0f,1.0f}, .normal = {0.0f,0.0f,1.0f} });//左下
    modelData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f}, .texcoord = {1.0f,0.0f}, .normal = {0.0f,0.0f,1.0f} });//右上
    modelData_.vertices.push_back({ .position = {-1.0f,-1.0f,0.0f,1.0f}, .texcoord = {1.0f,1.0f}, .normal = {0.0f,0.0f,1.0f} });//右下
    modelData_.material.textureFilePath = "./resources/uvChecker.png";

}

void Particle::CreateTransformationMatrix()
{

    transforms.resize(kNumInstance);

    //Instancing用のTransformationMatrixリソースを作成
    instancingResource = DirectXCommon::CreateBufferResource(sizeof(TransformationMatrix) * kNumInstance);
    //書き込むためのアドレスを取得
    instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));

    assert(instancingResource != nullptr);

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
    instancingSrvHandleCPU = DirectXCommon::GetSRVCPUDescriptorHandle(4);
    instancingSrvHandleGPU = DirectXCommon::GetSRVGPUDescriptorHandle(4);
    DirectXCommon::GetDevice()->CreateShaderResourceView(instancingResource.Get(), &instancingSrvDesc, instancingSrvHandleCPU);

    for (uint32_t index = 0; index < kNumInstance; ++index) {
        transforms[index].scale = { 1.0f,1.0f,1.0f };
        transforms[index].rotate = { 0.0f,0.0f,0.0f };
        transforms[index].translate = { index * 0.1f,index * 0.1f,index * 0.1f };
    }

}


void Particle::Draw(Camera& camera,ShaderResourceView& srv,BlendMode blendMode)
{

    for (uint32_t index = 0; index < kNumInstance; ++index) {
        Matrix4x4 worldMatrix = MakeAffineMatrix(transforms[index].scale, transforms[index].rotate, transforms[index].translate);
        Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, camera.GetViewProjectionMatrix());
        instancingData[index].WVP = worldViewProjectionMatrix;
    }

    ID3D12GraphicsCommandList* commandList = DirectXCommon::GetCommandList();
    PSO* pso = MyEngine::GetPSO(blendMode);

    //rootSignatureの設定
    commandList->SetGraphicsRootSignature(rootSignature_->GetRootSignature(1));
    commandList->SetPipelineState(pso->GetGraphicsPipelineState(PSO::PARTICLE).Get());
    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
    //マテリアルの設定
    commandList->SetGraphicsRootConstantBufferView(0, materialResource_.GetMaterialResource()->GetGPUVirtualAddress());
    //粒ごとのトランスフォーム
    commandList->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU);
    //テスクチャ
    commandList->SetGraphicsRootDescriptorTable(2, srv.GetTextureSrvHandleGPU());

    //描画!（DrawCall/ドローコール）6個のインデックスを使用しインスタンスを描画。
    commandList->DrawInstanced(UINT(modelData_.vertices.size()), kNumInstance, 0, 0);

}
