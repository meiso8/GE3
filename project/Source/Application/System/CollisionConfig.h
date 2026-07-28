#pragma once
#include <cstdint>
#include<unordered_map>
#include<string>

class CollisionTag {
public:
    static void AddTag(const std::string& tagName);
    static uint32_t GetTag(const std::string& tagName);
    static const std::string GetTagName(uint32_t tagNum);
    static std::unordered_map<std::string, uint32_t>& GetAllTags() { return tags_; };
private:
    static uint32_t useIndex_;
    static const uint32_t maxCount_;
    static std::unordered_map<std::string, uint32_t>tags_;
    //逆引き用
    static std::unordered_map< uint32_t, std::string>names_;
};

class TagFactory {
public:
    void SetTag();
};
