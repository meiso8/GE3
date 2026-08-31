#pragma once

#include"Block/Block.h"

class AnubisBlockMap
{
public:
    enum BlockColor {
        kRed,
        kBlue,
        kCyan,
        kGreen,
        kBlockColors,
    };
public:

    AnubisBlockMap();

    void Initialize();
    void Update();
    void Draw();
    const bool& IsClear() { return isClear_; };
    void ResetPushMap();
    void RayCastHit(RaySprite& raySprite);
    void ClearSet();
    std::array< std::unique_ptr<Block>, kBlockColors>& GetMap() { return blocks_; }
private:
    std::map<BlockColor, Vector4>colorMap_;
    std::array< std::unique_ptr<Block>, kBlockColors>blocks_;
    std::vector<int> correctOrder_ = { kBlue, kGreen, kCyan, kRed };
    std::vector<int> steppedOrder_;
    bool isClear_ = false;
};

