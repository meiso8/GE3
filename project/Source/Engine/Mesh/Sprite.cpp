#include "Sprite.h"
#include"CreateBufferResource.h"
#include"TransformationMatrix.h"
#include"MakeAffineMatrix.h"
#include"MakeIdentity4x4.h"
#include"Multiply.h"

void Sprite::Create(
    const Microsoft::WRL::ComPtr<ID3D12Device>& device, ModelConfig& mc
) {

    CreateVertex(device);
    CreateIndexResource(device);
    CreateTransformationMatrix(device);
    CreateMaterial(device);

    uvTransform_ = {
          {1.0f,1.0f,1.0f},
          {0.0f,0.0f,0.0f},
          {0.0f,0.0f,0.0f},
    };

    uvTransformMatrix_ = MakeIdentity4x4();

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

    size_ = { 640.0f,360.0f };

}

void Sprite::CreateVertex(const Microsoft::WRL::ComPtr<ID3D12Device>& device) {

    //VertexResourceとVertexBufferViewを用意 矩形を表現するための三角形を二つ(頂点4つ)
    vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * 4);

    //頂点バッファビューを作成する
    //リソースの先頭アドレスから使う
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    //使用するリソースのサイズ頂点4つ分のサイズ
    vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
    //1頂点あたりのサイズ
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

#pragma region //Sprite用の頂点データの設定

    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
    //1枚目の三角形 四頂点でスプライト描画が完成

    vertexData_[0].position = { 0.0f,size_.y,0.0f,1.0f };//左下
    vertexData_[0].texcoord = { 0.0f,1.0f };
    vertexData_[0].normal = { 0.0f,0.0f,-1.0f };//法線

    vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
    vertexData_[1].texcoord = { 0.0f,0.0f };
    vertexData_[1].normal = { 0.0f,0.0f,-1.0f };

    vertexData_[2].position = { size_.x,size_.y,0.0f,1.0f };//右下
    vertexData_[2].texcoord = { 1.0f,1.0f };
    vertexData_[2].normal = { 0.0f,0.0f,-1.0f };

    vertexData_[3].position = { size_.x,0.0f,0.0f,1.0f };//右上
    vertexData_[3].texcoord = { 1.0f,0.0f };
    vertexData_[3].normal = { 0.0f,0.0f,-1.0f };

#pragma endregion

}

void Sprite::CreateIndexResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device) {

#pragma region//IndexResourceを作成
    indexResource_ = CreateBufferResource(device, sizeof(uint32_t) * 6);
    //Viewを作成する IndexBufferView(IBV)

    //リソースの先頭アドレスから使う
    indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
    //使用するリソースのサイズはインデックス6つ分のサイズ
    indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
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

#pragma endregion
}

void Sprite::CreateTransformationMatrix(const Microsoft::WRL::ComPtr<ID3D12Device>& device) {

    //Matrix4x4　1つ分のサイズを用意
    transformationMatrixResource_ = CreateBufferResource(device, sizeof(TransformationMatrix));
    //データを書き込む
    //書き込むためのアドレスを取得
    transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));

    transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
    worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

}

void Sprite::CreateMaterial(const Microsoft::WRL::ComPtr<ID3D12Device>& device) {

    //マテリアルリソースを作成 //ライトなし
    materialResource_.CreateMaterial(device, MaterialResource::LIGHTTYPE::NONE);

}

void Sprite::SetSize(const Vector2& size) {
    size_ = size;
    vertexData_[0].position = { 0.0f,size_.y,0.0f,1.0f };//左下
    vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
    vertexData_[2].position = { size_.x,size_.y,0.0f,1.0f };//右下
    vertexData_[3].position = { size_.x,0.0f,0.0f,1.0f };//右上
}

void Sprite::SetColor(const Vector4& color) {
    materialResource_.SetColor(color);
}

void Sprite::UpdateUV() {

    uvTransformMatrix_ = MakeAffineMatrix(uvTransform_.scale, uvTransform_.rotate, uvTransform_.translate);
    materialResource_.SetUV(uvTransformMatrix_);
}

void Sprite::PreDraw() {
    modelConfig_.commandList->GetComandList()->RSSetViewports(1, modelConfig_.viewport);//Viewportを設定
    modelConfig_.commandList->GetComandList()->RSSetScissorRects(1, modelConfig_.scissorRect);//Scirssorを設定
    //RootSignatureを設定。PSOに設定しているけど別途設定が必要
    modelConfig_.commandList->GetComandList()->SetGraphicsRootSignature(modelConfig_.rootSignature->GetRootSignature(0).Get());
    modelConfig_.commandList->GetComandList()->SetPipelineState(modelConfig_.pso->GetGraphicsPipelineState(PSO::TRIANGLE).Get());//PSOを設定
    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    modelConfig_.commandList->GetComandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Sprite::Draw(
    ShaderResourceView& srv, Camera& camera, uint32_t lightType
) {

    materialResource_.SetLightType(lightType);

    worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    worldViewProjectionMatrix_ = Multiply(worldMatrix_, camera.GetViewProjectionMatrix());
    *transformationMatrixData_ = { worldViewProjectionMatrix_,worldMatrix_ };

    //頂点バッファビューを設定
    modelConfig_.commandList->GetComandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
    //IBVを設定new
    modelConfig_.commandList->GetComandList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定
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

    //描画!（DrawCall/ドローコール）6個のインデックスを使用し1つのインスタンスを描画。その他は当面0で良い。
    modelConfig_.commandList->GetComandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
};

