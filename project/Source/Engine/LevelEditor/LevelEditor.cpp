#include "LevelEditor.h"
#include"Model.h"
#include<ctype.h>
#include"../Mesh/PrimitiveFactory/PrimitiveFactory.h"

const std::array<std::string, LevelEditor::kObjectTypeNames>  LevelEditor::objectTypeName_ = { "MESH","PlayerSpawn","EnemySpawn","StageChangeTrigger" };

void LevelEditor::Load(const std::string& fileName, bool useButtobiEditor)
{
    // =============================JSON繝輔ぃ繧､繝ｫ繧定ｪｭ縺ｿ霎ｼ繧薙〒縺ｿ繧・============================

    const std::string fullpath = kDefaultBaseFirectory + fileName + kExtension;

    std::ifstream file;

    file.open(fullpath);

    if (file.fail()) {
        assert(0);
    }

    // =============================繝輔ぃ繧､繝ｫ繝√ぉ繝・け========================================

    nlohmann::json deserialized;

    file >> deserialized;

    assert(deserialized.is_object());
    assert(deserialized.contains("name"));
    assert(deserialized["name"].is_string());

    //"name"繧呈枚蟄怜・縺ｨ縺励※蜿門ｾ・
    std::string name =
        deserialized["name"].get<std::string>();
    //豁｣縺励＞繝ｬ繝吶Ν繝・・繧ｿ繝輔ぃ繧､繝ｫ縺九メ繧ｧ繝・け
    std::string toLowerName;
    for (auto& w : name) {
        toLowerName += std::tolower(w);
    }

    //scene縺悟性縺ｾ繧後※縺・◆繧・
    assert(toLowerName.find("scene") != std::string::npos);

    // =============================繧ｪ繝悶ず繧ｧ繧ｯ繝郁ｵｰ譟ｻ========================================

    levelData_ = std::make_unique<LevelData>();

    levelData_->objects.reserve(kMaxObjectCount_);

    if (useButtobiEditor) {
        vector3Name_ = { "x","y","z" };
        transformsName_ = { "translate","rotate","scale" };

    } else {
        vector3Name_ = { "x","z","y" };
        transformsName_ = { "translation","rotation","scaling" };
    }

    //"objects"縺ｮ蜈ｨ繧ｪ繝悶ず繧ｧ繧ｯ繝医ｒ襍ｰ譟ｻ
    for (nlohmann::json& object : deserialized["objects"]) {
        //辟｡蜉ｹ縺九←縺・°縺ｮ繝輔Λ繧ｰ
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

        Model* model = nullptr;

        if (!filePath.directoryPath.empty()) {
            model = ModelManager::LoadModelAndGet(filePath.directoryPath + "/" + filePath.fileName);
        }
      
        newObjctData->obj_ = std::make_unique<Object3d>();
        newObjctData->obj_->Create();
        newObjctData->obj_->SetObjectType("MESH");
        
        if (model) {
            newObjctData->obj_->SetMeshAndMaterial(model);
        } else {
            auto* primitive = PrimitiveFactory::GetPrimitiveForName(objectData.filePath.fileName);
            newObjctData->obj_->SetMeshAndMaterial(primitive);
            //繝・け繧ｹ繝√Ε繝上Φ繝峨Ν縺ｮ繧ｻ繝・ヨ
            newObjctData->obj_->SetTextureHandle(static_cast<TextureFactory::Handle>(objectData.textureHandle));
        }

        newObjctData->obj_->SetColor(objectData.color);
        newObjctData->obj_->SetTemperature(objectData.tempareture);
        newObjctData->obj_->SetObjectName(objectData.objectName);
        newObjctData->obj_->RegisterObject();

        auto& transform = newObjctData->obj_->GetWorldTransform();

        transform.eTransform_ = objectData.transform;

        //繧ｳ繝ｩ繧､繝繝ｼ縺ｮ險ｭ螳・
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

    // 驟榊・繧偵け繝ｪ繧｢
    triggers.clear();

    for (auto& triggerData : levelData_->stageChangeTriggers_) {

        // 繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ繧剃ｽ懈・
        std::unique_ptr<StageChangeTrigger> newTrigger = std::make_unique<StageChangeTrigger>();
        newTrigger->Create(triggerData);
        // 邂｡逅・畑驟榊・縺ｫ霑ｽ蜉
        triggers.push_back(std::move(newTrigger));
    }

}


void LevelEditor::LoadVector4(nlohmann::json& object, Vector4& vector)
{      
    //濶ｲ縺ｮ蜿門ｾ・
    if (object.contains("color")) {
        vector = {
            .x = (float)object["color"]["x"] ,
            .y = (float)object["color"]["y"] ,
            .z = (float)object["color"]["z"] ,
            .w = (float)object["color"]["w"]
        };
    } else {
        vector = { 1.0f,1.0f,1.0f,1.0f };
    }
}

void LevelEditor::LoadObject(nlohmann::json& object, LevelData* levelData) {

    assert(object.contains("type"));
    //遞ｮ蛻･繧貞叙蠕・
    std::string type = object["type"].get<std::string>();
    //遞ｮ鬘槭＃縺ｨ縺ｮ蜃ｦ逅・

    //MESH縺後≠繧句ｴ蜷・
    if (type.compare(objectTypeName_[kMesh]) == 0) {

        //隕∫ｴ霑ｽ蜉
        levelData->objects.emplace_back(LevelData::ObjectData{});
        LevelData::ObjectData& objectData = levelData->objects.back();
        
        //繝・け繧ｹ繝√Ε繝上Φ繝峨Ν縺ｮ隱ｭ縺ｿ霎ｼ縺ｿ
        LoadTextureHandle(object, objectData.textureHandle);
        //濶ｲ縺ｮ隱ｭ縺ｿ霎ｼ縺ｿ
        LoadVector4(object, objectData.color);
        //貂ｩ蠎ｦ縺ｮ隱ｭ縺ｿ霎ｼ縺ｿ
        LoadTempareture(objectData.tempareture, object);
        //繧ｪ繝悶ず繧ｧ繧ｯ繝亥錐縺ｮ隱ｭ縺ｿ霎ｼ縺ｿ
        LoadName(objectData.objectName, object, "name");
        //繝｡繝・す繝･繝輔ぃ繧､繝ｫ繝代せ繝・・繧ｿ繧偵Ο繝ｼ繝峨☆繧・
        LoadMeshData(objectData.filePath, object);
        //繝医Λ繝ｳ繧ｹ繝輔か繝ｼ繝縺ｮ繝代Λ繝｡繝ｼ繧ｿ隱ｭ縺ｿ霎ｼ縺ｿ
        LoadTransform(object, objectData.transform);
        //蟄占ｦ∫ｴ縺ｮ襍ｰ譟ｻ
        LoadChildren(object, levelData);
        //繧ｳ繝ｩ繧､繝繝ｼ縺ｮ隱ｭ縺ｿ霎ｼ縺ｿ
        LoadCollider(objectData.colliderData, object);

    } else if (type.compare(objectTypeName_[kPlayerSpawn]) == 0) {
        //隕∫ｴ霑ｽ蜉
        levelData->players.emplace_back(LevelData::PlayerSpawnData{});
        LevelData::PlayerSpawnData& playerData = levelData->players.back();
        //繝医Λ繝ｳ繧ｹ繝輔か繝ｼ繝縺ｮ繝代Λ繝｡繝ｼ繧ｿ隱ｭ縺ｿ霎ｼ縺ｿ
        LoadTransform(object, playerData.transform);

    } else if (type.compare(objectTypeName_[kEnemySpawn]) == 0) {
        //隕∫ｴ霑ｽ蜉
        levelData->enemies.emplace_back(LevelData::EnemySpawnData{});
        LevelData::EnemySpawnData& enemyData = levelData->enemies.back();
     
        //貂ｩ蠎ｦ縺ｮ隱ｭ縺ｿ霎ｼ縺ｿ
        LoadTempareture(enemyData.tempareture, object);
        //繝医Λ繝ｳ繧ｹ繝輔か繝ｼ繝縺ｮ繝代Λ繝｡繝ｼ繧ｿ隱ｭ縺ｿ霎ｼ縺ｿ
        LoadTransform(object, enemyData.transform);
        //蟄占ｦ∫ｴ縺ｮ襍ｰ譟ｻ
        LoadChildren(object, levelData);
        //繝｡繝・す繝･繝輔ぃ繧､繝ｫ繝代せ繝・・繧ｿ繧偵Ο繝ｼ繝峨☆繧・
        LoadMeshData(enemyData.filePath, object);

    } else if (type.compare(objectTypeName_[kStageChangeTrigger]) == 0) {
        //隕∫ｴ霑ｽ蜉
        levelData->stageChangeTriggers_.emplace_back(LevelData::StageChangeTriggerData{});
        LevelData::StageChangeTriggerData& stageChangeTriggerData = levelData->stageChangeTriggers_.back();
        //繝・け繧ｹ繝√Ε繝上Φ繝峨Ν縺ｮ隱ｭ縺ｿ霎ｼ縺ｿ
        LoadTextureHandle(object,stageChangeTriggerData.textureHandle);
        //濶ｲ縺ｮ隱ｭ縺ｿ霎ｼ縺ｿ
        LoadVector4(object, stageChangeTriggerData.color);
        //貂ｩ蠎ｦ縺ｮ隱ｭ縺ｿ霎ｼ縺ｿ
        LoadTempareture(stageChangeTriggerData.tempareture, object);
        //谺｡縺ｮ繧ｹ繝・・繧ｸ蜷阪ｒ險倬鹸
        LoadName(stageChangeTriggerData.nextStageName, object, "nextStageName");
        //繝｡繝・す繝･繝輔ぃ繧､繝ｫ繝代せ繝・・繧ｿ繧偵Ο繝ｼ繝峨☆繧・
        LoadMeshData(stageChangeTriggerData.filePath, object);
        //繝医Λ繝ｳ繧ｹ繝輔か繝ｼ繝縺ｮ繝代Λ繝｡繝ｼ繧ｿ隱ｭ縺ｿ霎ｼ縺ｿ
        LoadTransform(object, stageChangeTriggerData.transform);
        //繧ｳ繝ｩ繧､繝繝ｼ縺ｮ隱ｭ縺ｿ霎ｼ縺ｿ
        LoadCollider(stageChangeTriggerData.colliderData, object);
    }

}

void LevelEditor::LoadTextureHandle(nlohmann::json& object, uint32_t& handle)
{
    if (object.contains("textureHandle")) {

        handle = object["textureHandle"];
    } else {
        handle = TextureFactory::WHITE_1X1;
    }

}

void LevelEditor::LoadName(std::string& fileName, nlohmann::json& object, const std::string& loadName)
{
    if (object.contains(loadName)) {
        fileName = object[loadName];
    }
}

void LevelEditor::LoadMeshData(LevelData::MeshFileData& meshData, nlohmann::json& object)
{        //繝輔ぃ繧､繝ｫ蜷阪・隱ｭ縺ｿ霎ｼ縺ｿ
    LoadName(meshData.fileName, object);
    //繝輔ぃ繧､繝ｫ繝・ぅ繝ｬ繧ｯ繝医Μ縺ｮ隱ｭ縺ｿ霎ｼ縺ｿ
    LoadName(meshData.directoryPath, object, "directoryPath");
}

void LevelEditor::LoadTempareture(float& tempareture, nlohmann::json& object)
{
    if (object.contains("temperature")) {
        tempareture = object["temperature"];
    } else {
        tempareture = 0.0f;
    }
}

void LevelEditor::LoadTransform(nlohmann::json& object, EulerTransform& transform)
{

    if (object.contains("transform")) {
        nlohmann::json& loadTransform = object["transform"];
        //縺昴ｌ縺槭ｌ蠎ｧ讓咏ｳｻ繧貞粋繧上○繧九◆繧√【z縺ｮ蜈･繧梧崛縺医ｒ陦後▲縺ｦ縺・ｋ
        //蟷ｳ陦檎ｧｻ蜍・
        uint32_t transformIndex = 0;

        transform.translate.x = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[0]];
        transform.translate.y = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[1]];
        transform.translate.z = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[2]];
        transformIndex++;
        //蝗櫁ｻ｢隗・霆ｸ蝗櫁ｻ｢譁ｹ蜷代ｒ螟画鋤縺励※縺翫￥

        transform.rotate.x = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[0]];
        transform.rotate.y = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[1]];
        transform.rotate.z = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[2]];
        transformIndex++;

        if (vector3Name_[2] == "y") {
            transform.rotate.x *= -1.0f;
            transform.rotate.z *= -1.0f;
        }

        //繧ｹ繧ｱ繝ｼ繝ｪ繝ｳ繧ｰ
        transform.scale.x = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[0]];
        transform.scale.y = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[1]];
        transform.scale.z = (float)loadTransform[transformsName_[transformIndex]][vector3Name_[2]];

    }
}

void LevelEditor::LoadChildren(nlohmann::json& object, LevelData* levelData)
{
    //繧ｪ繝悶ず繧ｧ繧ｯ繝郁ｵｰ譟ｻ繧貞・襍ｷ髢｢謨ｰ縺ｫ縺ｾ縺ｨ繧√∝・蟶ｰ蜻ｼ縺ｳ蜃ｺ縺励※譫昴ｒ襍ｰ譟ｻ縺吶ｋ
    if (object.contains("children")) {

        for (nlohmann::json& child : object["children"]) {
            LoadObject(child, levelData);
        }
    }
}

void LevelEditor::LoadCollider(LevelData::ColliderData& colliderData, nlohmann::json& object)
{
    //繧ｳ繝ｩ繧､繝繝ｼ縺ｮ隱ｭ縺ｿ霎ｼ縺ｿ
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


