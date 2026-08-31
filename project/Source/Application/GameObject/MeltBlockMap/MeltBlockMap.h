#pragma once
#include"Block/Block.h"
class MeltBlockMap
{
    //tempareture
    enum BlockTempareture {
        kMiddle_Low,
        kMiddle_High,
        kLow,
        kHigh,
        kMaxBolckTempareture,
    };
public:
    MeltBlockMap();
    void Initialize();
    void Update();
    void Draw();

    const bool& IsClear() { return isClear_; };
    void ResetPushMap();
    void RayCastHit(RaySprite& raySprite);
    void ClearSet();
    std::array< std::unique_ptr<Block>, kMaxBolckTempareture>& GetMap() { return blocks_; }
private:
    std::map<int, Vector4>colorMap_;
    //温度マップ
    std::map<BlockTempareture, float>temparetureMap_;
    //ブロック群
    std::array< std::unique_ptr<Block>, kMaxBolckTempareture>blocks_;
    //順番
    std::vector<int> steppedOrder_;
    //正しい順番
    std::vector<int> correctOrder_ = { kLow, kMiddle_Low, kMiddle_High, kHigh };
    //クリアフラグ
    bool isClear_ = false;
};

