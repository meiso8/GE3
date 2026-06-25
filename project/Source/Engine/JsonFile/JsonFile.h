#include "nlohmann/json.hpp"
#include <fstream>
#include<unordered_map>

using Json = nlohmann::json;

struct JsonData {
    Json data;
    std::string path;
    bool modified = false; // ← 変更されたかどうかを記録！
};
struct Vector3;
struct EulerTransform;

class JsonFile {
public:
    ~JsonFile();
    static Json& GetJsonFiles(const std::string& Tag) { return jsonFiles_[Tag].data; };
    static const std::unordered_map <std::string, JsonData > GetJsonData();
    static const size_t GetSize() { return jsonFiles_.size(); };
    static void SaveJson(const std::string& Tag);
    static void LoadJson(const std::string& Tag);
   static void LoadAllJsonFile();
    static void SetJson(const std::string& tag, const nlohmann::json& j);

    static void MarkModified(const std::string& tag) {
        jsonFiles_[tag].modified = true;
    }

    static bool IsModified(const std::string& tag) {
        return jsonFiles_[tag].modified;
    }

    static void ClearModified(const std::string& tag) {
        jsonFiles_[tag].modified = false;
    }

    static   nlohmann::json Vector3ToJson(const Vector3& vector);

    static  nlohmann::json EulerTransformToJson(const EulerTransform& eTransform);
private:
    static std::unordered_map <std::string, JsonData > jsonFiles_;

};