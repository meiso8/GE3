#include "ItemSlot.h"
#include"DebugUI.h"
#include"Sound.h"
#include"SoundManager/SoundManager.h"
#include"SpriteCamera.h"
#include"Model.h"
#include"Input.h"
using namespace std;
#define rep(i,n)for(int i =0;i < n;++i)
#include"CollisionConfig.h"
#include"TimeManager.h"
#include"TransformAni/TransformAni.h"
#include"CoordinateTransform.h"
#include"MakeMatrix.h"
#include<algorithm>
#include"Easing.h"
Item::Item()
{
    object_ = std::make_shared<Object3d>();
    object_->Create();
    SetAABB({ .min = { -0.5f,-0.5f,-0.5f},.max = { 0.5f,0.5f,0.5f } });
    SetCollisionAttribute(kCollisionItem);
    SetCollisionMask(!kCollisionItem);
    SetWorldMatrix(object_->worldTransform_.matWorld_);
  
}
void Item::SetModel(const std::string& fileName)
{

 
    object_->SetMeshAndMaterial(ModelManager::GetModel(fileName));
}
void Item::Init()
{
    isGetAnimEnd_ = false;
    isGet_ = false;
    isUsed_ = false;
    aniTimer_ = 0.0f;
    object_->Initialize();
    startPos_ = { 0.0f };
    endPos_ = { 0.0f };
}
void Item::DrawInfoUI()
{
//#ifdef USE_IMGUI
//
//    ImGui::Begin("Item");
//    //ImGui::Text(description_.c_str());
//    DebugUI::CheckObject3d(*object_, name_.c_str());
//
//    ImGui::End();
//
//#endif
}

void Item::Draw(Camera& camera)
{
    object_->Draw(camera);
    //ColliderDraw(camera);
}

void Item::OnCollision(Collider* collider)
{
    object_->SetColor({ 1.0f,0.0f,0.0f,1.0f });
    OnCollisionCollider();
}

void Item::Rotate()
{

    TransformAni::RotateY(object_->worldTransform_, 1.0f);
}

void Item::SetScreenStartPos()
{
    object_->Initialize();
    object_->worldTransform_.translate_.z = 1.0f;

    startPos_ = object_->worldTransform_.translate_;
    object_->Update();
}

void Item::UpdateAniTimer(const float& endTime)
{
    if (aniTimer_ == endTime) {
        isGetAnimEnd_ = true;
        return;
    }

    aniTimer_ += Time::DeltaTime();
    aniTimer_ = std::clamp(aniTimer_, 0.0f, endTime);
}

void Item::LerpScreenPos(const Vector2& screenPos, const Matrix4x4& matInverseVPV)
{

    float localTime = (aniTimer_ - 2.0f) / 2.0f;
    // スクリーン座標 → ワールド座標に変換（Z=0.5f くらいがちょうど中間）
    Vector3 screenPoint = { screenPos.x, screenPos.y, -0.92f };
    Vector3 worldPos = CoordinateTransform(screenPoint, matInverseVPV);
    // アイテムの位置を更新！ Trigger時に格納したstartPos
    object_->worldTransform_.translate_ = Lerp(startPos_, worldPos, localTime);

}

void Item::SetStartEndPos(const Vector3& start, const Vector3& end)
{
    startPos_ = start;
    endPos_ = end;
}

void Item::Update()
{
    object_->Update();
    object_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
    ColliderUpdate();
}

ItemSlot::ItemSlot()
{
    width = static_cast<float>(Window::GetClientWidth());
    height = static_cast<float>(Window::GetClientHeight());

    //カメラについての
    itemCamera_ = std::make_unique<Camera>();
    itemCamera_->Initialize();
    itemCamera_->nearZ_ = 0.2f;
    itemCamera_->farZ_ = 1000.0f;
    float scales = 0.005f;
    itemCamera_->scale_ = { scales,scales,scales };
    itemCamera_->translate_.z = -10.0f;
    itemCamera_->UpdateMatrix();

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

void ItemSlot::Init()
{

    for (auto& slot : slots_) {
        slot.reset();
        slot = nullptr;
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

#ifdef USE_IMGUI
    DebugUI::CheckCamera(*itemCamera_);
#endif

}

void ItemSlot::ToScreen()
{
    itemCamera_->UpdateMatrix(); // ← カメラ行列を更新！
    matViewport = MakeViewportMatrix(0, 0, width, height, 0, 1);
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
    if (item->isUsed_ || item->isGet_) {
        return false;
    }

    for (auto& slot : slots_) {
        if (!slot) {
            slot = item;
            slot->Init();
            slot->SetScreenStartPos();
            slot->isGet_ = true;
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
        //ShowMessage("アイテムを組み合わせて「" + combined->name_ + "」を作成した！");
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
        if (item && !item->isUsed_) {
            item->Draw(*itemCamera_);
        }
    }
}

void ItemSlot::GetAnimation(const std::shared_ptr<Item>& item, const Vector2& screenPos)
{
    //すでに使われていたら入れない
    if (item->isGet_ && item->aniTimer_ >= 5.0f || item->isUsed_) {
        return;
    }

    item->UpdateAniTimer();

    if (item->aniTimer_ <= 2.1f) {
        item->Rotate();
    }
    if (item->aniTimer_ > 2.0f) {
        item->LerpScreenPos(screenPos, matInverseVPV);

    }

}
