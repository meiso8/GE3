#pragma once

#include"Collider.h"
#include <memory>
#include"Object3d.h"

class AmenRa : public Collider {
public:
    AmenRa();
    void Initialize();
    void Update();
    void Draw(Camera& camera);
    void OnCollision(Collider* collider) override;
private:
    std::unique_ptr<Object3d> object_;

};
