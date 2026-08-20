#pragma once
#include"Collider.h"
#include"WorldTransform.h"

class GateSensor : public Collider {
public:
    GateSensor(); 
    void SetParent(WorldTransform& parent) { transform_.Parent(parent); }
    void Initialize();
   void Update();
    void OnCollision(Collider* collider) override;
   bool GetHitPlayer() { return isHitPlayer_; };
private:
    bool isHitPlayer_ = false;
    WorldTransform transform_{};
};

