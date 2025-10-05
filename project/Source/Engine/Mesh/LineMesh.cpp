#include "LineMesh.h"
#include"CreateBufferResource.h"

#include"MakeAffineMatrix.h"
#include"Multiply.h"

void LineMesh::Create(
    const Microsoft::WRL::ComPtr<ID3D12Device>& device, ModelConfig& mc, PSO& pso
) {

    pso_ = &pso;
    CreateVertex(device);
    CreateTransformationMatrix(device);
    CreateMaterial(device);

#pragma region//time

    int waveCount = 2;

    waveResource_ = CreateBufferResource(device, sizeof(Wave) * waveCount);

    //データを書き込む

    //書き込むためのアドレスを取得
    waveResource_->Map(0, nullptr, reinterpret_cast<void**>(&waveData));

    waveData[0].direction = { 1.0f,0.0f,0.0f };
    waveData[0].time = 0.0f;
    waveData[0].amplitude = 0.0f;
    waveData[0].frequency = 4;

    waveData[1].direction = { 1.0f,0.0f,0.0f };
    waveData[1].time = 0.0f;
    waveData[1].amplitude = 0.0f;
    waveData[1].frequency = 4;

#pragma endregion

#pragma region//Balloon

    expansionResource_ = CreateBufferResource(device, sizeof(Balloon));

    //書き込むためのアドレスを取得
    expansionResource_->Map(0, nullptr, reinterpret_cast<void**>(&expansionData_));
    //データを書き込む
    expansionData_->expansion = 0.0f;
    expansionData_->sphere = 0.0f;
    expansionData_->cube = 0.0f;
    expansionData_->isSphere = false;

#pragma endregion

    modelConfig_ = mc;

}

void LineMesh::CreateVertex(const Microsoft::WRL::ComPtr<ID3D12Device>& device) {

    //VertexResourceとVertexBufferViewを用意
    vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * 2);

    //頂点バッファビューを作成する
    //リソースの先頭アドレスから使う
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    //使用するリソースのサイズ頂点2つ分のサイズ
    vertexBufferView_.SizeInBytes = sizeof(VertexData) * 2;
    //1頂点あたりのサイズ
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

#pragma region //頂点データの設定

    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

    vertexData_[0].position = { -0.5f,0.0f,0.0f,1.0f };//左下
    vertexData_[0].texcoord = { 0.0f,1.0f };
    vertexData_[0].normal = { vertexData_[0].position.x,  vertexData_[0].position.y,  vertexData_[0].position.z };//法線
    vertexData_[1].position = { 0.5f, 0.0f,0.0f,1.0f };// 右
    vertexData_[1].texcoord = { 0.0f,0.0f };
    vertexData_[1].normal = { vertexData_[1].position.x,  vertexData_[1].position.y,  vertexData_[1].position.z };

#pragma endregion

}

void LineMesh::SetVertexPos(const Vector3& start, const Vector3& end) {
    vertexData_[0].position = { start.x,start.y,start.z,1.0f };//左下
    vertexData_[0].normal = { vertexData_[0].position.x,  vertexData_[0].position.y,  vertexData_[0].position.z };//法線
    vertexData_[1].position = { end.x,end.y,end.z,1.0f };//右下
    vertexData_[1].normal = { vertexData_[1].position.x,  vertexData_[1].position.y,  vertexData_[1].position.z };
};

void LineMesh::CreateIndexResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device) {

#pragma region//IndexResourceを作成
    indexResource_ = CreateBufferResource(device, sizeof(uint32_t) * 12);
    //Viewを作成する IndexBufferView(IBV)

    //リソースの先頭アドレスから使う
    indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
    //使用するリソースのサイズはインデックス6つ分のサイズ
    indexBufferView_.SizeInBytes = sizeof(uint32_t) * 12;
    //インデックスはuint32_tとする
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
#pragma endregion

#pragma region//IndexResourceにデータを書き込む
    //インデックスリーソースにデータを書き込む
    indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

    //頂点数を削減
    indexData_[0] = 0;
    indexData_[1] = 1;
    indexData_[2] = 2;

    indexData_[3] = 1;
    indexData_[4] = 3;
    indexData_[5] = 2;

    indexData_[6] = 2;
    indexData_[7] = 3;
    indexData_[8] = 0;

    indexData_[9] = 3;
    indexData_[10] = 1;
    indexData_[11] = 0;

#pragma endregion
}

void LineMesh::CreateTransformationMatrix(const Microsoft::WRL::ComPtr<ID3D12Device>& device) {

    //Matrix4x4　1つ分のサイズを用意
    transformationMatrixResource_ = CreateBufferResource(device, sizeof(TransformationMatrix));
    //データを書き込む
    //書き込むためのアドレスを取得
    transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));

    transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
    worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

}

void LineMesh::CreateMaterial(const Microsoft::WRL::ComPtr<ID3D12Device>& device) {

    //マテリアルリソースを作成 ライトなし
    materialResource_.CreateMaterial(device, MaterialResource::LIGHTTYPE::NONE);

}

void LineMesh::SetColor(const Vector4& color) {
    materialResource_.SetColor(color);
}

void LineMesh::PreDraw() {
    modelConfig_.commandList->GetComandList()->SetGraphicsRootSignature(modelConfig_.rootSignature->GetRootSignature(0).Get());
    modelConfig_.commandList->GetComandList()->SetPipelineState(pso_->GetGraphicsPipelineState(PSO::LINE).Get());//PSOを設定
    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    modelConfig_.commandList->GetComandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
}

void LineMesh::Draw(
    ShaderResourceView& srv, Camera& camera
) {

    worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    worldViewProjectionMatrix_ = Multiply(worldMatrix_, camera.GetViewProjectionMatrix());
    *transformationMatrixData_ = { worldViewProjectionMatrix_,worldMatrix_ };
    //頂点バッファビューを設定
    modelConfig_.commandList->GetComandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
    //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
    modelConfig_.commandList->GetComandList()->SetGraphicsRootConstantBufferView(0, materialResource_.GetMaterialResource()->GetGPUVirtualAddress());
    //TransformationMatrixCBufferの場所を設定
    modelConfig_.commandList->GetComandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
    //SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    modelConfig_.commandList->GetComandList()->SetGraphicsRootDescriptorTable(2, srv.GetTextureSrvHandleGPU());
    //LightのCBufferの場所を設定
    modelConfig_.commandList->GetComandList()->SetGraphicsRootConstantBufferView(3, modelConfig_.directionalLightResource->GetGPUVirtualAddress());
    //timeのSRVの場所を設定
    modelConfig_.commandList->GetComandList()->SetGraphicsRootShaderResourceView(4, waveResource_->GetGPUVirtualAddress());
    //expansionのCBufferの場所を設定
    modelConfig_.commandList->GetComandList()->SetGraphicsRootConstantBufferView(5, expansionResource_->GetGPUVirtualAddress());

    //描画!（DrawCall/ドローコール）
    modelConfig_.commandList->GetComandList()->DrawInstanced(2, 1, 0, 0);
};

