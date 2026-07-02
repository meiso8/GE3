#pragma once
#include"Collider.h"
#include <memory>
#include"Object3d.h"

class Papyrus : public Collider {
public:
    Papyrus();
    void Initialize();
    void Update();
    void Draw(Camera& camera);
    void OnCollision(Collider* collider) override;
private:
    std::unique_ptr<Object3d> object_;
};
