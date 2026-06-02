#define NOMINMAX
#include "Collider.h"
#include"Collision.h"
#include"Matrix/MakeMatrix.h"

void Collider::InitCalcuatedTisFrameFlag()
{
    isCalculatedThisFrame_ = false;
}

Collider::Collider()
{
#ifdef _DEBUG
    object3d_.Create();
    debugMesh_ = std::make_unique<Primitive>();
    debugMesh_->Create(PrimitiveGenerator::CreateSphere({ .center = {0.0f,0.0f,0.0f}, .radius = 0.5f }));
    object3d_.SetMeshAndMaterial(debugMesh_.get());
#endif // _DEBUG

    collisionInfo_.collided = false;
    collisionInfo_.normal = { 0.0f,0.0f,0.0f };
    collisionInfo_.penetration = { 0.0f };

    center_ = { 0.0f,0.0f,0.0f };
    float size = 0.5f;
    aabb_ = { {-size ,-size ,-size }, {size,size,size} };

    isCalculatedThisFrame_ = false;
}

const Vector3& Collider::CalculateWorldPos()
{
    if (isCalculatedThisFrame_) {
        return tempWorldTransform_;
    }
    //中心点のローカル行列を計算する
    Matrix4x4 child = MakeTranslateMatrix(center_);
    assert(worldMat_);
    //親の行列と掛け算する
    child = Multiply(child, *worldMat_);
    //行列から位置を取得する
    tempWorldTransform_ = GetWorldTransformByMatrix(child);

#ifdef _DEBUG
    object3d_.worldTransform_.translate_ = tempWorldTransform_;
#endif // _DEBUG

    //計算終了
    isCalculatedThisFrame_ = true;

    return tempWorldTransform_;
}

void Collider::ColliderUpdate()
{
#ifdef _DEBUG
    object3d_.SetColor({ 1.0f,1.0f,0.0f,0.5f });
    object3d_.Update();
#endif // _DEBUG
}

void Collider::ColliderDraw(Camera& camera)
{
#ifdef _DEBUG
    object3d_.SetLightMode(kLightModeNone);
    object3d_.Draw(camera);
#endif // _DEBUG
}

void Collider::OnCollisionCollider()
{
#ifdef _DEBUG
    object3d_.SetColor({ 1.0f,0.0f,0.0f,0.5f });
#endif // _DEBUG

}

CollisionInfo GetCollisionInfo(const AABB& a, const AABB& b) {

    CollisionInfo result;

    if (!IsCollision(a, b)) {
        result.collided = false;
        return result;
    }

    result.collided = true;
    //オーバーラップを調べる
    float overlapX = std::min(a.max.x - b.min.x, b.max.x - a.min.x);
    float overlapY = std::min(a.max.y - b.min.y, b.max.y - a.min.y);
    float overlapZ = std::min(a.max.z - b.min.z, b.max.z - a.min.z);

    Vector3 centerA = Math::AABBCenter(a);
    Vector3 centerB = Math::AABBCenter(b);

    //最小のオーバーラップ軸を分離する
    if (overlapX <= overlapY && overlapX <= overlapZ) {

        result.penetration = overlapX;
        result.normal = (centerA.x < centerB.x) ? Vector3(-1.0f, 0.0f, 0.0f) : Vector3(1.0f, 0.0f, 0.0f);

    } else if (overlapY <= overlapZ) {
        result.penetration = overlapY;
        result.normal = (centerA.y < centerB.y) ? Vector3(0.0f, -1.0f, 0.0f) : Vector3(0.0f, 1.0f, 0.0f);
    } else {
        result.penetration = overlapZ;
        result.normal = (centerA.z < centerB.z) ? Vector3(0.0f, 0.0f, -1.0f) : Vector3(0.0f, 0.0f, 1.0f);
    }

    return result;


}

void ResolveCollision(Vector3& pos, Vector3& velocity, const CollisionInfo& info) {

    if (!info.collided) return;

    pos += info.normal * info.penetration;

    float normalVelocity = Dot(velocity, info.normal);

    if (normalVelocity < 0.0f) {
        velocity -= info.normal * normalVelocity;
    }
}