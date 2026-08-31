#include "CollisionManager.h"
#include "Collider.h"
#include "Collision.h"
#include"Sound.h"

AABB ColliderWorldPos::GetAABBWorldPos(Collider* aabb)
{
    //中心点を考慮した座標を取得してくる
    Vector3 pos = aabb->CalculateWorldPos();
    AABB aabbWorld = aabb->GetAABB();
    aabbWorld.min += pos;
    aabbWorld.max += pos;
    return aabbWorld;
}

Sphere ColliderWorldPos::GetSphereWorldPos(Collider* sphere)
{
    //中心点を考慮した座標を取得してくる
    return Sphere{
      .center = sphere->CalculateWorldPos(),
      .radius = sphere->GetRadius()
    };
}

void CollisionManager::CheckAllCollisions() {

    for (auto& collider : colliders_) {
        //計算フラグをfalseにする
        collider->InitCalcuatedTisFrameFlag();
        collider->GetCollisionInfo().collided = false;
    }

    // リスト内のペアを総当たり
    std::list<Collider*>::iterator itrA = colliders_.begin();
    for (; itrA != colliders_.end(); ++itrA) {
        std::list<Collider*>::iterator itrB = itrA;
        for (++itrB; itrB != colliders_.end(); ++itrB) {
            // 衝突フィルタリング
            if (((*itrA)->GetCollisionAttribute() & (*itrB)->GetCollisionMask()) == 0 ||
                ((*itrB)->GetCollisionAttribute() & (*itrA)->GetCollisionMask()) == 0) {
                continue; // 衝突しない
            }

            CheckCollisionPair(*itrA, *itrB);

        }
    }
}

void CollisionManager::DrawAllCollider()
{

    if (!isDraw_) {
        return;
    }

    for (auto& collider : colliders_) {
        collider->ColliderDraw();
    }
}

void CollisionManager::UpdateAllCollider()
{
    for (auto& collider : colliders_) {
        collider->ColliderUpdate();
    }
}

void CollisionManager::CheckCollisionSpherePair(Collider* colliderA, Collider* colliderB)
{
    // 衝突判定
    if (IsCollision(ColliderWorldPos::GetSphereWorldPos(colliderA), ColliderWorldPos::GetSphereWorldPos(colliderB))) {
        
        OnCollision(colliderA, colliderB);
    }
}

void CollisionManager::CheckCollisionAABBPair(Collider* colliderA, Collider* colliderB)
{
    AABB worldPosA = ColliderWorldPos::GetAABBWorldPos(colliderA);
    AABB worldPosB = ColliderWorldPos::GetAABBWorldPos(colliderB);

    colliderA->SetCollisionInfo(GetCollisionInfo(worldPosA, worldPosB));
    colliderB->SetCollisionInfo(GetCollisionInfo(worldPosA, worldPosB));

    // 衝突判定
    if (colliderA->GetCollisionInfo().collided && colliderB->GetCollisionInfo().collided) {
        OnCollision(colliderA, colliderB);
    }
}

void CollisionManager::CheckCollisionSphereAABBPair(Collider* sphereC, Collider* aabbC)
{

    Sphere worldSphereC = ColliderWorldPos::GetSphereWorldPos(sphereC);
    AABB worldAABBC = ColliderWorldPos::GetAABBWorldPos(aabbC);

    CollisionInfo info = GetCollisionInfo(worldSphereC, worldAABBC);

    // 衝突判定
    if (info.collided) {
        // Sphere側にはそのままセット

        sphereC->SetCollisionInfo(info);
        // AABB側には法線を逆向きにしてセット
        CollisionInfo aabbInfo = info;
        aabbInfo.normal = { -info.normal.x, -info.normal.y, -info.normal.z };
        aabbC->SetCollisionInfo(aabbInfo);

        OnCollision(aabbC, sphereC);
    }

}


void CollisionManager::CheckCollisionPair(Collider* a, Collider* b) {

    auto typeA = a->GetType();
    auto typeB = b->GetType();

    if (typeA == Collider::kSphere && typeB == Collider::kSphere) {
        CheckCollisionSpherePair(a, b);
    } else if (typeA == Collider::kSphere && typeB == Collider::kAABB) {
        CheckCollisionSphereAABBPair(a, b);
    } else if (typeA == Collider::kAABB && typeB == Collider::kSphere) {
        CheckCollisionSphereAABBPair(b, a); // 順番に注意！
    } else if (typeA == Collider::kAABB && typeB == Collider::kAABB) {
        CheckCollisionAABBPair(a, b);
    }
}

void CollisionManager::OnCollision(Collider* a,Collider* b)
{
    a->OnCollision(b);
    b->OnCollision(a);
}
