#pragma once
#include"Vector3.h"
#include"Vector4.h"
#include<string>
#include"Transform.h"
#include<vector>

struct LevelData {

    struct ColliderData {
        Vector3 center = { 0.0f,0.0f,0.0f };
        Vector3 size = { 1.0f,1.0f,1.0f };
        std::string tag ="unKown";
        uint32_t maskTag = 0xffffffff;
        std::string type = "";
    };

    struct MeshFileData {
        std::string fileName;
        std::string directoryPath;
    };

    struct MaterialData {
        uint32_t textureHandle;
        Vector4 color;
        float tempareture;
        float shininess;
        float environmentCoefficient;
    };

    struct ObjectData {
     
        MaterialData materialData;
        std::string objectName;
        MeshFileData filePath;
        std::string meshTypeName;
        EulerTransform transform;
        ColliderData colliderData;

    };

    struct PlayerSpawnData {
        EulerTransform transform;
    };

    struct EnemySpawnData {
        float tempareture;
        MeshFileData filePath;
        EulerTransform transform;
    };

    struct StageChangeTriggerData {
        MaterialData materialData;
        std::string nextStageName;
        MeshFileData filePath;
        EulerTransform transform;
        ColliderData colliderData;
    };

    std::vector<ObjectData>objects;
    //自キャラ配列
    std::vector<PlayerSpawnData>players;
    //敵キャラ配列
    std::vector<EnemySpawnData>enemies;
    //シーン遷移トリガー配列
    std::vector<StageChangeTriggerData>stageChangeTriggers_;
};