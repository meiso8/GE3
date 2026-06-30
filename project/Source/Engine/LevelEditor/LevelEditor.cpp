#include "LevelEditor.h"
#include"Model.h"
#include<ctype.h>
#include"../Mesh/PrimitiveFactory/PrimitiveFactory.h"

const std::array<std::string, LevelEditor::kObjectTypeNames>  LevelEditor::objectTypeName_ = { "MESH","PlayerSpawn","EnemySpawn","StageChangeTrigger" };

void LevelEditor::Load(const std::string& fileName, bool useButtobiEditor)
{
    // =============================JSONファイルを読み込んでみる=============================

    const std::string fullpath = kDefaultBaseFirectory + fileName + kExtension;

    std::ifstream file;

    file.open(fullpath);

    if (file.fail()) {
        assert(0);
    }

    // =============================ファイルチェック========================================

    nlohmann::json deserialized;

    file >> deserialized;

    assert(deserialized.is_object());
    assert(deserialized.contains("name"));
    assert(deserialized["name"].is_string());

    //"name"を文字列として取得
    std::string name =
        deserialized["name"].get<std::string>();
    //正しいレベルデータファイルかチェック
    std::string toLowerName;
    for (auto& w : name) {
        toLowerName += std::tolower(w);
    }

    //sceneが含まれていたら
    assert(toLowerName.find("scene") != std::string::npos);

    // =============================オブジェクト走査========================================

    levelData_ = std::make_unique<LevelData>();

    levelData_->objects.reserve(kMaxObjectCount_);

    if (useButtobiEditor) {
        vector3Name_ = { "x","y","z" };
        transformsName_ = { "translate","rotate","scale" };

    } else {
        vector3Name_ = { "x","z","y" };
        transformsName_ = { "translation","rotation","scaling" };
    }

    //"objects"の全オブジェクトを走査
    for (nlohmann::json& object : deserialized["objects"]) {
        //無効かどうかのフラグ
        if (object.contains("disabled")) {
            bool disabled = object["disabled"].get<bool>();

            if (disabled) {
                continue;
            }
        }

        LoadObject(object, levelData_.get());
    }

}

void LevelEditor::CreateObject(std::vector<std::unique_ptr<ObjectSet>>& objects)
{

    objects.clear();

    for (auto& objectData : levelData_->objects) {

        std::unique_ptr<ObjectSet> newObjctData = std::make_unique<ObjectSet>();
        auto filePath = objectData.filePath;

        Model* model = ModelManager::LoadModelAndGet(filePath.directoryPath + "/" + filePath.fileName);
       
        newObjctData->obj_ = std::make_unique<Object3d>();
        newObjctData->obj_->Create();

        if (model) {
            newObjctData->obj_->SetMeshAndMaterial(model);
        } else {
            auto* primitive = PrimitiveFactory::GetPrimitiveForName(objectData.filePath.fileName);
            newObjctData->obj_->SetMeshAndMaterial(primitive);
        }

        newObjctData->obj_->SetObjectName(objectData.objectName);
        newObjctData->obj_->RegisterObject();

        auto& transform = newObjctData->obj_->GetWorldTransform();

        transform.eTransform_ = objectData.transform;

        //コライダーの設定
        newObjctData->collider_ = std::make_unique<Collider>();
        newObjctData->collider_->SetCenter(objectData.colliderData.center);
        Vector3 size = objectData.colliderData.size;
        size *= 0.5f;
        newObjctData->collider_->SetAABB({ .min = -size ,.max = size });

        objects.push_back(std::move(newObjctData));
    }
}

void LevelEditor::CreateStageChangeTriggers(std::vector<std::unique_ptr<StageChangeTrigger>>& triggers)
{

    // 配列をクリア
    triggers.clear();

    for (auto& triggerData : levelData_->stageChangeTriggers_) {

        // インスタンスを作成
        std::unique_ptr<StageChangeTrigger> newTrigger = std::make_unique<StageChangeTrigger>();

        newTrigger->Create(
            triggerData.filePath.fileName,
            triggerData.filePath.directoryPath,
            triggerData.nextStageName,
            triggerData.transform,
            triggerData.colliderData.center,
            triggerData.colliderData.size
        );

        // 管理用配列に追加
        triggers.push_back(std::move(newTrigger));
    }

}


void LevelEditor::LoadObject(nlohmann::json& object, LevelData* levelData) {

    assert(object.contains("type"));
    //種別を取得
    std::string type = object["type"].get<std::string>();
    //種類ごとの処理

    //MESHがある場合
    if (type.compare(objectTypeName_[kMesh]) == 0) {
        //要素追加
        levelData->objects.emplace_back(LevelData::ObjectData{});
        LevelData::ObjectData& objectData = levelData->objects.back();

        //オブジェクト名の読み込み
        LoadName(objectData.objectName, object,"name");
        //メッシュファイルパスデータをロードする
        LoadMeshData(objectData.filePath, object);
        //トランスフォームのパラメータ読み込み
        LoadTransform(object, objectData.transform);
        //子要素の走査
        LoadChildren(object, levelData);
        //コライダーの読み込み
        LoadCollider(objectData.colliderData, object);

    } else if (type.compare(objectTypeName_[kPlayerSpawn]) == 0) {
        //要素追加
        levelData->players.emplace_back(LevelData::PlayerSpawnData{});
        LevelData::PlayerSpawnData& playerData = levelData->players.back();
        //トランスフォームのパラメータ読み込み
        LoadTransform(object, playerData.transform);

    } else if (type.compare(objectTypeName_[kEnemySpawn]) == 0) {
        //要素追加
        levelData->enemies.emplace_back(LevelData::EnemySpawnData{});
        LevelData::EnemySpawnData& enemyData = levelData->enemies.back();
        //トランスフォームのパラメータ読み込み
        LoadTransform(object, enemyData.transform);
        //子要素の走査
        LoadChildren(object, levelData);
        //メッシュファイルパスデータをロードする
        LoadMeshData(enemyData.filePath, object);

    } else if (type.compare(objectTypeName_[kStageChangeTrigger]) == 0) {
        //要素追加
        levelData->stageChangeTriggers_.emplace_back(LevelData::StageChangeTriggerData{});
        LevelData::StageChangeTriggerData& stageChangeTriggerData = levelData->stageChangeTriggers_.back();
        //次のステージ名を記録
        LoadName(stageChangeTriggerData.nextStageName, object, "nextStageName");
        //メッシュファイルパスデータをロードする
        LoadMeshData(stageChangeTriggerData.filePath,object);
        //トランスフォームのパラメータ読み込み
        LoadTransform(object, stageChangeTriggerData.transform);
        //コライダーの読み込み
        LoadCollider(stageChangeTriggerData.colliderData, object);
    }

}

void LevelEditor::LoadName(std::string& fileName, nlohmann::json& object, const std::string& loadName)
{
    if (object.contains(loadName)) {
        fileName = object[loadName];
    }
}

void LevelEditor::LoadMeshData(LevelData::MeshFileData& meshData, nlohmann::json& object)
{        //ファイル名の読み込み
    LoadName(meshData.fileName, object);
    //ファイルディレクトリの読み込み
    LoadName(meshData.directoryPath, object, "directoryPath");
}

void LevelEditor::LoadTransform(nlohmann::json& object, EulerTransform& transform)
{

    if (object.contains("transform")) {
        nlohmann::json& loadTransform = object["transform"];
        //それぞれ座標系を合わせるため、yzの入れ替えを行っている
        //平行移動
        uint32_t transformIndex = 0;

        transform.translate.x = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[0]];
        transform.translate.y = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[1]];
        transform.translate.z = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[2]];
        transformIndex++;
        //回転角 軸回転方向を変換しておく

        transform.rotate.x = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[0]];
        transform.rotate.y = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[1]];
        transform.rotate.z = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[2]];
        transformIndex++;

        if (vector3Name_[2] == "y") {
            transform.rotate.x *= -1.0f;
            transform.rotate.z *= -1.0f;
        }

        //スケーリング
        transform.scale.x = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[0]];
        transform.scale.y = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[1]];
        transform.scale.z = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[2]];

    }
}

void LevelEditor::LoadChildren(nlohmann::json& object, LevelData* levelData)
{
    //オブジェクト走査を再起関数にまとめ、再帰呼び出して枝を走査する
    if (object.contains("children")) {

        for (nlohmann::json& child : object["children"]) {
            LoadObject(child, levelData);
        }
    }
}

void LevelEditor::LoadCollider(LevelData::ColliderData& colliderData, nlohmann::json& object)
{
    //コライダーの読み込み
    if (object.contains("collider")) {

        nlohmann::json& collider = object["collider"];

        colliderData.center = {
                (float)collider["center"][vector3Name_[0]],
                (float)collider["center"][vector3Name_[1]],
                (float)collider["center"][vector3Name_[2]],
        };
        colliderData.size = {
            .x = (float)collider["size"][vector3Name_[0]] ,
            .y = (float)collider["size"][vector3Name_[1]] ,
            .z = (float)collider["size"][vector3Name_[2]]
        };

    }
}


