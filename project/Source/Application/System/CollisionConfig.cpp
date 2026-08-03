#include "CollisionConfig.h"
#include"JsonFile.h"

uint32_t CollisionTag::useIndex_ = 0;
const uint32_t CollisionTag::maxCount_ = 1000;
std::unordered_map<std::string, uint32_t>CollisionTag::tags_;
std::unordered_map< uint32_t, std::string>CollisionTag::names_;

void CollisionTag::AddTag(const std::string& tagName)
{
    if (tags_.contains(tagName) || useIndex_ >= maxCount_) {
        return;
    };

    useIndex_++;

    tags_[tagName] = 0b1 << useIndex_;
    names_[tags_[tagName]] = tagName;
}

uint32_t CollisionTag::GetTag(const std::string& tagName)
{
    if (tags_.contains(tagName)) {
        return tags_.at(tagName);
    };

    //何にも属さないを 返す
    return 0;
}

const std::string CollisionTag::GetTagName(uint32_t tagNum)
{
    if (names_.contains(tagNum)) {
        return names_.at(tagNum);
    }

    return "unKnown";
}

void CollisionTag::SaveTagNames()
{
    nlohmann::json  json;

    for (auto& [name, tag] : tags_) {
        nlohmann::json tagName = { "name" ,name };
        json.push_back(tagName );
    }

    JsonFile::SetJson("TagNames", json);
}

void CollisionTag::LoadTagNames()
{
    auto& json = JsonFile::GetJsonFiles("TagNames");

    for (auto& [name, tag] : tags_) {

        //タグ名が既に存在するときスキップする
        if (name.c_str() == json["name"]) {
            continue;
        }
        //タグを追加する
        AddTag(name);
    }

}

void TagFactory::SetTag()
{

    CollisionTag::SaveTagNames();

    CollisionTag::AddTag("Player");
    CollisionTag::AddTag("PlayerBulletCold");
    CollisionTag::AddTag("PlayerBulletHot");

    CollisionTag::AddTag("Enemy");
    CollisionTag::AddTag("EnemyBulletCold");
    CollisionTag::AddTag("EnemyBulletHot");

    CollisionTag::AddTag("Medjed");
    CollisionTag::AddTag("DummyMedjed");
    CollisionTag::AddTag("Mummy");

    CollisionTag::AddTag("Wall");
    CollisionTag::AddTag("Floor");
    CollisionTag::AddTag("Water");
    CollisionTag::AddTag("Block");

    CollisionTag::AddTag("Item");
    CollisionTag::AddTag("Memo");

    CollisionTag::AddTag("StageTrigger");
    CollisionTag::AddTag("CameraUp");
    CollisionTag::LoadTagNames();

}
