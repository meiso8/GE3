#pragma once
#include<memory>
#include"Object3d.h"
#include<functional>

#include "Circle.h"

class Camera;

class Field
{
private:
    std::unique_ptr <Object3d> object3d_;
    std::unique_ptr<Primitive> circleMesh_ = nullptr;
    Circle& GetCircle() {
        return circle_;
    };

public:
    Field();
    void Init();
    void Update();
    void Draw(Camera& camera);
    Circle circle_;
    float uvTranslate_ = 0.0f;
};
