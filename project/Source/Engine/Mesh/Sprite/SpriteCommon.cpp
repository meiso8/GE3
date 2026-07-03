#include "SpriteCommon.h"
#include"DirectXCommon.h"
#include"PSO.h"
#include"CommandList.h"
#include"Log.h"

RootSignature* SpriteCommon::rootSignature_ = nullptr;

D3D12_INDEX_BUFFER_VIEW SpriteCommon::indexBufferView_;
Microsoft::WRL::ComPtr <ID3D12Resource> SpriteCommon::indexResource_ = nullptr;
uint32_t* SpriteCommon::indexData_ = nullptr;

void SpriteCommon::Finalize()
{
    indexResource_.Reset();
    LogFile::Log("Finalize SpriteCommon");
}

void SpriteCommon::Initialize(RootSignature* rootSignature)
{
    rootSignature_ = rootSignature;
    CreateIndexResource();
    LogFile::Log("Initialize SpriteCommon");
}

void SpriteCommon::SetIndexBuffer(ID3D12GraphicsCommandList* commandList)
{
    //IBVを設定new
    commandList->IASetIndexBuffer(&indexBufferView_);//IBVを設定

}

void SpriteCommon::PreDraw(uint32_t blendMode,ID3D12GraphicsCommandList* commandList)
{
    commandList->SetGraphicsRootSignature(rootSignature_->GetRootSignature(RootSignature::SPRITE));
    commandList->SetPipelineState(PSO::GetGraphicsPipelineStateSprite(blendMode).Get());//PSOを設定
    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void SpriteCommon::DrawCall(ID3D12GraphicsCommandList* commandList)
{
    //描画!（DrawCall/ドローコール）6個のインデックスを使用し1つのインスタンスを描画。その他は当面0で良い。
    commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void SpriteCommon::CreateIndexResource() {

#pragma region//IndexResourceを作成
    indexResource_ = DirectXCommon::CreateBufferResource(sizeof(uint32_t) * 6);
    //Viewを作成する IndexBufferView(IBV)
    indexResource_->SetName(L"SpriteCommon_IndexResource");
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

    indexResource_->Unmap(0, nullptr);


#pragma endregion
}

