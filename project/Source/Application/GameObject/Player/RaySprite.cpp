#include "RaySprite.h"
#include"Vector3.h"

#include"Input.h"
#include"Sound.h"
#include"InputBind.h"

RaySprite::RaySprite()
{
    sprite_ = std::make_unique<Sprite>();
    sprite_->Create(TextureFactory::EYE, { 0.0f,0.0f });
    sprite_->SetAnchorPoint({ 0.5f,0.5f });

    const float width = float(Window::GetClientWidth());
    const float height = float(Window::GetClientHeight());
    sprite_->SetPosition({ width * 0.5f,height * 0.5f });
}

void RaySprite::OnCollisionColor()
{
    sprite_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
}

void RaySprite::Draw()
{
    Sprite::PreDraw();
    sprite_->Draw();
}

bool RaySprite::IntersectsAABB(const AABB& aabb, const Vector3& pos,const float kMaxDistance)
{

    if (RayIntersectsAABB(ray_, aabb, tMin_, tMax_)) {

        float dist = Distance(ray_.origin, pos);
        if (dist <= kMaxDistance) {
            OnCollisionColor();
            return true;
        }

    }

    return false;
}

void RaySprite::Update()
{
    sprite_->SetColor({ 1.0f,1.0f,1.0f,0.125f });
}
