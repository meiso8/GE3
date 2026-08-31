#pragma once
#include"Block/Block.h"

class Camera;
class BastetBlockMap
{
public:
    enum BlockHz {
        kC,
        kC_S,
        kD,
        kD_S,
        kE,
        kF,
        kF_S,
        kG,
        kG_S,
        kA,
        kA_S,
        kB,
        kC_H,
        kMaxHz,
    };
public:

    BastetBlockMap();

    void Initialize();
    void Update();
    void Draw();
    const bool& IsClear() { return isClear_; };
    void ResetPushMap();
    void RayCastHit(RaySprite& raySprite);
    void ClearSet();
    std::array< std::unique_ptr<Block>, kMaxHz>& GetMap() { return blocks_; }
private:
    std::array<bool, kMaxHz>isPushs_;
    std::map<BlockHz, Vector4>colorMap_;
    std::array< Block*,8>whiteMap_;
    std::array<Block*,5>blackMap_;
    std::array< std::unique_ptr<Block>, kMaxHz>blocks_;
    std::vector<int> correctOrder_ = { kC, kD, kD_S, kF_S,kG,kG_S,kB,kC_H };
    std::vector<int> steppedOrder_;
    bool isClear_ = false;
};


