#pragma once

#include"Collider.h"
#include <memory>
#include"Object3d.h"

class Thoth: public Collider
{
public:
    Thoth();
    void Initialize();
    void Update();
    void Draw(Camera& camera);
    void OnCollision(Collider* collider)override;
    void SetIsActive(const bool flag) { isActive_ = flag; };
    const bool GetIsActive() { return isActive_; }
private:
    std::unique_ptr<Object3d>object_ = nullptr;
    bool isActive_ = false;
    float timer_ = 0.0f;
};

