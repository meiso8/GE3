#pragma once
#include<memory>
#include"Object3d.h"
#include<functional>

class Camera;
class Model;

class World
{
private:
    std::unique_ptr <Primitive> sphereMesh_;
    std::unique_ptr <Object3d> object3d_;
    enum Tex {
        NORMAL,
    };
    float timer_ = 0.0f;
public:
    World();
    void Init();
    void Update();
    void Draw(Camera& camera);
    uint32_t texture_ = NORMAL;
    void SetTexture(const uint32_t& texture);
    void UpdateColor();

};

