#pragma once

#include"WorldTransform.h"
#include"ModelData.h"
#include<memory>

class MyEngine;
class Model;
class Camera;

class Player
{
public:
    Player();
    void Init();
    void Draw(Camera& camera, uint32_t lightType);
    void Update();
    Vector3& GetTranslate() { return worldTransform_.translate_; }
private:
    Vector3 velocity_ = { 0.0f,0.0f,0.0f };
    float kSpeed_ = 0.0f;
    std::unique_ptr<Model>model_ = nullptr;
    WorldTransform worldTransform_;
};

