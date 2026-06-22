#include "CrowbarItem.h"
#include "SunRod.h"
#include"CoordinateTransform.h"
#include"Easing.h"

std::shared_ptr<Item>  CrowbarItem::CombineWith(const std::shared_ptr<Item>& other)  {
    if (other->name_ == "SunMedal") {
        //メダルと合体するとSunRod
        return std::make_shared<SunRod>();
    }
    return nullptr;
}

void CrowbarItem::LerpScreenPos(const Vector2& screenPos, const Matrix4x4& matInverseVPV)
{
    float localTime = (aniTimer_ - 2.0f) / 2.0f;
    // スクリーン座標 → ワールド座標に変換（Z=0.5f くらいがちょうど中間）
    Vector3 screenPoint = { screenPos.x, screenPos.y, -0.92f };
    Vector3 worldPos = CoordinateTransform(screenPoint, matInverseVPV);
    // アイテムの位置を更新！ Trigger時に格納したstartPos
    object_->SetScale(Lerp(Vector3{ 1.0f,1.0f,1.0f }, { 0.75f,0.75f,0.5f }, localTime));
    object_->SetTranslate(Lerp(startPos_, worldPos, localTime));

}
