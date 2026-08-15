#include "RaySprite.h"
#include"Vector3.h"

#include"Input.h"
#include"Sound.h"
#include"InputBind.h"
#include"Collider.h"
#include"CollisionManager.h"
#include"Collision.h"


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

bool RaySprite::Intersect(Collider* collider,const float kMaxDistance)
{
    if (collider == nullptr) {
        return false;
    }

    auto type = collider->GetType();

    if (type == Collider::kSphere) {
        return IntersectSphere(collider, kMaxDistance);
    }

    if (type == Collider::kAABB) {
       return IntersectAABB(collider, kMaxDistance);
    }

    return false;
}

void RaySprite::Update()
{
    sprite_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
}

bool RaySprite::IntersectAABB(Collider* collider, const float kMaxDistance)
{

    if (RayIntersectsAABB(ray_, ColliderWorldPos::GetAABBWorldPos(collider), tMin_, tMax_)) {
        return CanSelect(collider,kMaxDistance);
    }

    return false;
}

bool RaySprite::IntersectSphere(Collider* collider, const float kMaxDistance)
{
    Vector3 pos1 = {0.0f};
    Vector3 pos2 = {0.0f};
    Sphere  sphere = ColliderWorldPos::GetSphereWorldPos(collider);
    if (IsCollision(ray_, sphere, pos1, pos2)) {
        return CanSelect(collider, kMaxDistance);
    };

    return false;
}

bool RaySprite::CanSelect(Collider* collider, const float kMaxDistance)
{
    float dist = Distance(ray_.origin, collider->CalculateWorldPos());
    if (dist <= kMaxDistance) {
        OnCollisionColor();
        SetSprite(collider);
        return true;
    }

    return false;
}

void RaySprite::SetSprite(Collider* collider)
{
    //アイテムのタグからテクスチャをセットする
    if (collider->GetCollisionAttribute() == CollisionTag::GetTag("Item")||
        collider->GetCollisionAttribute() == CollisionTag::GetTag("Block")||
        collider->GetCollisionAttribute() == CollisionTag::GetTag("Fire")) {
        sprite_->SetTexture(TextureFactory::HAND);
    }

    if (collider->GetCollisionAttribute() == CollisionTag::GetTag("Memo")
        || collider->GetCollisionAttribute() == CollisionTag::GetTag("Medjed")
        || collider->GetCollisionAttribute() == CollisionTag::GetTag("Enemy")
        ) {
        sprite_->SetTexture(TextureFactory::EYE);
    }

    if (collider->GetCollisionAttribute() == CollisionTag::GetTag("EnemyBulletCold")
        || collider->GetCollisionAttribute() == CollisionTag::GetTag("EnemyBulletHot")
        ) {
       /* sprite_->SetTexture(TextureFactory::EYE);*/
    }

}
