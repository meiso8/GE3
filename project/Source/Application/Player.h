#pragma once

#include"WorldTransform.h"
#include"ModelData.h"

class MyEngine;
class Model;
class Camera;

class Player
{
public:
    Player(const ModelData& modelData);
    void Init();
    void Draw(Camera& camera, uint32_t lightType);
    void Update();
    Vector3& GetTranslate() { return worldTransform_.translate_; }
private:
    Vector3 velocity_;
    float kSpeed_;
    const ModelData* modelData_;
    std::unique_ptr<Model>model_;
    WorldTransform worldTransform_;
};

