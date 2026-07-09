#pragma once
#include"Memo.h"
#include<map>
#include"Sprite.h"
#include"Player/RaySprite.h"
#include"Vector2.h"

class MemoManager
{
private:
    std::map< TextureFactory::Handle, std::unique_ptr<Memo>>memos_;
    std::unique_ptr<Sprite> sprite_ = nullptr;
    Vector2 spriteScale_ = { 1.0f,1.0f };
public:
    static bool isLookItem_;
    MemoManager();
    ~MemoManager();
    void Initialize();
    void Update();
    void Draw(Camera& camera);
    void DrawUI();
    void GenerateMemos(const std::vector<TextureFactory::Handle>& handles);

    void SetSpriteSize(const TextureFactory::Handle& handle);
    void RayCastHit(RaySprite& raySprite);
    std::map<TextureFactory::Handle, std::unique_ptr<Memo>>& GetMemos() { return memos_; }
};
