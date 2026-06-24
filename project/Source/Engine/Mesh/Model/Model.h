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
private:
    std::unique_ptr<ModelData> modelData_ = nullptr;
};
