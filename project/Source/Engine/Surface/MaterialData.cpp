#include"MaterialData.h"


std::string LoadMaterial::Convert(const std::filesystem::path& directoryPath, aiString textureFilePath)
{
   
    return directoryPath.string() + "/" + textureFilePath.C_Str();
}

MaterialData LoadMaterial::LoadMaterialFromAssimp(aiMaterial * material, const  std::filesystem::path& directoryPath)
{

    MaterialData materialData;

    // ★追加：拡散反射成分（ディフューズ色／ベースカラー）の取得
    {
        aiColor3D color(1.0f, 1.0f, 1.0f); // 取得失敗時のデフォルトは白
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
        {
            materialData.diffuse.x = color.r;
            materialData.diffuse.y = color.g;
            materialData.diffuse.z = color.b;
        } else
        {
            materialData.diffuse = { 1.0f, 1.0f, 1.0f };
        }
    }

    // 鏡面反射成分.
    {
        aiColor3D color(0.0f, 0.0f, 0.0f);

        if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
        {
            materialData.specular.x = color.r;
            materialData.specular.y = color.g;
            materialData.specular.z = color.b;
        } else
        {
            materialData.specular.x = 0.0f;
            materialData.specular.y = 0.0f;
            materialData.specular.z = 0.0f;
        }
    }

    // 鏡面反射強度.
    {
        auto shininess = 0.0f;
        if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
        {
            materialData.shininess = shininess;
        } else
        {
            materialData.shininess = 0.0f;
        }
    }

    // ディフューズマップ（glTFのBASE_COLOR対応含む）
    {
        aiString path;
        // まず標準的なDIFFUSEを試す
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
        {
            materialData.textureData_[TEXTURE_USAGE_DIFFUSE].textureFilePath = LoadMaterial::Convert(directoryPath, path);
        }
        // 失敗した場合、glTF特有のBASE_COLORを試す
        else if (material->GetTexture(aiTextureType_BASE_COLOR, 0, &path) == AI_SUCCESS)
        {
            materialData.textureData_[TEXTURE_USAGE_DIFFUSE].textureFilePath = LoadMaterial::Convert(directoryPath, path);
        } else
        {
            materialData.textureData_[TEXTURE_USAGE_DIFFUSE].textureFilePath.clear();
        }
    }

    ////MultiMaterialに対応していく場合などは改造が必要である
    //{
    //    aiString path;
    //    if (material->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
    //    {
    //        materialData.textureData_[TEXTURE_USAGE_DIFFUSE].textureFilePath = LoadMaterial::Convert(directoryPath, path);
    //    } else
    //    {
    //        materialData.textureData_[TEXTURE_USAGE_DIFFUSE].textureFilePath.clear();
    //    }
    //}

    // スペキュラーマップ.
    {
        aiString path;
        if (material->Get(AI_MATKEY_TEXTURE_SPECULAR(0), path) == AI_SUCCESS)
        {
            materialData.textureData_[TEXTURE_USAGE_SPECULAR].textureFilePath = LoadMaterial::Convert(directoryPath, path);
        } else
        {
            materialData.textureData_[TEXTURE_USAGE_SPECULAR].textureFilePath.clear();
        }
    }

    // シャイネスマップ.
    {
        aiString path;
        if (material->Get(AI_MATKEY_TEXTURE_SHININESS(0), path) == AI_SUCCESS)
        {
            materialData.textureData_[TEXTURE_USAGE_SHININESS].textureFilePath = LoadMaterial::Convert(directoryPath, path);
        } else
        {
            materialData.textureData_[TEXTURE_USAGE_SHININESS].textureFilePath.clear();
        }
    }

    // 法線マップ
    {
        aiString path;
        if (material->Get(AI_MATKEY_TEXTURE_NORMALS(0), path) == AI_SUCCESS)
        {
            materialData.textureData_[TEXTURE_USAGE_NORMAL].textureFilePath = LoadMaterial::Convert(directoryPath, path);
        } else
        {
            if (material->Get(AI_MATKEY_TEXTURE_HEIGHT(0), path) == AI_SUCCESS)
            {
                materialData.textureData_[TEXTURE_USAGE_NORMAL].textureFilePath = LoadMaterial::Convert(directoryPath, path);
            } else
            {
                materialData.textureData_[TEXTURE_USAGE_NORMAL].textureFilePath.clear();
            }
        }
    }


    return materialData;
}
