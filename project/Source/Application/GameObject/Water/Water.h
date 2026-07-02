#pragma once

#include"Collider.h"
#include <memory>
#include"Object3d.h"

class Water : public Collider {
public:
    Water();
    void Initialize();
    void Update();
    void Draw(Camera& camera);
    void OnCollision(Collider* collider) override;
    const bool& IsDrain() { return isDrain_; };
    void  SetIsDrain(const bool& isDrain) { isDrain_ = isDrain; };

private:
    bool isPrePlayerHit_ = false;
    bool isPlayerHit_ = false;
    std::unique_ptr<Object3d> object_;
    bool isDrain_ = false;
};
