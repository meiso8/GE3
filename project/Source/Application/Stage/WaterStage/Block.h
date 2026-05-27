#pragma once
#include<array>
#include"../Platform.h"
#include<memory>
#include"Player/RaySprite.h"
class Block :public Platform
{
private:
    bool isPush_ = false;
    float startPosY_ = { 0.0f };
    float endPosY_ = { 0.0f };
    float aniTimer_ = 0.0f;

public:
    void SetTemperature(const float temp) { object_->SetTemperature(temp); };
    Block();
    void SetIsPush(const bool& isPush) { isPush_ = isPush; }
    const bool& GetIsPush() { return isPush_; };
    bool CanPushBlock(){     // 通常ブロックなら無視 
        if (GetSrvIndex() == Texture::GetSRVHandle(TextureFactory::PUZZLE) ||
            GetSrvIndex() == Texture::GetSRVHandle(TextureFactory::NONE)) {
            return false;
        };
        return true;
    }
    void Initialize()override;
    void Update()override;
    //void Draw(Camera& camera);
    void OnCollision(Collider* collider)override;
    void SetPos(const Vector3& pos, const float& endOffset = -0.5f);
    void SetEndPos(const float& endOffset = -0.5f);
    void InitAnitimer();
    void Reset();
    void RayCastHit();
};

class BlockMap {
public:
    static const int kMapWidth = 8;
    static const int kMapHeight = 8;
    BlockMap();
    void Initialize();
    void Update();
    void Draw(Camera& camera);
    const bool& IsClear() { return isClear_; };
    std::array < std::array<std::unique_ptr<Block>, kMapWidth>, kMapHeight >& GetMap() { return map_; };
    void ResetPushMap();
    void RayCastHit(RaySprite& raySprite);
private:
    std::array<Block*, 4> centerBlocks_;
    std::array<Block*, 12> roundBlocks_;
    std::array < std::array<std::unique_ptr<Block>, kMapWidth>, kMapHeight >map_;
    std::vector<TextureFactory::Handle> correctOrder_ = { TextureFactory::HIERO_S, TextureFactory::HIERO_P, TextureFactory::HIERO_D, TextureFactory::HIERO_T };
    std::vector<TextureFactory::Handle> steppedOrder_;
    bool isClear_ = false;
    void ClearSet();
};