#include "MemoManager.h"
#include"JsonFile/JsonFile.h"
#include"CollisionManager.h"
#include"InputBind.h"
#include"Sound.h"
#include<algorithm>
#include"TimeManager.h"

bool MemoManager::isLookItem_ = false;

namespace {
   float maxSpriteScale_ = { 2.0f};
   float minSpriteScale_ = { 1.0f};
}

MemoManager::MemoManager()
{
    sprite_ = std::make_unique<Sprite>();
    float width = static_cast<float>(Window::GetClientWidth());
    float height = static_cast<float>(Window::GetClientHeight());
    sprite_->Create(TextureFactory::MEMO1, { width * 0.5f, height * 0.5f });
    sprite_->SetAnchorPoint({ 0.5f,0.5f });

}

void MemoManager::Initialize()
{
    isLookItem_ = false;
}

void MemoManager::Update()
{


    for (auto& [handle, memo] : memos_) {
        memo->Update();
    }

    if (isLookItem_) {

        Vector2 controllerPos = { 0.0f, 0.0f };
        if (Input::IsControllerStickPosMove(BUTTON_RIGHT, 0, &controllerPos)) {
            //コントローラーの入力情報を取得する　どちらもy
            spriteScale_ = { controllerPos.y, controllerPos.y };

        } else {
            spriteScale_ += Input::GetMouseWheel()*Time::DeltaTime();
        }
        spriteScale_.x = std::clamp(spriteScale_.x, minSpriteScale_, maxSpriteScale_);
        spriteScale_.y = std::clamp(spriteScale_.y, minSpriteScale_, maxSpriteScale_);

        sprite_->SetScale(spriteScale_);
    } 
}

void MemoManager::Draw(Camera& camera)
{
    for (auto& [handle, memo] : memos_) {
        memo->Draw(camera);
    }


}

void MemoManager::DrawUI()
{
    //アイテムを見ていたら表示する
    if (isLookItem_) {
        Sprite::PreDraw();
        sprite_->Draw();
    }
}


void MemoManager::GenerateMemos(const std::vector<TextureFactory::Handle>& handles)
{
    memos_.clear(); // 既存のメモをクリア

    Json file = JsonFile::GetJsonFiles("memo");

    std::unordered_map<std::string, WorldTransform> memoTransforms;
    std::unordered_map<std::string, AABB> memoAABB;

    // サイズ読み込み
    std::vector<std::string> sizeKeys = { "memoSize", "bookSize", "noteSize" };
    for (const auto& key : sizeKeys) {
        if (file.contains(key)) {
            AABB aabb;
            aabb.min.x = file[key]["min"]["x"];
            aabb.min.y = file[key]["min"]["y"];
            aabb.min.z = file[key]["min"]["z"];
            aabb.max.x = file[key]["max"]["x"];
            aabb.max.y = file[key]["max"]["y"];
            aabb.max.z = file[key]["max"]["z"];
            memoAABB[key] = aabb;
        }
    }

    for (const auto& handle : handles) {
        auto memo = std::make_unique<Memo>();
        memo->SetTexture(handle);

        std::string key;
        switch (handle) {
        case TextureFactory::MEMO1: key = "memo1"; break;
        case TextureFactory::MEMO2: key = "memo2"; break;
        case TextureFactory::MEMO3: key = "memo3"; break;
        case TextureFactory::MEMO4: key = "memo4"; break;
        case TextureFactory::MEMO5: key = "memo5"; break;
        case TextureFactory::BOOK:  key = "book1"; break;
        case TextureFactory::BOOK2: key = "book2"; break;
        case TextureFactory::BOOK3: key = "book3"; break;
        case TextureFactory::BOOK4: key = "book4"; break;
        default: continue;
        }

        if (file.contains(key)) {
            WorldTransform wt;
            wt.translate_.x = file[key]["translate"]["x"];
            wt.translate_.y = file[key]["translate"]["y"];
            wt.translate_.z = file[key]["translate"]["z"];
            wt.rotate_.x = file[key]["rotate"]["x"];
            wt.rotate_.y = file[key]["rotate"]["y"];
            wt.rotate_.z = file[key]["rotate"]["z"];
            memo->GetWorldTransform() = wt;
        }

        if (handle == TextureFactory::BOOK || handle == TextureFactory::BOOK2) {
            memo->SetCubeSize(memoAABB["bookSize"]);
        } else if (handle == TextureFactory::BOOK3 || handle == TextureFactory::BOOK4) {
            memo->SetCubeSize(memoAABB["noteSize"]);
        } else {
            memo->SetCubeSize(memoAABB["memoSize"]);
        }


        memos_[handle] = std::move(memo);
    }
}

void MemoManager::SetSpriteSize(const TextureFactory::Handle& handle)
{
    sprite_->SetTexture(handle);
    sprite_->AdjustTextureSize();
}

void MemoManager::RayCastHit(RaySprite& raySprite)
{
    if (isLookItem_) {
        return;
    }

    for (auto& [handle, memo] : memos_) {
        AABB aabb = GetAABBWorldPos(memo.get());

        if (raySprite.IntersectsAABB(aabb, memo->GetWorldTransform().GetWorldPosition())) {

            memo->SetColor({ 1.0f,0.0f,0.0f,1.0f });

            if (InputBind::IsClick()) {
                if (!isLookItem_) {
                    isLookItem_ = true;
                    SetSpriteSize(handle);
                    spriteScale_ = { minSpriteScale_ ,minSpriteScale_ };
                    Sound::PlayOriginSE(SoundFactory::BOOK);
                    return;
                }

            }

        }
    }
}
