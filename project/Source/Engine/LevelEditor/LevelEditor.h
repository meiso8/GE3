#pragma once
#include<string>
#include<vector>
#include "nlohmann/json.hpp"
#include"Transform.h"
#include<memory>
#include<fstream>
#include"Object3d/Object3d.h"
#include"../System/Collider.h"
#include "../../Application/GameObject/StageChangeTrigger/StageChangeTrigger.h"

#include<array>
struct LevelData {

    struct ColliderData {
        Vector3 center = { 0.0f,0.0f,0.0f };
        Vector3 size = { 1.0f,1.0f,1.0f };
    };

    struct ObjectData {
        std::string objectName;
        std::string fileName;
        std::string meshTypeName;
        EulerTransform transform;
        ColliderData colliderData;

    };

    struct PlayerSpawnData {
        EulerTransform transform;
    };

    struct EnemySpawnData {
        std::string fileName;
        EulerTransform transform;
    };

    struct StageChangeTriggerData {
        std::string nextStageName;
        std::string fileName;
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

class LevelEditor
{
public:

    struct ObjectSet {
        std::unique_ptr<Object3d> obj_ = nullptr;
        std::unique_ptr<Collider>collider_ = nullptr;
    };

    enum ObjectTypeName {
        kMesh,
        kPlayerSpawn,
        kEnemySpawn,
        kStageChangeTrigger,
        kObjectTypeNames,
    };

private:
    const std::string kDefaultBaseFirectory = "Resource/JsonFiles/";
    const std::string  kExtension = ".json";
    const uint32_t kMaxObjectCount_ = 1000;
    std::unique_ptr<LevelData>levelData_ = nullptr;
    std::array<std::string, 3> vector3Name_ = { "x","z","y" };
    std::array<std::string, 3> transformsName_ = { "translation","rotation","scaling" };
    static const std::array<std::string, kObjectTypeNames>  objectTypeName_;
public:
    static const std::array<std::string, kObjectTypeNames>GetObjectTypeName() { return  objectTypeName_; };
    LevelData* GetLevelData() { return levelData_.get(); };
    void Load(const std::string& fileName, bool useButtobiEditor = false);
    /// @brief オブジェクトの作成関数
    /// @param objects 
    void CreateObject(std::vector<std::unique_ptr<ObjectSet>>& objects);
    void CreateStageChangeTriggers(std::vector<std::unique_ptr<StageChangeTrigger>>& triggers);
private:

    void LoadObject(nlohmann::json& object, LevelData* levelData);
    /// @brief 名前の読み込み
    /// @param fileName セットする名前の変数
    /// @param object オブジェクト
    /// @param loadName どの名前を読み込むか
    void LoadName(std::string& fileName, nlohmann::json& object, const std::string& loadName = "file_name");
    /// @brief 位置情報の読み込み
    /// @param object オブジェクト
    /// @param transform 位置情報
    void LoadTransform(nlohmann::json& object, EulerTransform& transform);
    /// @brief 子要素の走査
    /// @param object オブジェクト
    /// @param levelData レベルデータ
    void LoadChildren(nlohmann::json& object, LevelData* levelData);
    /// @brief 衝突判定のロード
    /// @param object オブジェクト
    /// @param levelData　レベルデータ 
    void LoadCollider(LevelData::ColliderData& colliderData, nlohmann::json& object);
};

