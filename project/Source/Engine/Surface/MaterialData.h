#pragma once
#include<string>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include<array>
#include"Vector3.h"

enum TEXTURE_USAGE
{
    TEXTURE_USAGE_DIFFUSE = 0,  //!< ディフューズマップとして利用します.
    TEXTURE_USAGE_SPECULAR,     //!< スペキュラーマップとして利用します.
    TEXTURE_USAGE_SHININESS,    //!< シャイネスマップとして利用します.
    TEXTURE_USAGE_NORMAL,       //!< 法線マップとして利用します.

    TEXTURE_USAGE_COUNT
};

struct TextureData {
    std::string textureFilePath;
    int32_t textureSrvIndex = -1;
};

struct MaterialData {
    Vector3   diffuse = { 0.5f,0.5f,0.5f };        //!< 拡散反射成分です.
    Vector3   specular = { 1.0f,1.0f,1.0f };       //!< 鏡面反射成分です.
    float     alpha = 1.0f;          //!< 透過成分です.
    float     shininess = 0.0f;      //!< 鏡面反射強度です.
    std::array<TextureData, TEXTURE_USAGE_COUNT> textureData_;
};

namespace LoadMaterial {
    std::string Convert(const std::string& directoryPath, aiString textureFilePath);
    MaterialData LoadMaterialFromAssimp(aiMaterial* material, const std::string& directoryPath);
}

