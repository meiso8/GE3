#pragma once
#include<Object3d.h>

class Beam
{
public:
    void SetParent(WorldTransform* parent) { if(object3d_)object3d_->worldTransform_.Parent(*parent); }
    Beam();
    void Initialize();
    void Update();
    void Draw(Camera* camera);
private:
    std::unique_ptr<Primitive>beam_ = nullptr;
    std::unique_ptr<Object3d> object3d_ = nullptr;
};

