#include "ModelManager.h"
//ファイルやディレクトリに関する操作を行うライブラリ
#include <filesystem>
#include <fstream>//ファイルの書いたり読んだりするライブラリ
#include<cassert> //assertも利用するため

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include"DirectXCommon.h"

std::vector<ModelData> ModelManager::modelDatas;
std::vector<uint32_t> ModelManager::modelHandle_;


void ModelManager::LoadAllModel()
{
    modelHandle_.resize(MODELS);
    modelHandle_[PLAYER] = Load("resources/player", "player.obj");
}

// ========================================================================================================

uint32_t ModelManager::Load(const std::string& directoryPath, const std::string& filename)
{
    LoadModel(directoryPath, filename);
    return GetTextureIndexByFileName(filename);
}

void ModelManager::LoadModel(const std::string& directoryPath, const std::string& filename)
{
    //読み込み済みテクスチャを検索
    auto it = std::find_if(
        modelDatas.begin(),
        modelDatas.end(),
        [&](ModelData& soundData) {return soundData.filename == filename; }
    );

    //テクスチャ枚数上限チェック
    assert(modelDatas.size() < DirectXCommon::kMaxModelCount);

    if (it != modelDatas.end()) {
        return;
    }

    //テクスチャデータを追加
    modelDatas.resize(modelDatas.size() + 1);
    //追加したテクスチャデータの参照を取得する
    ModelData& modelData = modelDatas.back();

    Assimp::Importer importer;
    std::string filePath = directoryPath + "/" + filename;
    modelData.filename = filename;

    const aiScene* scene = importer.ReadFile(filePath.c_str(),
        aiProcess_Triangulate |
        aiProcess_FlipWindingOrder |
        aiProcess_FlipUVs |
        aiProcess_SortByPType);

    assert(scene->HasMeshes());

    for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
        aiMesh* mesh = scene->mMeshes[meshIndex];
        assert(mesh->HasNormals());
        assert(mesh->HasTextureCoords(0));

        for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
            aiFace& face = mesh->mFaces[faceIndex];
            assert(face.mNumIndices == 3);//三角形のみサポート
            for (uint32_t element = 0; element < face.mNumIndices; ++element) {
                uint32_t vertexIndex = face.mIndices[element];
                aiVector3D& position = mesh->mVertices[vertexIndex];
                aiVector3D& normal = mesh->mNormals[vertexIndex];
                aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
                VertexData vertex;
                vertex.position = { position.x,position.y,position.z,1.0f };
                vertex.normal = { normal.x,normal.y,normal.z };
                vertex.texcoord = { texcoord.x,texcoord.y };


                vertex.position.x *= -1.0f;
                vertex.normal.x *= -1.0f;
                modelData.vertices.push_back(vertex);
            }

        }


    }

    for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
        aiMaterial* material = scene->mMaterials[materialIndex];

        //MultiMaterialに対応していく場合などは改造が必要である
        if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
            aiString textureFilePath;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
            modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
        }

    }
}

uint32_t ModelManager::GetTextureIndexByFileName(const std::string& filePath)
{
    //読み込み済みデータを検索
    auto it = std::find_if(
        modelDatas.begin(),
        modelDatas.end(),
        [&](ModelData& modelData) {return modelData.filename == filePath; }
    );

    if (it != modelDatas.end()) {
        uint32_t modelIndex = static_cast<uint32_t>(std::distance(modelDatas.begin(), it));
        return modelIndex;
    }

    assert(0);
    return 0;
}
