#pragma once
#include"Primitive.h"
#include"ModelManager.h"
#include"ModelData.h"
#include<memory>

class Model :public Primitive
{
public:
    ~Model();
    void CreateModel();
    void SetModelData(std::unique_ptr<ModelData> modelData) {
        //メッシュの名前を記録
        meshName_ = modelData->meshName;
        modelData_ = std::move(modelData);
       
    }
    ModelData* GetModelData() {
        return modelData_.get();
    }
private:
    std::unique_ptr<ModelData> modelData_ = nullptr;
};
