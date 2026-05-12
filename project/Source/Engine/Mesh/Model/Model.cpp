#include "Model.h"

void Model::CreateModel()
{
    Create(*modelData_);
    
    textureHandle_ = modelData_->material.textureSrvIndex;
}

//
//void Model::CreateVertex() {
//    //頂点リソースを作る
//    vertexResource_ = DirectXCommon::CreateBufferResource(sizeof(VertexData) * modelData_->vertices.size());
//
//    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();//リソースの先頭アドレスから使う
//    vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_->vertices.size());//使用するリソースのサイズは頂点のサイズ
//    vertexBufferView_.StrideInBytes = sizeof(VertexData);//1頂点あたりのサイズ
//
//    //頂点リソースにデータを書き込む
//    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));//書き込むためのアドレスを取得
//    std::memcpy(vertexData_, modelData_->vertices.data(), sizeof(VertexData) * modelData_->vertices.size());//頂点データをリソースにコピー
//    vertexResource_->Unmap(0, nullptr);
//
//}
//void Model::CreateIndexResource()
//{
//    indexResource_ = DirectXCommon::CreateBufferResource(sizeof(uint32_t) * modelData_->indices.size());
//    //Viewを作成する IndexBufferView(IBV)
//
//    //リソースの先頭アドレスから使う
//    indexBufferView_.BufferLocation = UINT(indexResource_->GetGPUVirtualAddress());
//    indexBufferView_.SizeInBytes = sizeof(uint32_t) * UINT(modelData_->indices.size());
//    //インデックスはuint32_tとする
//    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
//#pragma endregion
//
//#pragma region//IndexResourceにデータを書き込む
//    //インデックスリーソースにデータを書き込む
//    indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
//    std::memcpy(indexData_, modelData_->indices.data(), sizeof(uint32_t) * modelData_->indices.size());
//
//}
//;
//
//void Model::Draw(ID3D12GraphicsCommandList* commandList) {
//
//    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
// 
//    //IBVを設定
//    commandList->IASetIndexBuffer(&indexBufferView_);//IBVを設定
//
//    //SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
//    SrvManager::SetGraphicsRootDescriptorTable(2, textureHandle_);
//
//    //描画!（DrawCall/ドローコール）6個のインデックスを使用し1つのインスタンスを描画。その他は当面0で良い。
//    commandList->DrawIndexedInstanced(UINT(modelData_->indices.size()), 1, 0, 0, 0);
//
//}
