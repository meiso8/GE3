#pragma once
#include"ModelData.h"

class ModelManager
{
public:
    enum TAG {
        PLAYER,
        MODELS,
    };

    static ModelData& GetModelData(const TAG& modelHandle) {
        if (modelHandle < MODELS) {
            return modelDatas[modelHandle];
        }
        return modelDatas[0];
    }
    static void LoadAllModel();

private:
    static uint32_t Load(const std::string& directoryPath, const std::string& filename);
    static void LoadModel(const std::string& directoryPath, const std::string& filename);
    //インデックスの開始番号
    static uint32_t GetTextureIndexByFileName(const std::string& filename);
private:
    static std::vector<ModelData> modelDatas;
    static std::vector<uint32_t> modelHandle_;
};

