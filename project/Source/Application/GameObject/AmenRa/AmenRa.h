#pragma once

#include"Collider.h"
#include <memory>
#include"Object3d.h"

class AmenRa : public Collider {
public:
    AmenRa();
    ~AmenRa();
    void Initialize();
    void Update();
    void Draw();
    void OnCollision(Collider* collider) override;
private:
    std::unique_ptr<Object3d> object_ = nullptr;

};
