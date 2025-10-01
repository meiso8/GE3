#pragma once

#include"WorldTransform.h"
#include"ModelData.h"

class MyEngine;
class Model;
class Camera;

class Player
{
public:
    Player(MyEngine& myEngine, const ModelData& modelData);
    void Init();
    void Draw(Camera& camera);
    void Update();
private:
    const ModelData* modelData_;
    std::unique_ptr<Model>model_;
    WorldTransform worldTransform_;
};

