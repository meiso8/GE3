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
    void Draw();
    void SetBulletType(const BulletType& type);
    void Shot(const Vector3& position, const Vector3& direction, const float speed, const float size, const Bullet::BulletType& type);
    void SetColor(const Vector4& color);
    bool GetIsActive() { return isActive_; };
    void SetIsActive(const bool flag) { isActive_ = flag; };
private:
    std::unique_ptr<Object3d> body_ = nullptr;
    Model* model_ = nullptr;
    //敵つめたで初期化
    BulletType type_ = kEnemyCold;
    
    bool isActive_ = false;
    
    float size_ = 1.0f;

    Vector3 moveDir_ = {0.0f};
    float moveSpeed_ = {0.0f};
    float lifeTimer_ = {0.0f};
    float lifeDuration_ = {0.0f};

};