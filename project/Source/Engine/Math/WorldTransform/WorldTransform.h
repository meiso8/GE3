#pragma once
#include"Vector2.h"
#include"Vector3.h"
#include"Matrix4x4.h"
#include"Matrix3x3.h"
#include<numbers>
#include"Transform.h"

class WorldTransform {
public://一旦public

    EulerTransform eTransform_ = {
        .scale = {1.0f,1.0f,1.0f},
        .rotate = {0.0f,0.0f,0.0f},
        .translate = {0.0f,0.0f,0.0f} 
    };
    Matrix4x4 matWorld_ = {0.0f};
    const WorldTransform* parent_ = nullptr;
public:
    void Initialize();
    Vector3 GetWorldPosition()const;
    void Parent(const WorldTransform& parentWorldTransform);
};

class WorldTransform2D {
public://一旦public
    Vector2 scale_ = { 0.0f };
    float rotate_ = { 0.0f };
    Vector2 translate_ = { 0.0f };
    Matrix3x3 matWorld_ = { 0.0f };
    const WorldTransform2D* parent_ = nullptr;
public:
    void Initialize();
    Vector2 GetWorldPosition()const;
};

void WorldTransformUpdate(WorldTransform& worldTransform);
void WorldTransformUpdate(WorldTransform2D& worldTransform2D);
