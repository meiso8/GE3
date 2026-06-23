#pragma once
#include"ModelData.h"
#include<map>
#include<filesystem>

class Model;
struct aiNode;
class ModelManager
{
private:
public:
    static const uint32_t kMaxModelCount = 512;
    /// @brief モデルを取得します
    /// @param handle モデルハンドル
    /// @return Modelのポインタ
    static Model* GetModel(const std::filesystem::path& tag);
    static std::map <const std::filesystem::path, std::unique_ptr< Model>>& GetModels() { return models_; };
    static void Finalize();
    static void LoadModel(const std::filesystem::path& filePath);
private:
    static Node ReadNode(aiNode* node);
private:
    static std::map <const std::filesystem::path, std::unique_ptr< Model> > models_;
};

