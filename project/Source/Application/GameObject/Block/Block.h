#pragma once
#include<array>
#include"../Platform/Platform.h"
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
    void Reset(const bool isCheckPushBlock = true);
    void RayCastHit(const bool isCheckPushBlock = true);
    
};

