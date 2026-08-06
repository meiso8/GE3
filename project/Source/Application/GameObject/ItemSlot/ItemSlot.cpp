#include "ItemSlot.h"
#include"SoundManager/SoundManager.h"
#include"Input.h"
using namespace std;
#define rep(i,n)for(int i =0;i < n;++i)
#include"MakeMatrix.h"
#include<algorithm>

ItemSlot::ItemSlot()
{
    width = static_cast<float>(Window::GetClientWidth());
    height = static_cast<float>(Window::GetClientHeight());

    //カメラについての
    itemCamera_ = std::make_unique<Camera>();
    itemCamera_->Initialize();
   

    itemCamera_->SetTransform(
        { .scale = { 1.0f,1.0f,1.0f},
        .rotate = {Math::ZERO},
        .translate = { 0.0f,0.0f,0.0f } }
    );

    matViewport = MakeViewportMatrix(0, 0, width, height, 0, 1);
    matInverseVPV = Inverse(itemCamera_->GetViewProjectionMatrix() * matViewport);
    const float sizeX = 96.0f;

    rep(i, kMaxSlots_) {
        slotSprites_[i] = make_unique<Sprite>();
        slotSprites_[i]->Create(TextureFactory::SLOT, { 0.0f,0.0f });
        slotSprites_[i]->SetSize({ sizeX,sizeX });
        slotSprites_[i]->SetAnchorPoint({ 0.5f,0.5f });
        slotSprites_[i]->SetPosition({ (width - sizeX * kMaxSlots_ + sizeX) * 0.5f + i * sizeX,height - 64.0f });
    }
}

ItemSlot::~ItemSlot()
{
   
}


void ItemSlot::Init()
{
    for (auto& slot : slots_) {
        if (slot) {
            slot.reset();
        }   
    }
}

void ItemSlot::OnTriggerItemPickup(const std::shared_ptr<Item>& item)
{
    if (AddItem(item)) {
        SoundManager::PlayCorrectSE();
    } else {
        SoundManager::PlayCancelSE();
    }
}

void ItemSlot::Update()
{
    ToScreen();

    Vector2 pos = Input::GetCursorPosition();

    for (auto& sprite : slotSprites_) {
        if (IsCollision(pos, *sprite)) {
            sprite->SetColor({ 1.0f,0.0f,0.0f,1.0f });
        } else {
            sprite->SetColor({ 1.0f,1.0f,1.0f,1.0f });
        }

    }



}

void ItemSlot::ToScreen()
{
    itemCamera_->UpdateMatrix(); // ← カメラ行列を更新！
    matViewport = MakeViewportMatrix(0, 0, width, height, 0,1);  

    matInverseVPV = Inverse(itemCamera_->GetViewProjectionMatrix() * matViewport);

    for (int i = 0; i < slotSprites_.size(); ++i) {
        if (!slots_[i]) continue;
        // スプライトのスクリーン座標を取得（2D）
        Vector2 screenPos = slotSprites_[i]->GetPosition();
        GetAnimation(slots_[i], screenPos);

    }

}

bool ItemSlot::AddItem(const std::shared_ptr<Item>& item)
{
    //すでに使われていたら入れない
    if (item->IsUsed() || item->IsGet()) {
        return false;
    }

    for (auto& slot : slots_) {
        if (!slot) {
            slot = item;
            slot->Init();
            slot->SetScreenStartPos();
            slot->SetIsGet(true);
            return true;
        }
    }
    return false; // 空きスロットがなかった
}

void ItemSlot::UseItem(int index)
{
    slots_[index]->Use();
    slots_[index].reset();
}

void ItemSlot::CombineItems(int indexA, int indexB)
{
    if (!slots_[indexA] || !slots_[indexB]) return;
    auto combined = slots_[indexA]->CombineWith(slots_[indexB]);

    if (!combined) { combined = slots_[indexB]->CombineWith(slots_[indexA]); }

    if (combined) {
        slots_[indexA] = combined;
        slots_[indexB].reset();
        SoundManager::PlayCorrectSE();
    } else {
        SoundManager::PlayCancelSE();
    }
}

void ItemSlot::DrawUI()
{
    for (auto& sprite : slotSprites_) {
        if (sprite) {
            sprite->Draw();
        }
    }

}

void ItemSlot::Draw()
{
    for (auto& item : slots_) {
        if (item && !item->IsUsed()) {
            item->DrawForSlotItem(*itemCamera_);
        }
    }
}

void ItemSlot::GetAnimation(const std::shared_ptr<Item>& item, const Vector2& screenPos)
{
    //すでに使われていたら入れない
    if (item->IsGet() && item->GetAnimTimer() >= 5.0f || item->IsUsed()) {
        return;
    }

    item->UpdateAniTimer();

    if (item->GetAnimTimer() <= 2.1f) {
        item->Rotate();
    }
    if (item->GetAnimTimer() > 2.0f) {
        item->LerpScreenPos(screenPos, matInverseVPV);
        const float size = 0.03125f;
        item->Scale({ 1.0f,1.0f,1.0f }, { size,size,size });
    }

}

