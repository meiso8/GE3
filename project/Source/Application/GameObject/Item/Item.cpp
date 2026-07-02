#include"Item.h"
#include"Model.h"
#include"TransformAni/TransformAni.h"
#include"TimeManager.h"

#include"CoordinateTransform.h"

Item::Item()
{
    object_ = std::make_shared<Object3d>();
    object_->Create();
    SetAABB({ .min = { -0.5f,-0.5f,-0.5f},.max = { 0.5f,0.5f,0.5f } });
    SetCollisionAttribute(CollisionTag::GetTag("Item"));
    SetCollisionMask(!CollisionTag::GetTag("Item"));
    SetWorldMatrix(*object_);

}
void Item::SetModel(const std::string& tagName)
{
    object_->SetMeshAndMaterial(ModelManager::GetModel(tagName));
}
void Item::Init()
{
    isGetAnimEnd_ = false;
    isGet_ = false;
    isUsed_ = false;
    aniTimer_ = 0.0f;
    object_->Initialize();
    object_->SetTemperature(0.5f);
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

void Item::DrawForSlotItem(Camera& camera)
{
    object_->Draw(camera, kBlendModeNormal, kCullModeBack, kNone, true);
}

void Item::OnCollision(Collider* collider)
{
    object_->SetColor({ 1.0f,0.0f,0.0f,1.0f });
    OnCollisionCollider();
}

void Item::Rotate()
{

    TransformAni::RotateY(object_->GetWorldTransform(), 1.0f);
}

void Item::Scale(const Vector3 start, const Vector3 end)
{
    float localTime = (aniTimer_ - 2.0f) / 2.0f;
    object_->SetScale(Lerp(start, end, localTime));
}

void Item::SetScreenStartPos()
{
    object_->Initialize();

    auto& trnasform = object_->GetTransform();
    trnasform.translate.z = 1.0f;

    startPos_ = trnasform.translate;
    object_->Update();
}

void Item::UpdateAniTimer(const float& endTime)
{
    if (aniTimer_ == endTime) {
        isGetAnimEnd_ = true;
        return;
    }

    aniTimer_ += TimeManager::DeltaTime();
    aniTimer_ = std::clamp(aniTimer_, 0.0f, endTime);
}

void Item::LerpScreenPos(const Vector2& screenPos, const Matrix4x4& matInverseVPV)
{

    float localTime = (aniTimer_ - 2.0f) / 2.0f;
    // スクリーン座標 → ワールド座標に変換（Z=0.5f くらいがちょうど中間）
    Vector3 screenPoint = { screenPos.x, screenPos.y, 0.0f };
    Vector3 worldPos = CoordinateTransform(screenPoint, matInverseVPV);

    // アイテムの位置を更新！ Trigger時に格納したstartPos
    object_->SetTranslate(Lerp(startPos_, worldPos, localTime));

}

void Item::SetStartEndPos(const Vector3& start, const Vector3& end)
{
    startPos_ = start;
    endPos_ = end;
}

void Item::Update()
{
    object_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
    object_->Update();
    ColliderUpdate();
}