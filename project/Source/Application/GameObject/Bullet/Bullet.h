#pragma once
#include"WorldTransform.h"
#include"Object3d.h"
#include <functional>
#include <unordered_map>
#include"Collider.h"

class Model;
class Camera;
enum LightMode;
class CubeMesh;

class Bullet :public Collider
{
public:
    enum BulletType {
        kEnemyCold,
        kEnemyHot,
        kPlayerCold,
        kPlayerHot,
    };

    Bullet();
    ~Bullet();
    void OnCollision(Collider* collider)override;
    Vector3 GetWorldPosition();
    void Initialize();
    void Update();
    void Draw(Camera& camera);
    void SetBulletType(const BulletType& type);
    void Shot(const Vector3& position, const Vector3& direction, const float speed, const float size, const Bullet::BulletType& type);
    void SetColor(const Vector4& color);
    bool isActive_ = false;
    Object3d body_;
    float size_;
private:
    BulletType type_;
    Model* model_ = nullptr;
    Vector3 moveDir_;
    float moveSpeed_;
    float lifeTimer_;
    float lifeDuration_;
};