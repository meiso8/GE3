#pragma once
#include <cstdint>
#include<unordered_map>
#include<string>

class CollisionTag {
public:
    static void SetTag(const std::string& tagName);
    static uint32_t GetTag(const std::string& tagName);
private:
    static uint32_t useIndex_;
    static const uint32_t maxCount_;
    static std::unordered_map<std::string, uint32_t>tags_;
};

class TagFactory {
public:
    void SetTag();
};
