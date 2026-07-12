#include "MeltBlockMap.h"
#include"SoundManager/SoundManager.h"
#include"CollisionManager.h"
MeltBlockMap::MeltBlockMap()
{
    AABB aabb = { .min = {-0.5f, -0.5f, -0.5f}, .max = {0.5f, 0.5f, 0.5f} };

    for (auto& block : blocks_) {
        block = std::make_unique<Block>();
        block->SetCubeAABB(aabb);

        block->SetTextureHandle(TextureFactory::NONE);
    }
    colorMap_[0] = COLOR::CYAN;
    colorMap_[1] = COLOR::BLUE;
    colorMap_[2] = COLOR::GREEN;
    colorMap_[3] = COLOR::RED;


    temparetureMap_[kHigh] = 1.0f;
    temparetureMap_[kMiddle_High] = 0.75f;
    temparetureMap_[kMiddle_Low] = 0.25f;
    temparetureMap_[kLow] = 0.0f;



    for (int i = 0; i < kMaxBolckTempareture; ++i)
    {
        blocks_[i]->SetColor(colorMap_[i]);
        blocks_[i]->SetTemperature(temparetureMap_[static_cast<BlockTempareture>(i)]);
    }
}


void MeltBlockMap::Initialize()
{
    isClear_ = false;

    AABB aabb = blocks_[0]->GetAABB();
    float blockSize = aabb.max.x - aabb.min.x;
    float offsetX = -kMaxBolckTempareture * blockSize + blockSize;

    for (int i = 0; i < kMaxBolckTempareture; ++i) {
        blocks_[i]->Initialize();

        Vector3 pos = {
            static_cast<float>(i) * blockSize * 2.0f + offsetX,
            -0.25f,
            7.0f
        };

        blocks_[i]->SetPos(pos);
    }
}

void MeltBlockMap::Update()
{

    for (int i = 0; i < kMaxBolckTempareture; ++i) {
        blocks_[i]->Update();
        Vector4 color = colorMap_[i];
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

    for (int i = 0; i < kMaxBolckTempareture; ++i)
        if (blocks_[i]->GetIsPush()) {

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
        steppedOrder_.clear();
        SoundManager::PlayCancelSE();
        ResetPushMap();
    }

}


void MeltBlockMap::Draw(Camera& camera)
{
    for (auto& block : blocks_) {
        block->Draw(camera);
    }
}

void MeltBlockMap::ResetPushMap()
{

    for (auto& block : blocks_) {
        if (block->GetIsPush()) {
            block->Reset(false);
            //戻す位置を設定
            block->SetEndPos();
        }
    }

}

void MeltBlockMap::RayCastHit(RaySprite& raySprite)
{
    for (int i = 0; i < kMaxBolckTempareture; ++i) {

        if (!blocks_[i]->GetIsPush()) {
            AABB aabb = GetAABBWorldPos(blocks_[i].get());
            if (raySprite.IntersectsAABB(aabb, blocks_[i]->GetWorldTransform().GetWorldPosition())) {
                Vector4 color = COLOR::ToShadowColor(colorMap_[i]);
                blocks_[i]->SetColor(color);
                //ブロックテクスチャによって判定しない
                blocks_[i]->RayCastHit(false);

            }
        }
    }
}

void MeltBlockMap::ClearSet()
{
    isClear_ = true;
    SoundManager::PlayCorrectSE();
    SoundManager::PlayGOGOGOSE();

}
