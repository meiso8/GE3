#include "BastetBlockMap.h"
#include"SoundManager/SoundManager.h"
#include"CollisionManager.h"
#include"Sound.h"

BastetBlockMap::BastetBlockMap()
{
    AABB aabb = { .min = {-0.5f, -0.5f, -1.5f}, .max = {0.5f, 0.5f, 1.5f} };
    //AABB collisionAABB = { .min = {-0.25f,-0.25f,-0.75f},.max = {0.25f,0.25f,0.75f} };
    for (auto& block : blocks_) {
        block = std::make_unique<Block>();
        block->SetCubeSize(aabb);
        block->SetAABB(aabb);
        block->SetTextureHandle(TextureFactory::NONE);
    }

    for (int i = 0; i < kMaxHz; ++i) {

        if (i <= 4 && i % 2 == 1 || i <= 10 && i >= 6 && i % 2 == 0) {
            colorMap_[static_cast<BlockHz>(i)] = COLOR::BLACK;
        } else {
            colorMap_[static_cast<BlockHz>(i)] = COLOR::WHITE;
        }
    }

    for (int i = 0; i < kMaxHz; ++i) {
        blocks_[i]->SetColor(colorMap_[static_cast<BlockHz>(i)]);
    }
}

void BastetBlockMap::Initialize()
{

    isClear_ = false;

    for (auto& isPush : isPushs_) {
        isPush = false;
    }


    AABB collisionAABB = blocks_[0]->GetAABB();
    float blockSize = collisionAABB.max.x - collisionAABB.min.x;
    float offsetX = -kMaxHz * blockSize + blockSize;

    for (int i = 0; i < kMaxHz; ++i) {

        blocks_[i]->Initialize();

        float posY = -0.25f;
        float posZ = 6.0f;
        if (colorMap_[static_cast<BlockHz>(i)] == COLOR::BLACK) {
            posY = 0.0f;
            posZ = 7.0f;
        }

        Vector3 pos = {
            static_cast<float>(i) * blockSize * 2.0f + offsetX,
            posY,
            posZ
        };

        blocks_[i]->SetPos(pos);
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
