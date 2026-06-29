#include "ModelManager.h"
//ファイルやディレクトリに関する操作を行うライブラリ
#include <filesystem>
#include <fstream>//ファイルの書いたり読んだりするライブラリ
#include<cassert> //assertも利用するため

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include"DirectXCommon.h"

#include"Texture.h"
#include"Model.h"
#include"Quaternion/Quaternion.h"
#include"MakeMatrix.h"
#include"Animation.h"

std::map < const std::filesystem::path, std::unique_ptr< Model> > ModelManager::models_;

// ========================================================================================================

Model* ModelManager::GetModel(const std::filesystem::path& tag)
{

    if (models_.contains(tag)) {
        return models_.at(tag).get();
    }
    std::cerr << "モデルの取得に失敗しました！" << std::endl;

    return nullptr;

}
void ModelManager::Finalize()
{
    models_.clear();
}

// ========================================================================================================

void ModelManager::LoadModel(const std::filesystem::path& filePath)
{
    std::string directoryPath = filePath.parent_path().string();
    std::string filename = filePath.filename().string();
    //std::string ext = filePath.extension().string();


    // .stem() で拡張子抜きのファイル名を取得し、.string() で std::string に変換
    std::string tag = std::filesystem::path(filename).stem().string();

    //読み込み済みテクスチャを検索
    if (models_.contains(tag)) {
        return;
    }
    //テクスチャ枚数上限チェック
    assert(models_.size() < kMaxModelCount);

    //追加したテクスチャデータのポインタ
    std::unique_ptr<Model> model = std::make_unique<Model>();

    std::unique_ptr<ModelData> modelData = std::make_unique<ModelData>();

    //モデルデータにタグを代入
    modelData->meshName = tag;

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        filePath.string(),
        aiProcess_Triangulate |
        aiProcess_FlipWindingOrder |
        aiProcess_FlipUVs |
        aiProcess_SortByPType);

    assert(scene->HasMeshes());


    // 全メッシュの合計頂点数とインデックス数を先に数える 

    size_t totalVertices = 0;
    size_t totalIndices = 0;

    for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
        aiMesh* mesh = scene->mMeshes[meshIndex];
        totalVertices += mesh->mNumVertices;
        totalIndices += mesh->mNumFaces * 3;
    }

    modelData->vertices.reserve(totalVertices);
    modelData->indices.reserve(totalIndices);

    for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
        aiMesh* mesh = scene->mMeshes[meshIndex];
        assert(mesh->HasNormals());
        assert(mesh->HasTextureCoords(0));

        // 現在の頂点数を記録（インデックスのオフセット用） 
        uint32_t vertexOffset = static_cast<uint32_t>(modelData->vertices.size());
        // ★ここが重要：このメッシュのインデックスが始まる位置を記録
        uint32_t indexStart = static_cast<uint32_t>(modelData->indices.size());

        for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {

            aiVector3D& position = mesh->mVertices[vertexIndex];
            aiVector3D& normal = mesh->mNormals[vertexIndex];
            aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

            VertexData vertex;
            vertex.position = { -position.x,position.y,position.z,1.0f };
            vertex.normal = { -normal.x,normal.y,normal.z };
            vertex.texcoord = { texcoord.x,texcoord.y };
            modelData->vertices.push_back(vertex);
        }

        // インデックス追加（オフセットを加える！）
        for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
            aiFace& face = mesh->mFaces[faceIndex]; assert(face.mNumIndices == 3);
            for (uint32_t element = 0; element < face.mNumIndices; ++element) {
                uint32_t vertexIndex = face.mIndices[element]; modelData->indices.push_back(vertexOffset + vertexIndex); // ← ここが重要！ 
            }
        }

        MeshSection section;
        section.indexStart = indexStart;
        section.indexCount = static_cast<uint32_t>(modelData->indices.size()) - indexStart;
        aiMaterial* meshMaterial = scene->mMaterials[mesh->mMaterialIndex];
        section.materialName = meshMaterial->GetName().C_Str();

        modelData->sections.push_back(section);

        //SkinCluster骨の解析
        for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
            //Jointごとの格納領域を作る
            aiBone* bone = mesh->mBones[boneIndex];
            std::string jointName = bone->mName.C_Str();
            JointWeightData& jointWeightData = modelData->skinClusterData[jointName];
            //InverseBinePoseMatrixの抽出
            aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
            aiVector3D scale, translate;
            aiQuaternion rotate;
            bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
            //左手系のBindPoseを作る
            Matrix4x4 bindPoseMatrix = MakeAffineMatrix(
                { scale.x,scale.y,scale.z }, { rotate.x,-rotate.y,-rotate.z,rotate.w }, { -translate.x,translate.y,translate.z });
            //InverseBindPosemMatrixにする
            jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);
            //Wight情報を取り出す
            for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
                //mVertexIdは該当Mesh内でのIndexである MultiMesh/MuitiMaterial対応する際にはこのまま保存するのではなく、全体を通して改良が必要である。
                jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
            }

        }

    }

    //マテリアルの解析
    for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {

        aiMaterial* material = scene->mMaterials[materialIndex];
        //マテリアルの名前をマッピングする
        modelData->materials[material->GetName().C_Str()] = LoadMaterial::LoadMaterialFromAssimp(material, directoryPath);
    }

    //モデルのテクスチャを読む
    for (auto& [name, material] : modelData->materials) {
        for (auto& textureData : material.textureData_) {
            if (!textureData.textureFilePath.empty()) {
                textureData.textureSrvIndex = Texture::AddTextureHandle(textureData.textureFilePath);
            }
        }
    }

    //モデルのNodeを読む
    modelData->rootNode = ReadNode(scene->mRootNode);

    //アニメーションがあったら
    if (scene->HasAnimations()) {
        modelData->animations_ = AnimationManager::LoadAnimation(filePath);
    }



    model->SetModelData(std::move(modelData));
    //モデルを作成する
    model->CreateModel();



    //ハンドルとモデルをセットにする
    models_.insert(std::make_pair(tag, std::move(model)));

}

Model* ModelManager::LoadModelAndGet(const std::filesystem::path& filePath)
{
    LoadModel(filePath);
    // .stem() で拡張子抜きのファイル名を取得し、.string() で std::string に変換
    std::string tag = std::filesystem::path(filePath.filename().string()).stem().string();
    return  GetModel(tag);
}

Node ModelManager::ReadNode(aiNode* node)
{
    Node result;
    aiVector3D scale, translate;

    aiQuaternion rotate;
    //assimpの行列からSRTを抽出する関数を利用
    node->mTransformation.Decompose(scale, rotate, translate);
    //Scale
    result.transform.scale = { scale.x,scale.y,scale.z };
    //x軸を反転、さらに回転方向が逆なので軸を反転させる
    result.transform.rotate = { rotate.x,-rotate.y,-rotate.z,rotate.w };
    //x軸を反転
    result.transform.translate = { -translate.x,translate.y,translate.z };
    result.localMatrix = MakeAffineMatrix(result.transform.scale, result.transform.rotate, result.transform.translate);

    result.name = node->mName.C_Str();//Node名の格納
    result.children.resize(node->mNumChildren);//子供の数だけ確保

    for (uint32_t childrenIndex = 0; childrenIndex < node->mNumChildren; ++childrenIndex) {
        //再帰的に読んで階層構造を作っていく
        result.children[childrenIndex] = ReadNode(node->mChildren[childrenIndex]);
    }

    return result;
}


