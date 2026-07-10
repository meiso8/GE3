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
    object3d_ = std::make_unique<Object3d>();
    object3d_->Create();
    debugMesh_ = std::make_unique<Primitive>();
    debugMesh_->Create(PrimitiveGenerator::CreateSphere({ .center = {0.0f,0.0f,0.0f}, .radius = 0.5f }));
    object3d_->SetMeshAndMaterial(debugMesh_.get());
#endif // _DEBUG

    collisionInfo_.collided = false;
    collisionInfo_.normal = { 0.0f,0.0f,0.0f };
    collisionInfo_.penetration = { 0.0f };

    center_ = { 0.0f,0.0f,0.0f };
    float size = 0.5f;
    aabb_ = { {-size ,-size ,-size }, {size,size,size} };

    isCalculatedThisFrame_ = false;
}

Collider::~Collider()
{
#ifdef _DEBUG
    object3d_.reset();
    debugMesh_.reset();
#endif // _DEBUG
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
    tempWorldTransform_ = Math::GetWorldTransformByMatrix(child);

#ifdef _DEBUG
    object3d_->SetTranslate(tempWorldTransform_);
#endif // _DEBUG

    //計算終了
    isCalculatedThisFrame_ = true;

    return tempWorldTransform_;
}

void Collider::ColliderUpdate()
{
#ifdef _DEBUG
    object3d_->SetColor({ 1.0f,1.0f,0.0f,0.5f });
    object3d_->Update();
#endif // _DEBUG
}

void Collider::ColliderDraw(Camera& camera)
{
#ifdef _DEBUG
    object3d_->SetLightMode(Object3d::kLightModeNone);
    object3d_->Draw(camera);
#endif // _DEBUG
}

void Collider::OnCollisionCollider()
{
#ifdef _DEBUG
    object3d_->SetColor({ 1.0f,0.0f,0.0f,0.5f });
#endif // _DEBUG

}

CollisionInfo GetCollisionInfo(const Sphere& sphere, const AABB& AABB) {

    CollisionInfo result;
    // 最近接点をAABB内から計算（クランプ）
    Vector3 closestPoint;

    closestPoint.x = std::clamp(sphere.center.x, AABB.min.x, AABB.max.x);
    closestPoint.y = std::clamp(sphere.center.y, AABB.min.y, AABB.max.y);
    closestPoint.z = std::clamp(sphere.center.z, AABB.min.z, AABB.max.z);

    // 最近接点と球の中心の距離の2乗を計算
    Vector3 difference = sphere.center - closestPoint;
    float distanceSquared = Dot(difference, difference);

    result.collided = distanceSquared <= (sphere.radius * sphere.radius);

    // 球の半径の2乗と比較
    if (!result.collided) {
        return result;
    }

    result.collided = true;

    // 2. 球の中心がAABBの外側にある場合（表面での接触）
    if (distanceSquared > 1e-6f) {
        float distance = std::sqrt(distanceSquared);
        result.normal = difference / distance; // 正規化（AABBからSphereへ向かうベクトル）
        result.penetration = sphere.radius - distance;
        return result;
    }

    // 3. 球の中心がAABBの完全に内側にある場合（深くめり込んだ場合）
    // 各面への距離を計算
    float distX_min = sphere.center.x - AABB.min.x;
    float distX_max = AABB.max.x - sphere.center.x;
    float distY_min = sphere.center.y - AABB.min.y;
    float distY_max = AABB.max.y - sphere.center.y;
    float distZ_min = sphere.center.z - AABB.min.z;
    float distZ_max = AABB.max.z - sphere.center.z;

    // 最も近い面を探す
    float minDist = distX_min;
    Vector3 normal = Vector3(-1, 0, 0); // Xマイナス面

    if (distX_max < minDist) { minDist = distX_max; normal = Vector3(1, 0, 0); }
    if (distY_min < minDist) { minDist = distY_min; normal = Vector3(0, -1, 0); }
    if (distY_max < minDist) { minDist = distY_max; normal = Vector3(0, 1, 0); }
    if (distZ_min < minDist) { minDist = distZ_min; normal = Vector3(0, 0, -1); }
    if (distZ_max < minDist) { minDist = distZ_max; normal = Vector3(0, 0, 1); }

    result.normal = normal;
    result.penetration = sphere.radius + minDist;

    return result;
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