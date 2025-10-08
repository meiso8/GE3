#include"ModelData.h"
//ファイルやディレクトリに関する操作を行うライブラリ
#include <filesystem>
#include <fstream>//ファイルの書いたり読んだりするライブラリ
#include<cassert> //assertも利用するため

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

//ModelData LoadObjeFile(const std::string& directoryPath, const std::string& filename) {
//
//    ModelData modelData;
//
//    Assimp::Importer importer;
//    std::string filePath = directoryPath + "/" + filename;
//    modelData.filename = filename;
//
//    const aiScene* scene = importer.ReadFile(filePath.c_str(),
//        aiProcess_Triangulate |
//        aiProcess_FlipWindingOrder |
//        aiProcess_FlipUVs |
//        aiProcess_SortByPType);
//
//    assert(scene->HasMeshes());
//
//    for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
//        aiMesh* mesh = scene->mMeshes[meshIndex];
//        assert(mesh->HasNormals());
//        assert(mesh->HasTextureCoords(0));
//
//        for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
//            aiFace& face = mesh->mFaces[faceIndex];
//            assert(face.mNumIndices == 3);//三角形のみサポート
//            for (uint32_t element = 0; element < face.mNumIndices; ++element) {
//                uint32_t vertexIndex = face.mIndices[element];
//                aiVector3D& position = mesh->mVertices[vertexIndex];
//                aiVector3D& normal = mesh->mNormals[vertexIndex];
//                aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
//                VertexData vertex;
//                vertex.position = { position.x,position.y,position.z,1.0f };
//                vertex.normal = { normal.x,normal.y,normal.z };
//                vertex.texcoord = { texcoord.x,texcoord.y };
//
//
//                vertex.position.x *= -1.0f;
//                vertex.normal.x *= -1.0f;
//                modelData.vertices.push_back(vertex);
//            }
//
//        }
//
//
//    }
//
//
//    for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
//        aiMaterial* material = scene->mMaterials[materialIndex];
//
//        //MultiMaterialに対応していく場合などは改造が必要である
//        if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
//            aiString textureFilePath;
//            material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
//            modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
//        }
//
//    }
//
//    //ModelDataを返す
//    return modelData;
//
//
//}

//ModelData LoadObjeFile(const std::string& directoryPath, const std::string& filename) {
//
//    //1.中で必要となる変数の宣言
//    ModelData modelData;
//    std::vector<Vector4> positions;
//    std::vector<Vector3> normals;
//    std::vector<Vector2> texcoords;
//    std::string line;//ファイルから読み込んだ1行を格納するもの
//
//    //2.ファイルを開く
//    std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
//    assert(file.is_open());//開けなかったら止める
//
//    //3.実際にファイルを読み、ModelDataを構成していく
//    //std::getlineはstreamから1行読んでstd::stringに格納する関数
//    while (std::getline(file, line)) {
//        std::string identifier;
//        //文字列を分解しながら読むためのクラス（空白を区切り文字として読むことが可能）
//        std::istringstream s(line); //これに対して>>すると空文字区切りで1ずつ値を読むことが出来る
//        s >> identifier;//先頭の識別子を読む　IDを読む
//
//        //IDに応じた処理
//        if (identifier == "v") {
//            Vector4 position;
//            s >> position.x >> position.y >> position.z;
//
//            position.z *= -1.0f;
//            position.w = 1.0f;//同次座標のため
//            positions.push_back(position);
//
//        } else if (identifier == "vt") {
//            Vector2 texcoord;
//            s >> texcoord.x >> texcoord.y;
//            texcoord.y = 1.0f - texcoord.y;
//            texcoords.push_back(texcoord);
//
//        } else if (identifier == "vn") {
//            Vector3 normal;
//            s >> normal.x >> normal.y >> normal.z;
//
//            normal.z *= -1.0f;
//            normals.push_back(normal);
//        } else if (identifier == "f") {
//            //三角形を作る
//            VertexData triangle[3];
//            //面は三角形限定。その他は未対応
//            for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
//                std::string vertexDefinition;
//                s >> vertexDefinition;
//                //頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
//                std::istringstream v(vertexDefinition);
//                uint32_t elementIndices[3];
//                for (int32_t element = 0; element < 3; ++element) {
//                    std::string index;
//                    std::getline(v, index, '/');//区切りでインデックスを読んでいく
//                    elementIndices[element] = std::stoi(index);
//                }
//
//                //要素へのIndexから、実際の要素の値を取得して、頂点を構成する
//                Vector4 position = positions[elementIndices[0] - 1];//1始まりなので-1する
//                Vector2 texcoord = texcoords[elementIndices[1] - 1];
//                Vector3 normal = normals[elementIndices[2] - 1];
//
//                //まずobj通りに保存、格納する際に逆にする　座標系の統一のため
//                triangle[faceVertex] = { position,texcoord,normal };
//
//            }
//
//            //////頂点を逆順で登録することで、回り順を逆順にする
//            modelData.vertices.push_back(triangle[2]);
//            modelData.vertices.push_back(triangle[1]);
//            modelData.vertices.push_back(triangle[0]);
//
//        } else if (identifier == "mtllib") {
//
//            //materialTemplateLibraryファイルの名前を取得する
//            std::string materialFilename;
//            s >> materialFilename;
//            //基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
//            modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
//        }
//
//    }
//
//    //4.ModelDataを返す
//    return modelData;
//
//}
