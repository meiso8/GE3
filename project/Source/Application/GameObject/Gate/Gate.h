#pragma once
#include"Collider.h"
#include"AnimationObject3d.h"

class Gate : public Collider {

public:
    Gate();
    void Initialize();
    void Update();
    void Draw();
    void OnCollision(Collider* collider) override;

    void SetCanOpen(const bool isOpen) {
        canOpen_ = isOpen;
    };
    WorldTransform& GetWorldTransform() { return aniObj_->GetWorldTransform(); }
    void SetHitSensor(const bool isHitSensor) { isHitSensor_ = isHitSensor; }
private:
    Model* model_;
    std::unique_ptr<AnimationObject3d> aniObj_ = nullptr;
    bool isOpen_ = false;
    bool canOpen_ = false;
    bool isHitSensor_ = false;
};

