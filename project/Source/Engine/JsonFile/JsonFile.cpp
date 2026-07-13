#include "JsonFile.h"
#include <filesystem>
#include"Vector3.h"
#include"Vector4.h"

#include"Transform.h"

std::unordered_map<std::string, JsonData>JsonFile::jsonFiles_;

void JsonFile::LoadAllJsonFile()
{
    std::string folder = "Resource/JsonFiles/";
    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (entry.path().extension() == ".json") {
            std::string filename = entry.path().stem().string(); // 拡張子なしのファイル名
            LoadJson(filename);
        }
    }
}

void JsonFile::LoadJson(const std::string& Tag) {

    if (jsonFiles_.contains(Tag)) {
        return;
    }

    jsonFiles_[Tag].path = "Resource/JsonFiles/" + Tag + ".json";
    std::ifstream file(jsonFiles_[Tag].path);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open JSON file: " + jsonFiles_[Tag].path);
    }

    file >> jsonFiles_[Tag].data;

}

JsonFile::~JsonFile()
{
    jsonFiles_.clear();
}

const std::unordered_map <std::string, JsonData >JsonFile::GetJsonData()
{
    return jsonFiles_;
}

void JsonFile::SaveJson(const std::string& Tag) {

    if (jsonFiles_.contains(Tag)) {
        std::ofstream file(jsonFiles_[Tag].path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open JSON file for writing: " + jsonFiles_[Tag].path);
        }
        file << jsonFiles_[Tag].data.dump(4); // インデント付きで保存（4スペース）
    } else {
        throw std::runtime_error("Tag is UnKnown");
    }

}
void JsonFile::SetJson(const std::string& tag, const nlohmann::json& j) {
    JsonData& data = jsonFiles_[tag];
    data.data = j;
    data.path = "Resource/JsonFiles/" + tag + ".json";
    SaveJson(tag);
}

nlohmann::json JsonFile::Vector3ToJson(const Vector3& vector)
{
    return {
        {"x", vector.x},
        {"y", vector.y},
        {"z", vector.z},
    };
}

nlohmann::json JsonFile::Vector4ToJson(const Vector4& vector)
{
    return {
       {"x", vector.x},
       {"y", vector.y},
       {"z", vector.z},
       {"w", vector.w},
    };
}

nlohmann::json JsonFile::EulerTransformToJson(const EulerTransform& eTransform)
{
    return {
        {"translate", Vector3ToJson(eTransform.translate)},
        {"rotate",    Vector3ToJson(eTransform.rotate)   },
        {"scale",     Vector3ToJson(eTransform.scale)    },
    };
}

EulerTransform JsonFile::JsonToEulerTransform(const nlohmann::json& obj)
{
    EulerTransform result = { 0.0f };
    result.translate = JsonToVector3(obj["translate"]);
    result.rotate = JsonToVector3(obj["rotate"]);
    result.scale = JsonToVector3(obj["scale"]);
    return result;
}

Vector3 JsonFile::JsonToVector3(const nlohmann::json& obj)
{
    Vector3 result = { 0.0f };
    result.x = obj["x"].get<float>();
    result.y = obj["y"].get<float>();
    result.z = obj["z"].get<float>();
    return result;
}
