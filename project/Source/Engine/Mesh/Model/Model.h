#pragma once
#include"Primitive.h"
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
    void Draw(ID3D12GraphicsCommandList* commandList)override;
    void DrawCallForParticle(ID3D12GraphicsCommandList* commandList, const uint32_t numInstance);
    D3D12_VERTEX_BUFFER_VIEW& GetVBV() { return vertexBufferView_; };
    //インデックスデータ
    D3D12_INDEX_BUFFER_VIEW& GetIBV() { return indexBufferView_; };
private:
    std::unique_ptr<ModelData> modelData_ = nullptr;
};
