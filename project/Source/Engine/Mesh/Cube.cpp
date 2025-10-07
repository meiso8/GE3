#include "Cube.h"

#include"DirectXCommon.h"


#include"MakeAffineMatrix.h"
#include"Multiply.h"
#include"MyEngine.h"
#include"ModelConfig.h"

void Cube::Create(
) {

    CreateVertex();
    CreateIndexResource();
    CreateTransformationMatrix();
    CreateMaterial();

    modelConfig_ = ModelConfig::GetInstance();
 
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


}

void Cube::CreateVertex() {

    vertexResource_ = DirectXCommon::CreateBufferResource(sizeof(VertexData) * 8);

    //頂点バッファビューを作成する
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = sizeof(VertexData) * 8;
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

#pragma region //頂点データの設定

    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

    Vector3 min = { -0.5f,-0.5f,-0.5f };
    Vector3 max = { 0.5f,0.5f,0.5f };

    vertexData_[0].position = { min.x,min.y,min.z,1.0f };//左sita
    vertexData_[0].texcoord = { 0.0f,1.0f };
    vertexData_[0].normal = { vertexData_[0].position.x,  vertexData_[0].position.y,  vertexData_[0].position.z };//法線
    vertexData_[1].position = { min.x,max.y,min.z,1.0f };//左上
    vertexData_[1].texcoord = { 0.0f,0.0f };
    vertexData_[1].normal = { vertexData_[1].position.x,  vertexData_[1].position.y,  vertexData_[1].position.z };
    vertexData_[2].position = { max.x,min.y,min.z,1.0f };//右下
    vertexData_[2].texcoord = { 1.0f,1.0f };
    vertexData_[2].normal = { vertexData_[2].position.x,  vertexData_[2].position.y,  vertexData_[2].position.z };
    vertexData_[3].position = { max.x,max.y,min.z,1.0f };//右上
    vertexData_[3].texcoord = { 1.0f,0.0f };
    vertexData_[3].normal = { vertexData_[3].position.x,  vertexData_[3].position.y,  vertexData_[3].position.z };

    vertexData_[4].position = { min.x,min.y,max.z,1.0f };//左sita
    vertexData_[4].texcoord = { 0.0f,1.0f };
    vertexData_[4].normal = { vertexData_[4].position.x, vertexData_[4].position.y, vertexData_[4].position.z };//法線
    vertexData_[5].position = { min.x,max.y,max.z,1.0f };//hidariue
    vertexData_[5].texcoord = { 0.0f,0.0f };
    vertexData_[5].normal = { vertexData_[5].position.x, vertexData_[5].position.y, vertexData_[5].position.z };
    vertexData_[6].position = { max.x,min.y,max.z,1.0f };//migisita
    vertexData_[6].texcoord = { 1.0f,1.0f };
    vertexData_[6].normal = { vertexData_[6].position.x,  vertexData_[6].position.y,  vertexData_[6].position.z };
    vertexData_[7].position = { max.x,max.y,max.z,1.0f };//migiue
    vertexData_[7].texcoord = { 1.0f,0.0f };
    vertexData_[7].normal = { vertexData_[7].position.x,  vertexData_[7].position.y,  vertexData_[7].position.z };
#pragma endregion

}

void Cube::CreateIndexResource() {

#pragma region//IndexResourceを作成
    indexResource_ = DirectXCommon::CreateBufferResource(sizeof(uint32_t) * 36);
    //Viewを作成する IndexBufferView(IBV)

    //リソースの先頭アドレスから使う
    indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes = sizeof(uint32_t) * 36;
    //インデックスはuint32_tとする
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
#pragma endregion

#pragma region//IndexResourceにデータを書き込む
    //インデックスリーソースにデータを書き込む
    indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

    //頂点数を削減

        // Front face
    indexData_[0] = 0, indexData_[1] = 1, indexData_[2] = 2,
        indexData_[3] = 2, indexData_[4] = 1, indexData_[5] = 3,
        // Back face
        indexData_[6] = 4, indexData_[7] = 6, indexData_[8] = 5,
        indexData_[9] = 5, indexData_[10] = 6, indexData_[11] = 7,
        // Left face
        indexData_[12] = 4, indexData_[13] = 5, indexData_[14] = 0,
        indexData_[15] = 0, indexData_[16] = 5, indexData_[17] = 1,
        // Right face
        indexData_[18] = 2, indexData_[19] = 3, indexData_[20] = 6,
        indexData_[21] = 6, indexData_[22] = 3, indexData_[23] = 7,
        // Top face
        indexData_[24] = 1, indexData_[25] = 5, indexData_[26] = 3,
        indexData_[27] = 3, indexData_[28] = 5, indexData_[29] = 7,
        // Bottom face
        indexData_[30] = 4, indexData_[31] = 0, indexData_[32] = 6,
        indexData_[33] = 6, indexData_[34] = 0, indexData_[35] = 2;

#pragma endregion
}

void Cube::CreateTransformationMatrix() {

    //Matrix4x4　1つ分のサイズを用意
    transformationMatrixResource_ = DirectXCommon::CreateBufferResource(sizeof(TransformationMatrix));
    //データを書き込む
    //書き込むためのアドレスを取得
    transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
}

void Cube::CreateMaterial() {

    //マテリアルリソースを作成
    materialResource_.CreateMaterial(0);

}

void Cube::SetColor(const Vector4& color) {
    materialResource_.SetColor(color);
}

void Cube::SetMinMax(const Vector3& min, const Vector3& max) {
    vertexData_[0].position = { min.x,min.y,min.z,1.0f };//左sita
    vertexData_[0].normal = { vertexData_[0].position.x,  vertexData_[0].position.y,  vertexData_[0].position.z };//法線
    vertexData_[1].position = { min.x,max.y,min.z,1.0f };//左上
    vertexData_[1].normal = { vertexData_[1].position.x,  vertexData_[1].position.y,  vertexData_[1].position.z };
    vertexData_[2].position = { max.x,min.y,min.z,1.0f };//右下
    vertexData_[2].normal = { vertexData_[2].position.x,  vertexData_[2].position.y,  vertexData_[2].position.z };
    vertexData_[3].position = { max.x,max.y,min.z,1.0f };//右上
    vertexData_[3].normal = { vertexData_[3].position.x,  vertexData_[3].position.y,  vertexData_[3].position.z };

    vertexData_[4].position = { min.x,min.y,max.z,1.0f };//左sita
    vertexData_[4].texcoord = { 0.0f,1.0f };
    vertexData_[4].normal = { vertexData_[4].position.x, vertexData_[4].position.y, vertexData_[4].position.z };//法線
    vertexData_[5].position = { min.x,max.y,max.z,1.0f };//hidariue
    vertexData_[5].texcoord = { 0.0f,0.0f };
    vertexData_[5].normal = { vertexData_[5].position.x, vertexData_[5].position.y, vertexData_[5].position.z };
    vertexData_[6].position = { max.x,min.y,max.z,1.0f };//migisita
    vertexData_[6].texcoord = { 1.0f,1.0f };
    vertexData_[6].normal = { vertexData_[6].position.x,  vertexData_[6].position.y,  vertexData_[6].position.z };
    vertexData_[7].position = { max.x,max.y,max.z,1.0f };//migiue
    vertexData_[7].texcoord = { 1.0f,0.0f };
    vertexData_[7].normal = { vertexData_[7].position.x,  vertexData_[7].position.y,  vertexData_[7].position.z };

}

void Cube::PreDraw(BlendMode blendMode) {

    ID3D12GraphicsCommandList* commandList = DirectXCommon::GetCommandList();

    PSO* pso = MyEngine::GetPSO(blendMode);

    commandList->SetGraphicsRootSignature(modelConfig_->rootSignature->GetRootSignature(0));
    commandList->SetPipelineState(pso->GetGraphicsPipelineState(PSO::TRIANGLE).Get());//PSOを設定
    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Cube::Draw(
    ShaderResourceView& srv, Camera& camera,const Matrix4x4& worldMatrix, uint32_t lightType
) {

    ID3D12GraphicsCommandList* commandList = DirectXCommon::GetCommandList();

    materialResource_.SetLightType(lightType);

    worldViewProjectionMatrix_ = Multiply(worldMatrix, camera.GetViewProjectionMatrix());
    *transformationMatrixData_ = { worldViewProjectionMatrix_,worldMatrix };

 
    //頂点バッファビューを設定
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
    //IBVを設定new
    commandList->IASetIndexBuffer(&indexBufferView_);//IBVを設定
    //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
    commandList->SetGraphicsRootConstantBufferView(0, materialResource_.GetMaterialResource()->GetGPUVirtualAddress());
    //TransformationMatrixCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
    //SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    commandList->SetGraphicsRootDescriptorTable(2, srv.GetTextureSrvHandleGPU());
    //LightのCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(3, modelConfig_->directionalLightResource->GetGPUVirtualAddress());
    //timeのSRVの場所を設定
    commandList->SetGraphicsRootShaderResourceView(4, waveResource_->GetGPUVirtualAddress());
    //expansionのCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(5, expansionResource_->GetGPUVirtualAddress());

    //描画!（DrawCall/ドローコール）6個のインデックスを使用し1つのインスタンスを描画。その他は当面0で良い。
    commandList->DrawIndexedInstanced(36, 1, 0, 0, 0);
};

