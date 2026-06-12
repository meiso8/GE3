#pragma once
#include<memory>
#include"Line.h"
#include"Sprite.h"

class RaySprite
{
private:
    std::unique_ptr<Sprite>sprite_ = nullptr;
    float tMin_ = 0.0f;
    float tMax_ = 1.0f;
    void Update();
public:

    RaySprite();
    void UpdateRay(const Ray ray) {
        ray_ = ray;
        Update();
    };


    void OnCollisionColor();

    void Draw();
    bool IntersectsAABB(const AABB& aabb,const Vector3& pos,const float kMaxDistance = 5.0f);
    Ray ray_ = {};


};

