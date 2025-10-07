#include "SpriteC.h"
#include"DirectXCommon.h"

SpriteC* SpriteC::instance_ = nullptr;

SpriteC* SpriteC::GetInstance()
{
    if (instance_ == nullptr) {
        instance_ = new SpriteC();
    }

    return instance_;
}

void SpriteC::Initialize(ModelConfig& mc)
{

    CreateIndexResource();
    modelConfig_ = &mc;
}

void SpriteC::Draw(ID3D12GraphicsCommandList* commandList)
{

    //IBVを設定new
    commandList->IASetIndexBuffer(&indexBufferView_);//IBVを設定
    //LightのCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(3, modelConfig_->directionalLightResource->GetGPUVirtualAddress());
    //描画!（DrawCall/ドローコール）6個のインデックスを使用し1つのインスタンスを描画。その他は当面0で良い。
    commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void SpriteC::PreDraw(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetGraphicsRootSignature(modelConfig_->rootSignature->GetRootSignature(0));
}


void SpriteC::CreateIndexResource() {

#pragma region//IndexResourceを作成
    indexResource_ = DirectXCommon::CreateBufferResource(sizeof(uint32_t) * 6);
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

