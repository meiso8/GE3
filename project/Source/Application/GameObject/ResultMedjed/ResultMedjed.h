#pragma once
#include"AnimationObject3d.h"
#include<memory>
#include<array>
#include<string>
class ResultMedjed
{
private:
    std::unique_ptr<AnimationObject3d>aniObj_ = nullptr;
    std::array<std::string,6>anilists_;
    uint32_t preAnimNum_ = 0;
    uint32_t currentAnimNum_ = 0;
public:
    ResultMedjed();
    void Initialize();
    void Update(const bool isResetAnim);
    void Draw(Camera& camera);
};

