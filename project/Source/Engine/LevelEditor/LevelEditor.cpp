#include "LevelEditor.h"
#include"Model.h"
#include<ctype.h>
#include"../Mesh/PrimitiveFactory/PrimitiveFactory.h"


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
        vector3Name_ = { "x","z","y" };
        transformsName_ = { "translate","rotate","scale" };

    } else {
        vector3Name_ = { "x","y","z" };
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
    for (auto& objectData : levelData_->objects) {

        std::unique_ptr<ObjectSet> newObjctData = std::make_unique<ObjectSet>();
     
        Model* model = ModelManager::GetModel(objectData.fileName);

        newObjctData->obj_ = std::make_unique<Object3d>();
        newObjctData->obj_->Create();

        if (model) {
            newObjctData->obj_->SetMeshAndMaterial(model);
        } else {
          auto* primitive =  PrimitiveFactory::GetPrimitiveForName(objectData.fileName);
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


void LevelEditor::LoadObject(nlohmann::json& object, LevelData* levelData) {

    assert(object.contains("type"));
    //種別を取得
    std::string type = object["type"].get<std::string>();
    //種類ごとの処理

    //MESHがある場合
    if (type.compare("MESH") == 0) {
        //要素追加
        levelData->objects.emplace_back(LevelData::ObjectData{});
        LevelData::ObjectData& objectData = levelData->objects.back();

        if (object.contains("name")) {
            objectData.objectName = object["name"];
        }

        if (object.contains("file_name")) {
            objectData.fileName = object["file_name"];
        }

        //トランスフォームのパラメータ読み込み
        LoadTransform(object, objectData.transform);

        //コライダーの読み込み
        if (object.contains("collider")) {

            nlohmann::json& collider = object["collider"];

            objectData.colliderData.center = {
                    (float)collider["center"][0],
                    (float)collider["center"][2],
                    (float)collider["center"][1],
            };
            objectData.colliderData.size = {
                .x = (float)collider["size"][0] ,
                .y = (float)collider["size"][2] ,
                .z = (float)collider["size"][1]
            };

        }



        //オブジェクト走査を再起関数にまとめ、再帰呼び出して枝を走査する
        if (object.contains("children")) {

            for (nlohmann::json& child : object["children"]) {
                LoadObject(child, levelData);
            }
        }
    } else if (type.compare("PlayerSpawn") == 0) {

        //要素追加
        levelData->players.emplace_back(LevelData::PlayerSpawnData{});
        LevelData::PlayerSpawnData& playerData = levelData->players.back();
        //トランスフォームのパラメータ読み込み
        LoadTransform(object, playerData.transform);


    } else if (type.compare("EnemySpawn") == 0) {
        //要素追加
        levelData->enemies.emplace_back(LevelData::EnemySpawnData{});
        LevelData::EnemySpawnData& enemyData = levelData->enemies.back();

        if (object.contains("file_name")) {
            enemyData.fileName = object["file_name"];
        }
        //トランスフォームのパラメータ読み込み
        LoadTransform(object, enemyData.transform);


        //オブジェクト走査を再起関数にまとめ、再帰呼び出して枝を走査する
        if (object.contains("children")) {

            for (nlohmann::json& child : object["children"]) {
                LoadObject(child, levelData);
            }
        }

    }

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

        if (vector3Name_[2]=="y") {
            transform.rotate.x *= -1.0f;
            transform.rotate.z *= -1.0f;
        } 

        //スケーリング
        transform.scale.x = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[0]];
        transform.scale.y = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[1]];
        transform.scale.z = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[2]];

    }
    }




