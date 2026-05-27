#pragma once

#include"Collider.h"
#include <memory>
#include"Object3d.h"
#include"AnimationObject3d.h"
#include"SkinningModel.h"
#include"Vector3.h"

class DummyMummy : public Collider {
public:
    DummyMummy();
    void Initialize();
    void Update();
    void Draw(Camera& camera);
    void OnCollision(Collider* collider) override;
    void SetCollisionType();
    void SetIsOpen(const bool& isOpen) {
        isOpen_ = isOpen;
    };
    const bool& GetIsOpen() {
        return isOpen_;
    };
    static void SetTargetPosPtr(Vector3* pos) { targetPos_ = pos; };
    void SetPosition(const Vector3& pos) { object_->worldTransform_.translate_ = pos; };
    void SetRotateY(const float& rotate) { object_->worldTransform_.rotate_.y = rotate; };
    const bool& GetIsHitCollision() {
        return isHitCollision_;
    };
private:
    Vector3 GetWorldPos();
    static Vector3* targetPos_;
    std::unique_ptr<AnimationObject3d> object_;
    bool isOpen_ = false;
    static Model* model_;
    void Look(const Vector3& target);
    Vector3 velocity_ = {0.0f};
    bool isHitCollision_ = false;
};
