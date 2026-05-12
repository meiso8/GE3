#include "Block.h"
#include"Easing.h"
#include"TimeManager.h"
#include<algorithm>
#include"SoundManager/SoundManager.h"
#include"Sound.h"
#include"CollisionConfig.h"
#include"InputBind.h"
#include"../System/CollisionManager.h"
#include"../Engine/ImGui/DebugUI.h"
Block::Block()
{
    SetCollisionAttribute(kCollisionFloor);
    SetCollisionMask(kCollisionPlayer | kCollisionEnemy | kCollisionMedjed | kCollisionDummyMedjed);  
    SetAABB({ {-1.0f,-1.0f,-1.0f} ,{1.0f,1.0f,1.0f} });
}

void Block::Initialize()
{
    object_->Initialize();
    aniTimer_ = 0.0f;
    isPush_ = false;
    SetWorldMatrix(object_->worldTransform_.matWorld_);

}

void Block::Update()
{
    aniTimer_ += kInverseFPS;
    aniTimer_ = std::clamp(aniTimer_, 0.0f, 1.0f);

    if (isPush_) {
        object_->worldTransform_.translate_.y = Easing::EaseInOut(startPosY_, endPosY_, aniTimer_);
    } else {
        object_->worldTransform_.translate_.y = Easing::EaseInOut(endPosY_, startPosY_, aniTimer_);
    }

    object_->Update();
    ColliderUpdate();
}

void Block::OnCollision(Collider* collider)
{
    OnCollisionCollider();



}

void Block::SetPos(const Vector3& pos, const float& endOffset)
{
    object_->worldTransform_.translate_ = pos;
    startPosY_ = pos.y;
    SetEndPos(endOffset);
}

void Block::SetEndPos(const float& endOffset)
{
    endPosY_ = startPosY_ + endOffset;
}

void Block::InitAnitimer()
{
    aniTimer_ = 0.0f;
}

void Block::Reset()
{
    if (!CanPushBlock()) {
        return;
    }

    aniTimer_ = 0.0f;
    isPush_ = false;
}

void Block::RayCastHit()
{
    if (!InputBind::IsClick()) {
        return;
    }

    if (isPush_) {
        return;
    }

    if (!CanPushBlock()) {
        return;
    }

    Sound::PlaySE(SoundFactory::MOVE_ROCK, 1.0f);
    aniTimer_ = 0.0f;
    isPush_ = true;
}

BlockMap::BlockMap()
{

    AABB aabb = { .min = {-1.5f, -1.5f, -1.5f}, .max = {1.5f, 1.5f, 1.5f} };

    for (int y = 0; y < kMapHeight; ++y) {
        for (int x = 0; x < kMapWidth; ++x) {
            map_[y][x] = std::make_unique<Block>();
            map_[y][x]->SetCubeAABB(aabb);
            if (rand() % 2 == 0) {
                map_[y][x]->SetTextureHandle(TextureFactory::PUZZLE);
            } else {
                map_[y][x]->SetTextureHandle(TextureFactory::NONE);
            }

        }
    }

    // ヒエログリフ付きブロックを配置（例：中央付近）
    map_[4][2]->SetTextureHandle(TextureFactory::HIERO_S);
    map_[1][5]->SetTextureHandle(TextureFactory::HIERO_T);
    map_[2][1]->SetTextureHandle(TextureFactory::HIERO_D);
    map_[3][3]->SetTextureHandle(TextureFactory::HIERO_P);

    Vector4 color = { 1.0f,1.0f,0.0f,1.0f };
    map_[4][2]->SetColor(color);
    map_[1][5]->SetColor(color);
    map_[2][1]->SetColor(color);
    map_[3][3]->SetColor(color);

    centerBlocks_[0] = map_[3][3].get(); // 左上
    centerBlocks_[1] = map_[4][3].get(); // 右上
    centerBlocks_[2] = map_[3][4].get(); // 左下
    centerBlocks_[3] = map_[4][4].get(); // 右下

    // roundBlocks_ の設定（center を除いた周囲） 
    int index = 0; for (int y = 2; y <= 5; ++y) {
        for (int x = 2; x <= 5; ++x) {
            // center の範囲（3,3）〜（4,4）を除外 
            if (x >= 3 && x <= 4 && y >= 3 && y <= 4) continue;
            roundBlocks_[index++] = map_[x][y].get();
        }
    }
}

void BlockMap::Initialize() {

    isClear_ = false;
    steppedOrder_.clear();

    AABB aabb = { .min = {-1.5f, -1.5f, -1.5f}, .max = {1.5f, 1.5f, 1.5f} };
    float blockSize = aabb.max.x - aabb.min.x; // = 3.0f
    float offsetX = -(kMapWidth * blockSize) * 0.5f + aabb.max.x;
    float offsetZ = -(kMapHeight * blockSize) * 0.5f + aabb.max.y;

    for (int y = 0; y < kMapHeight; ++y) {
        for (int x = 0; x < kMapWidth; ++x) {
            map_[y][x]->Initialize();

            Vector3 pos = {
                static_cast<float>(x) * blockSize + offsetX,
                -1.5f,
                static_cast<float>(y) * blockSize + offsetZ
            };

            map_[y][x]->SetPos(pos);
        }
    }

}

void BlockMap::Update() {

    for (auto& y : map_) {
        for (auto& block : y) {
            block->Update();
            if (block->CanPushBlock()) {
                block->SetColor({ 1.0f,1.0f,0.0f,1.0f });
            }
        }
    }

#ifdef _DEVELOP

    if (ImGui::Button("isBlockClear")) {
        ClearSet();
    }
#endif //_DEVELOP
    if (isClear_) {
        return;
    }


    // ======================================================================================
    bool isReset = false;

    for (auto& y : map_) {
        for (auto& block : y) {

            if (block->GetIsPush()) {
                TextureFactory::Handle tex = Texture::GetTextureHandle(block->GetSrvIndex());

                // すでに踏んだ順番に追加（重複防止）
                if (std::find(steppedOrder_.begin(), steppedOrder_.end(), tex) == steppedOrder_.end()) {
                    steppedOrder_.push_back(tex);
                }

                // 正しい順番と一致したら水を引く！
                if (steppedOrder_ == correctOrder_) {
                    if (!isClear_) {
                        ClearSet();
                        return;
                    }
                }

                // 間違った順番ならリセット
                if (steppedOrder_.size() >= correctOrder_.size() ||
                    steppedOrder_[steppedOrder_.size() - 1] != correctOrder_[steppedOrder_.size() - 1]) {
      
                    isReset = true;
                }


            }

        }
    }


    if (isReset) {
        steppedOrder_.clear();
        SoundManager::PlayCancelSE();
        ResetPushMap();
    }

}


void BlockMap::Draw(Camera& camera)
{
    for (auto& y : map_) {
        for (auto& x : y) {
            x->Draw(camera);
        }
    }
}

void BlockMap::ResetPushMap()
{
  
    for (auto& y : map_) {
        for (auto& block : y) {
            if (block->GetIsPush()) {
                block->Reset();
                block->SetEndPos(); // ← 戻す位置を設定
            }
    
        }
    }

}

void BlockMap::RayCastHit(RaySprite& raySprite)
{
    for (auto& y : map_) {
        for (auto& block : y) {
            if (!block->GetIsPush()&&block->CanPushBlock()) {
                AABB aabb = GetAABBWorldPos(block.get());
                if (raySprite.IntersectsAABB(aabb, block->GetWorldTransform().GetWorldPosition())) {

                    block->SetColor({ 1.0f,0.0f,0.0f,1.0f });
                    block->RayCastHit();

                }
            }

        }
    }


}

void BlockMap::ClearSet()
{
    isClear_ = true;
    SoundManager::PlayCorrectSE();
    SoundManager::PlayGOGOGOSE();
    for (auto& center : centerBlocks_) {
        if (center) {
            center->SetEndPos(-1.5f);
            center->SetIsPush(true);
            center->InitAnitimer();
        }
    }

    for (auto& round : roundBlocks_) {
        if (round) {
            round->SetEndPos(-1.0f);
            round->SetIsPush(true);
            round->InitAnitimer();
        }
    }
}
