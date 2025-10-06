#include "SphereMesh.h"
#include"DirectXCommon.h"
#include"Texture.h"
#include"TransformationMatrix.h"
#include"MakeAffineMatrix.h"
#include"Multiply.h"
#include"Transform.h"
#include"MakeIdentity4x4.h"
#include<numbers>

void SphereMesh::CreateVertex() {

    //VertexResourceとVertexBufferViewを用意 矩形を表現するための三角形を二つ(頂点4つ)
    vertexResource_ = DirectXCommon::CreateBufferResource( sizeof(VertexData) * 6 * kSubdivision_ * kSubdivision_);

    //頂点バッファビューを作成する
    //リソースの先頭アドレスから使う
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    //使用するリソースのサイズ頂点4つ分のサイズ
    vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6 * kSubdivision_ * kSubdivision_;
    //1頂点あたりのサイズ
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

#pragma region//Resourceにデータを書き込む
    //頂点リソースにデータを書き込む
    //書き込むためのアドレスを取得
    vertexResource_->Map(0, nullptr,
        reinterpret_cast<void**>(&vertexData_));

    const float pi = std::numbers::pi_v<float>;
    const float kLonEvery = 2.0f * pi / float(kSubdivision_);
    const float kLatEvery = pi / float(kSubdivision_);

    //緯度の方向に分割　-pi/2 ~ pi/2

    for (uint32_t latIndex = 0; latIndex < kSubdivision_; ++latIndex) {
        float lat = -pi / 2.0f + kLatEvery * latIndex;//現在の緯度
        //経度の方向に分割 0 ~ 2*pi
        for (uint32_t lonIndex = 0; lonIndex < kSubdivision_; ++lonIndex) {
            //書き込む最初の場所
            uint32_t startIndex = (latIndex * kSubdivision_ + lonIndex) * 6;
            float lon = lonIndex * kLonEvery;//現在の経度

            Vector2 uv = { float(lonIndex) / float(kSubdivision_),
                1.0f - float(latIndex) / float(kSubdivision_) };

            //a   
            vertexData_[startIndex].position.x = std::cos(lat) * std::cos(lon);
            vertexData_[startIndex].position.y = std::sin(lat);
            vertexData_[startIndex].position.z = std::cos(lat) * std::sin(lon);
            vertexData_[startIndex].position.w = 1.0f;
            vertexData_[startIndex].texcoord = uv;
            vertexData_[startIndex].normal = { vertexData_[startIndex].position.x , vertexData_[startIndex].position.y, vertexData_[startIndex].position.z };

            //b
            vertexData_[startIndex + 1].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
            vertexData_[startIndex + 1].position.y = std::sin(lat + kLatEvery);
            vertexData_[startIndex + 1].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
            vertexData_[startIndex + 1].position.w = 1.0f;
            vertexData_[startIndex + 1].texcoord = { uv.x,
               uv.y - 1.0f / float(kSubdivision_) };
            vertexData_[startIndex + 1].normal = { vertexData_[startIndex + 1].position.x , vertexData_[startIndex + 1].position.y, vertexData_[startIndex + 1].position.z };

            //c
            vertexData_[startIndex + 2].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
            vertexData_[startIndex + 2].position.y = std::sin(lat);
            vertexData_[startIndex + 2].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
            vertexData_[startIndex + 2].position.w = 1.0f;
            vertexData_[startIndex + 2].texcoord = { uv.x + 1.0f / float(kSubdivision_),
                 uv.y };
            vertexData_[startIndex + 2].normal = { vertexData_[startIndex + 2].position.x , vertexData_[startIndex + 2].position.y, vertexData_[startIndex + 2].position.z };


            //c
            vertexData_[startIndex + 3] = vertexData_[startIndex + 2];

            //b
            vertexData_[startIndex + 4] = vertexData_[startIndex + 1];

            //d
            vertexData_[startIndex + 5].position.x = std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery);
            vertexData_[startIndex + 5].position.y = std::sin(lat + kLatEvery);
            vertexData_[startIndex + 5].position.z = std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery);
            vertexData_[startIndex + 5].position.w = 1.0f;
            vertexData_[startIndex + 5].texcoord = { uv.x + 1.0f / float(kSubdivision_),
                uv.y - 1.0f / float(kSubdivision_) };
            vertexData_[startIndex + 5].normal = { vertexData_[startIndex + 5].position.x , vertexData_[startIndex + 5].position.y, vertexData_[startIndex + 5].position.z };

        }

    }


#pragma endregion

#pragma region//time

    int waveCount = 2;

    waveResource_ = DirectXCommon::CreateBufferResource(sizeof(Wave) * waveCount);

    //データを書き込む

    //書き込むためのアドレスを取得
    waveResource_->Map(0, nullptr, reinterpret_cast<void**>(&waveData_));

    waveData_[0].direction = { 1.0f,0.0f,0.0f };
    waveData_[0].time = 0.0f;
    waveData_[0].amplitude = 0.0f;
    waveData_[0].frequency = 4;

    waveData_[1].direction = { 1.0f,0.0f,0.0f };
    waveData_[1].time = 0.0f;
    waveData_[1].amplitude = 0.0f;
    waveData_[1].frequency = 4;

#pragma endregion

#pragma region//Balloon

    expansionResource_ = DirectXCommon::CreateBufferResource(sizeof(Balloon));

    //書き込むためのアドレスを取得
    expansionResource_->Map(0, nullptr, reinterpret_cast<void**>(&expansionData_));
    //データを書き込む
    expansionData_->expansion = 0.0f;
    expansionData_->sphere = 0.0f;
    expansionData_->cube = 0.0f;
    expansionData_->isSphere = false;

#pragma endregion


};

void SphereMesh::CreateMaterial() {

    //マテリアルリソースを作成 //ライトなし
    materialResource_.CreateMaterial(MaterialResource::LIGHTTYPE::NONE);

}

void SphereMesh::Create(ModelConfig& mc) {

    //マテリアルの作成
    CreateMaterial();

    CreateWorldVPResource();

    //頂点リソースを作る
    CreateVertex();

    //CreateIndexResource(device);

    transform_ = {
    {1.0f,1.0f,1.0f },
    {0.0f,0.0f,0.0f},
    {0.0f,0.0f,0.0f}
    };
    worldMatrix_ = MakeIdentity4x4();

    uvTransform_ = {
          {1.0f,1.0f,1.0f},
          {0.0f,0.0f,0.0f},
          {0.0f,0.0f,0.0f},
    };

    uvTransformMatrix_ = MakeIdentity4x4();
    modelConfig_ = mc;
}

//void Sphere::CreateIndexResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device) {
//
//#pragma region//IndexResourceを作成
//    indexResource_ = CreateBufferResource(device, sizeof(uint32_t) * 6);
//    //Viewを作成する IndexBufferView(IBV)
//
//    //リソースの先頭アドレスから使う
//    indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
//    //使用するリソースのサイズはインデックス6つ分のサイズ
//    indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
//    //インデックスはuint32_tとする
//    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
//#pragma endregion
//
//#pragma region//IndexResourceにデータを書き込む
//    //インデックスリーソースにデータを書き込む
//    indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
//
//    //頂点数を削減
//    indexData_[0] = 0;
//    indexData_[1] = 1;
//    indexData_[2] = 2;
//
//    indexData_[3] = 1;
//    indexData_[4] = 3;
//    indexData_[5] = 2;
//
//#pragma endregion
//}

void SphereMesh::UpdateUV() {

    uvTransformMatrix_ = MakeAffineMatrix(uvTransform_.scale, uvTransform_.rotate, uvTransform_.translate);
    materialResource_.SetUV(uvTransformMatrix_);
}


void SphereMesh::CreateWorldVPResource() {
    //WVP用のリソースを作る。Matrix3x3 1つ分のサイズを用意する。
    wvpResource_ = DirectXCommon::CreateBufferResource(sizeof(TransformationMatrix));
    //データを書き込む
    //書き込むためのアドレスを取得
    wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpDate_));
};

void SphereMesh::SetColor(const Vector4& color) {
    materialResource_.SetColor(color);
};

void SphereMesh::PreDraw(PSO& pso, PSO::PSOType type) {

    ID3D12GraphicsCommandList* commandList = DirectXCommon::GetCommandList();


    commandList->SetGraphicsRootSignature(modelConfig_.rootSignature->GetRootSignature(0));
    commandList->SetPipelineState(pso.GetGraphicsPipelineState(type).Get());//PSOを設定
    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void SphereMesh::Draw(Camera& camera, ShaderResourceView& srv, uint32_t lightType
) {

    ID3D12GraphicsCommandList* commandList = DirectXCommon::GetCommandList();


    materialResource_.SetLightType(lightType);

    worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    worldViewProjectionMatrix_ = Multiply(worldMatrix_, camera.GetViewProjectionMatrix());
    //データを書き込む
    *wvpDate_ = { worldViewProjectionMatrix_,worldMatrix_ };

    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
    //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
    commandList->SetGraphicsRootConstantBufferView(0, materialResource_.GetMaterialResource()->GetGPUVirtualAddress());
    //wvp用のCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
    //SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    commandList->SetGraphicsRootDescriptorTable(2, srv.GetTextureSrvHandleGPU());
    //LightのCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(3, modelConfig_.directionalLightResource->GetGPUVirtualAddress());
    //timeのSRVの場所を設定
    commandList->SetGraphicsRootShaderResourceView(4, waveResource_->GetGPUVirtualAddress());
    //expansionのCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(5, expansionResource_->GetGPUVirtualAddress());
    //描画!(DrawCall/ドローコール)。
    commandList->DrawInstanced(6 * kSubdivision_ * kSubdivision_, 1, 0, 0);

}

SphereMesh::~SphereMesh() {

    delete texture_;
};