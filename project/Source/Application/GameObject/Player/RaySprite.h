#pragma once
#include<memory>
#include"Line.h"
#include"Sprite.h"
#include<unordered_map>

class Collider;

class RaySprite
{
private:

    //スプライト
    std::unique_ptr<Sprite>sprite_ = nullptr;
    float tMin_ = 0.0f;
    float tMax_ = 1.0f;
private:
    void Update();
    bool IntersectAABB(Collider* collider, const float kMaxDistance);
    bool IntersectSphere(Collider* collider, const float kMaxDistance);
    bool CanSelect(Collider* collider, const float kMaxDistance);
    void SetSprite(Collider* collider);
public:

    RaySprite();
    void UpdateRay(const Ray ray) {
        ray_ = ray;
        Update();
    };


    void OnCollisionColor();

    void Draw();
    bool Intersect(Collider* collider, const float kMaxDistance = 5.0f);
    Ray ray_ = {};


};

