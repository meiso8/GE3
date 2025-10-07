#include "Sprite.h"
#include"DirectXCommon.h"
#include"TransformationMatrix.h"
#include"MakeAffineMatrix.h"
#include"MakeIdentity4x4.h"
#include"Multiply.h"
#include"MyEngine.h"

SpriteC* Sprite::spriteCommon = nullptr;

void Sprite::Initialize(const Vector2& size)
{

    spriteCommon = SpriteC::GetInstance();

    commandList = DirectXCommon::GetCommandList();
    SetSize(size);
    CreateVertex();
    CreateTransformationMatrix();
    CreateUVTransformationMatrix();
    CreateWaveData();
    CreateBalloonData();

    materialResource_.CreateMaterial();
}

void Sprite::ResetSize(const Vector2& size) {
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

void Sprite::PreDraw(uint32_t blendMode) {

    PSO* pso = MyEngine::GetPSO(blendMode);

    spriteCommon->PreDraw(commandList);
    commandList->SetPipelineState(pso->GetGraphicsPipelineState(PSO::TRIANGLE).Get());//PSOを設定
    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Sprite::Draw(
    ShaderResourceView& srv, Camera& camera, uint32_t lightType
) {

    materialResource_.SetLightType(lightType);
    transform_.translate = { position_.x,position_.y,0.0f };
    transform_.rotate = { 0.0f,0.0f,rotate_ };
    worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
    worldViewProjectionMatrix_ = Multiply(worldMatrix_, camera.GetViewProjectionMatrix());
    *transformationMatrixData_ = { worldViewProjectionMatrix_,worldMatrix_ };

    //頂点バッファビューを設定
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
    spriteCommon->SetIndexBuffer(commandList);

    //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
    commandList->SetGraphicsRootConstantBufferView(0, materialResource_.GetMaterialResource()->GetGPUVirtualAddress());
    //TransformationMatrixCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
    //SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    commandList->SetGraphicsRootDescriptorTable(2, srv.GetTextureSrvHandleGPU());

    spriteCommon->LightDraw(commandList);

    //Wave timeのSRVの場所を設定
    commandList->SetGraphicsRootShaderResourceView(4, waveResource_->GetGPUVirtualAddress());
    //expansionのCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(5, expansionResource_->GetGPUVirtualAddress());

    spriteCommon->DrawCall(commandList);

};

// ===================================//Private関数//===================================


void Sprite::CreateVertex()
{
    //VertexResourceとVertexBufferViewを用意 矩形を表現するための三角形を二つ(頂点4つ)
    vertexResource_ = DirectXCommon::CreateBufferResource(sizeof(VertexData) * 4);

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

void Sprite::CreateUVTransformationMatrix()
{
    uvTransform_ = {
     {1.0f,1.0f,1.0f},
     {0.0f,0.0f,0.0f},
     {0.0f,0.0f,0.0f},
    };

    uvTransformMatrix_ = MakeIdentity4x4();
}

void Sprite::CreateTransformationMatrix() {

    //Matrix4x4　1つ分のサイズを用意
    transformationMatrixResource_ = DirectXCommon::CreateBufferResource(sizeof(TransformationMatrix));
    //データを書き込む
    //書き込むためのアドレスを取得
    transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));

    transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{ position_.x,position_.y,0.0f } };
    worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

}

void Sprite::CreateMaterial() {

    //マテリアルリソースを作成 //ライトなし
    materialResource_.CreateMaterial(MaterialResource::LIGHTTYPE::NONE);

}

void Sprite::CreateWaveData()
{
    int waveCount = 2;
    waveResource_ = DirectXCommon::CreateBufferResource(sizeof(Wave) * waveCount);

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
}

void Sprite::CreateBalloonData()
{
    expansionResource_ = DirectXCommon::CreateBufferResource(sizeof(Balloon));

    //書き込むためのアドレスを取得
    expansionResource_->Map(0, nullptr, reinterpret_cast<void**>(&expansionData_));
    //データを書き込む
    expansionData_->expansion = 0.0f;
    expansionData_->sphere = 0.0f;
    expansionData_->cube = 0.0f;
    expansionData_->isSphere = false;

}
