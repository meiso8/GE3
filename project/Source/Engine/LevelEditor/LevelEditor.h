#pragma once
#include"../LevelData/LevelData.h"

#include "nlohmann/json.hpp"

#include<memory>
#include<array>
#include<fstream>
#include<vector>

#include"Object3d/Object3d.h"
#include"../System/Collider.h"
#include "../../Application/GameObject/StageChangeTrigger/StageChangeTrigger.h"

class StageChangeTrigger;

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

private:
    LevelEditor() = default;
    ~LevelEditor() = default;

public:

    static LevelEditor* GetInstance() {
        static LevelEditor instance;
        return &instance;
    }

    LevelEditor(const LevelEditor&) = delete;
    LevelEditor& operator=(const LevelEditor&) = delete;
   
    static const std::array<std::string, kObjectTypeNames>GetObjectTypeName() { return  objectTypeName_; };
    LevelData* GetLevelData() { return levelData_.get(); };
    void Load(const std::string& fileName, bool useButtobiEditor = false);
    /// @brief オブジェクトの作成関数
    /// @param objects json
    void CreateObject(std::vector<std::unique_ptr<ObjectSet>>& objects);
    /// @brief ステージ移動トリガーの作成関数
    /// @param triggers ステージトリガーのvector
    void CreateStageChangeTriggers(std::vector<std::unique_ptr<StageChangeTrigger>>& triggers);
private:
    void LoadObject(nlohmann::json& object, LevelData* levelData);
    /// @brief 名前の読み込み
    /// @param fileName セットする名前の変数
    /// @param object json
    /// @param loadName どの名前を読み込むか
    void LoadName(std::string& fileName, nlohmann::json& object, const std::string& loadName = "file_name");
    /// @brief メッシュ情報の読み込み
    /// @param meshData メッシュ情報
    /// @param object json
    void LoadMeshData(LevelData::MeshFileData& meshData, nlohmann::json& object);
    void LoadTempareture(float& tempareture, nlohmann::json& object);
    /// @brief 位置情報の読み込み
    /// @param object json
    /// @param transform 位置情報
    void LoadTransform(nlohmann::json& object, EulerTransform& transform);
    /// @brief 子要素の走査
    /// @param object json
    /// @param levelData レベルデータ
    void LoadChildren(nlohmann::json& object, LevelData* levelData);
    /// @brief 衝突判定のロード
    /// @param object json
    /// @param levelData　レベルデータ 
    void LoadCollider(LevelData::ColliderData& colliderData, nlohmann::json& object);
};

