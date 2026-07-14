#include "BastetBlockMap.h"
#include"SoundManager/SoundManager.h"
#include"CollisionManager.h"
#include"Sound.h"

BastetBlockMap::BastetBlockMap()
{
    AABB aabb = { .min = {-0.5f, -0.5f, -1.5f}, .max = {0.5f, 0.5f, 1.5f} };

    AABB blackAABB = aabb;
    blackAABB.min.x *= 0.5f;
    blackAABB.max.x *= 0.5f;
    AABB collisionAABB = { aabb.min * 0.5f,aabb.max * 0.5f };
    AABB collisionBlackAABB = { blackAABB.min * 0.5f,blackAABB.max * 0.5f };

    for (auto& block : blocks_) {
        block = std::make_unique<Block>();
        block->SetTextureHandle(TextureFactory::NONE);
    }

    for (int i = 0; i < kMaxHz; ++i) {

        if (i <= 4 && i % 2 == 1 || i <= 10 && i >= 6 && i % 2 == 0) {
            colorMap_[static_cast<BlockHz>(i)] = COLOR::BLACK;
            blocks_[i]->SetCubeSize(blackAABB);
            blocks_[i]->SetAABB(collisionBlackAABB);
        } else {
            colorMap_[static_cast<BlockHz>(i)] = COLOR::WHITE;
            blocks_[i]->SetCubeSize(aabb);
            blocks_[i]->SetAABB(collisionAABB);
        }
    }

    for (int i = 0; i < kMaxHz; ++i) {
        blocks_[i]->SetColor(colorMap_[static_cast<BlockHz>(i)]);
    }

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

    whiteMap_[0] = blocks_[kC].get();
    whiteMap_[1] = blocks_[kD].get();
    whiteMap_[2] = blocks_[kE].get();
    whiteMap_[3] = blocks_[kF].get();
    whiteMap_[4] = blocks_[kG].get();
    whiteMap_[5] = blocks_[kA].get();
    whiteMap_[6] = blocks_[kB].get();
    whiteMap_[7] = blocks_[kC_H].get();

    blackMap_[0] = blocks_[kC_S].get();
    blackMap_[1] = blocks_[kD_S].get();
    blackMap_[2] = blocks_[kF_S].get();
    blackMap_[3] = blocks_[kG_S].get();
    blackMap_[4] = blocks_[kA_S].get();
}

void BastetBlockMap::Initialize()
{

    isClear_ = false;

    for (auto& isPush : isPushs_) {
        isPush = false;
    }



    bool isPreBlack = false;
    bool isBlack = false;


    for (auto& block : blocks_) {
        block->Initialize();
    }

    AABB collisionAABB = whiteMap_[0]->GetAABB();
    float blockSize = collisionAABB.max.x - collisionAABB.min.x;

    float offset = -blockSize * 2.0f * 4.0f + blockSize;

    for (int i = 0; i < whiteMap_.size(); ++i) {
        Vector3 pos = {
            static_cast<float>(i) * blockSize * 2.0f+ offset,
            0.0f,
            6.0f
        };

        isPreBlack = isBlack;
        whiteMap_[i]->SetPos(pos);
    }

    AABB collisionBlackAABB = blackMap_[0]->GetAABB();

    float blockSizeBlack = collisionBlackAABB.max.x - collisionBlackAABB.min.x;

    for (int i = 0; i < blackMap_.size(); ++i) {

        float posX = 0.0f;
        if (i >= 2) {
            posX = whiteMap_[i + 1]->GetPos().x;

        } else {
            posX = whiteMap_[i]->GetPos().x;
        }

        posX += blockSizeBlack * 2.0f;

        Vector3 pos = {
          posX,
           0.75f,
           7.0f
        };

        blackMap_[i]->SetPos(pos);
    }

}

void BastetBlockMap::Update()
{
    for (int i = 0; i < kMaxHz; ++i) {
        blocks_[i]->Update();
        Vector4 color = colorMap_[static_cast<BlockHz>(i)];
        blocks_[i]->SetColor(color);
    }

#ifdef USE_IMGUI

    ImGui::Begin("Debug");
    if (ImGui::Button("AnubisBlockClear")) {
        ClearSet();
    }
    ImGui::End();
#endif
    if (isClear_) {
        return;
    }

    bool isReset = false;

    for (int i = 0; i < kMaxHz; ++i)
        if (blocks_[i]->GetIsPush()) {

            if (!isPushs_[i]) {
                isPushs_[i] = true;
                //どの音からピッチを変化させてみる
                Sound::PlaySE(SoundFactory::Sound_C, 1.0f);
                Sound::SetFrequencyRatio(SoundFactory::Sound_C, i);

            }

            // すでに踏んだ順番に追加（重複防止）
            if (std::find(steppedOrder_.begin(), steppedOrder_.end(), i) == steppedOrder_.end()) {
                steppedOrder_.push_back(i);
            }

            // 正しい順番と一致したらクリア
            if (steppedOrder_ == correctOrder_) {
                if (!isClear_) {
                    ClearSet(); 
                    ResetPushMap();
                    return;
                }
            }

            // 全てふんでみて間違った順番ならリセット
            if (steppedOrder_.size() >= correctOrder_.size()) {
                isReset = true;
            }


        }

    if (isReset) {

        for (auto& isPush : isPushs_) {
            isPush = false;
        }
        steppedOrder_.clear();
        SoundManager::PlayCancelSE();
        ResetPushMap();
    }
}

void BastetBlockMap::Draw(Camera& camera)
{
    for (auto& block : blocks_) {
        block->Draw(camera);
    }
}

void BastetBlockMap::ResetPushMap()
{
    for (auto& block : blocks_) {
        if (block->GetIsPush()) {
            block->Reset(false);
            //戻す位置を設定
            block->SetEndPos();
        }
    }
}

void BastetBlockMap::RayCastHit(RaySprite& raySprite)
{
    for (int i = 0; i < kMaxHz; ++i) {

        if (!blocks_[i]->GetIsPush()) {
            AABB aabb = GetAABBWorldPos(blocks_[i].get());
            if (raySprite.IntersectsAABB(aabb, blocks_[i]->GetWorldTransform().GetWorldPosition())) {
                Vector4 color = COLOR::ToShadowColor(colorMap_[static_cast<BlockHz>(i)]);
                blocks_[i]->SetColor(color);
                //ブロックテクスチャによって判定しない
                blocks_[i]->RayCastHit(false);


            }
        }
    }
}

void BastetBlockMap::ClearSet()
{
    isClear_ = true;
    SoundManager::PlayCorrectSE();
    SoundManager::PlayGOGOGOSE();
}
