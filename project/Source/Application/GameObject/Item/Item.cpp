#include"Item.h"
#include"Model.h"
#include"TransformAni/TransformAni.h"
#include"TimeManager.h"
#include"MakeMatrix.h"

#include"CoordinateTransform.h"

Item::Item()
{
    object_ = std::make_shared<Object3d>();
    object_->Create();

    handItemObj_ = std::make_unique<Object3d>();
    handItemObj_->Create();
   
    SetAABB({ .min = { -0.5f,-0.5f,-0.5f},.max = { 0.5f,0.5f,0.5f } });
    SetCollisionAttribute(CollisionTag::GetTag("Item"));
    SetCollisionMask(!CollisionTag::GetTag("Item"));
    SetWorldMatrix(*object_);

}
void Item::SetModel(const std::string& tagName)
{
    object_->SetMeshAndMaterial(ModelManager::GetModel(tagName));
    handItemObj_->SetMeshAndMaterial(ModelManager::GetModel(tagName));
}
void Item::Init()
{
    aniTimer_ = 0.0f;
    isAnimEnd_ = false;
    isGet_ = false;
    isUsed_ = false;
    isMelt_ = false;

    object_->Initialize();
    object_->SetTemperature(0.5f);

    handItemObj_->Initialize();
    handItemObj_->SetTemperature(0.5f);

    startPos_ = { 0.0f };
    endPos_ = { 0.0f };
}

void Item::Draw(Camera& camera)
{
    if (isGet_&& !isUsed_) {
    
        if (parent_) {
            handItemObj_->Draw(camera);
        }

    } else {
        object_->Draw(camera);
    }   
}

void Item::DrawForSlotItem(Camera& camera)
{
    object_->Draw(camera);
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
        isAnimEnd_ = true;
        return;
    }

    aniTimer_ += TimeManager::DeltaTime();
    aniTimer_ = std::clamp(aniTimer_, 0.0f, endTime);
}

void Item::LerpScreenPosAndScale(const Vector2& screenPos, const Matrix4x4& matInverseVPV)
{

    float localTime = (aniTimer_ - 2.0f) / 2.0f;
    // スクリーン座標 → ワールド座標に変換（Z=0.5f くらいがちょうど中間）
    Vector3 screenPoint = { screenPos.x, screenPos.y, 0.0f };
    Vector3 worldPos = CoordinateTransform(screenPoint, matInverseVPV);

    // アイテムの位置を更新！ Trigger時に格納したstartPos
    object_->SetTranslate(Lerp(startPos_, worldPos, localTime));

    //スケールのラープ
    object_->SetScale(Lerp({ 1.0f,1.0f,1.0f }, Vector3{ screenEndSize_,screenEndSize_,screenEndSize_ }, localTime));
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

    if (parent_&&isGet_) {
        handItemObj_->SetWorldMatrix(*parent_);
    }
   
}