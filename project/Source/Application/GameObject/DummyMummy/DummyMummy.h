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
    void Draw();
    void OnCollision(Collider* collider) override;
    void SetCollisionType();
    void SetIsOpen(const bool& isOpen) {
        isOpen_ = isOpen;
    };
    const bool& GetIsOpen() {
        return isOpen_;
    };
    static void SetTargetPosPtr(Vector3* pos) { targetPos_ = pos; };
    void SetPosition(const Vector3& pos) { aniObj_->SetTranslate(pos); };
    void SetRotateY(const float& rotate) { aniObj_->GetTransform().rotate.y = rotate; };
    const bool& GetIsHitCollision() {
        return isHitCollision_;
    };
private:
    Vector3 GetWorldPos();
    void Look(const Vector3& target);
private:
    static Vector3* targetPos_;
    static Model* model_;

    std::unique_ptr<AnimationObject3d> aniObj_ = nullptr;
    Vector3 velocity_ = {0.0f};
    bool isHitCollision_ = false;
    bool isOpen_ = false;
};
