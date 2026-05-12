#pragma once
#include"MeshCommon.h"
#include"ModelManager.h"
#include"ModelData.h"
#include<memory>

class Model :public Primitive
{
public:
    void CreateModel();

    void SetModelData(std::unique_ptr<ModelData> modelData) {
        modelData_ = std::move(modelData);
    }

    ModelData* GetModelData() {
        return modelData_.get();
    }
 
    D3D12_VERTEX_BUFFER_VIEW& GetVBV() { return vertexBufferView_; };
    //インデックスデータ
    D3D12_INDEX_BUFFER_VIEW& GetIBV() { return indexBufferView_; };


    /// @brief テクスチャをモデルオリジナルのものに戻す
    void ResetTextureHandle() {
        textureHandle_ = modelData_->material.textureSrvIndex;
    };
private:
    std::unique_ptr<ModelData> modelData_ = nullptr;
};
