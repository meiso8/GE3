#pragma once
#include"Block/Block.h"
#include"TextureFactory.h"
#include<vector>
#include<SoundManager/SoundManager.h>

class BlockMap {
public:
    static const int kMapWidth = 8;
    static const int kMapHeight = 8;
private:
    std::array<Block*, 4> centerBlocks_;
    std::array<Block*, 12> roundBlocks_;
    std::array < std::array<std::unique_ptr<Block>, kMapWidth>, kMapHeight >map_;
    std::vector<TextureFactory::Handle> correctOrder_ = { TextureFactory::HIERO_S, TextureFactory::HIERO_P, TextureFactory::HIERO_D, TextureFactory::HIERO_T };
    std::vector<TextureFactory::Handle> steppedOrder_;
    bool isClear_ = false;
public:
    BlockMap();
    ~BlockMap();
    void Initialize();
    void Update();
    void Draw();
    const bool& IsClear() { return isClear_; };
    std::array < std::array<std::unique_ptr<Block>, kMapWidth>, kMapHeight >& GetMap() { return map_; };
    void ResetPushMap();
    void RayCastHit(RaySprite& raySprite);
private:
    void ClearSet();
};