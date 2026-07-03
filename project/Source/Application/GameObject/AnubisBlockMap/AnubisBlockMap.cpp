#include "AnubisBlockMap.h"
#include"SoundManager/SoundManager.h"
#include"CollisionManager.h"

AnubisBlockMap::AnubisBlockMap()
{
    AABB aabb = { .min = {-0.5f, -0.5f, -0.5f}, .max = {0.5f, 0.5f, 0.5f} };

    for (auto& block : blocks_) {
        block = std::make_unique<Block>();
        block->SetCubeAABB(aabb);
        block->SetTemperature(0.75f);
        block->SetTextureHandle(TextureFactory::NONE);
    }

    colorMap_[kRed] = COLOR::RED;
    colorMap_[kBlue] = COLOR::BLUE;
    colorMap_[kCyan] = COLOR::CYAN;
    colorMap_[kGreen] = COLOR::GREEN;

    for (int i = 0; i < kBlockColors;++i) {
        blocks_[i]->SetColor(colorMap_[static_cast<BlockColor>(i)]);
    }
}

void AnubisBlockMap::Initialize()
{
    isClear_ = false;

    AABB aabb = blocks_[0]->GetAABB();
    float blockSize = aabb.max.x - aabb.min.x;
    float offsetX = -kBlockColors * blockSize + blockSize;

    for (int i = 0; i < kBlockColors; ++i) {
        blocks_[i]->Initialize();

        Vector3 pos = {
            static_cast<float>(i) * blockSize*2.0f + offsetX,
            -0.25f,
            7.0f
        };

        blocks_[i]->SetPos(pos);
    }
}

Vector4 Color(const Vector4 color) {
    Vector4 newColor = { 0.0f };
    newColor = color - Vector4{ 0.5f,0.5f,0.5f,0.0f };
    newColor.x = std::clamp(newColor.x, 0.0f, 1.0f);
    newColor.y = std::clamp(newColor.y, 0.0f, 1.0f);
    newColor.z = std::clamp(newColor.z, 0.0f, 1.0f);
    newColor.w = std::clamp(newColor.w, 0.0f, 1.0f);
    return newColor;
}

void AnubisBlockMap::Update()
{

    for (int i = 0; i < kBlockColors; ++i) {
        blocks_[i]->Update();
        Vector4 color = colorMap_[static_cast<BlockColor>(i)];
        blocks_[i]->SetColor(color);
    }

    ImGui::Begin("Debug");
    if (ImGui::Button("AnubisBlockClear")) {
        ClearSet();
    }
    ImGui::End();

    if (isClear_) {
        return;
    }

    bool isReset = false;

    for (int i = 0; i < kBlockColors; ++i)
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

void AnubisBlockMap::Draw(Camera& camera)
{
    for (auto& block : blocks_) {
        block->Draw(camera);
    }
}

void AnubisBlockMap::ResetPushMap()
{

    for (auto& block : blocks_) {
        if (block->GetIsPush()) {
            block->Reset(false);
            //戻す位置を設定
            block->SetEndPos();
        }
    }

}

void AnubisBlockMap::RayCastHit(RaySprite& raySprite)
{
    for (int i = 0; i < kBlockColors; ++i) {

        if (!blocks_[i]->GetIsPush()) {
            AABB aabb = GetAABBWorldPos(blocks_[i].get());
            if (raySprite.IntersectsAABB(aabb, blocks_[i]->GetWorldTransform().GetWorldPosition())) {
                Vector4 color = Color(colorMap_[static_cast<BlockColor>(i)]);
                blocks_[i]->SetColor(color);
                //ブロックテクスチャによって判定しない
                blocks_[i]->RayCastHit(false);

            }
        }
    }
}

void AnubisBlockMap::ClearSet()
{
    isClear_ = true;
    SoundManager::PlayCorrectSE();
    SoundManager::PlayGOGOGOSE();

}
