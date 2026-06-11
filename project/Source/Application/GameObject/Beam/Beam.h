#pragma once
#include<Object3d.h>
#include"../../../Engine/Object3d/BeamObject3d.h"

class Beam
{
public:
    void SetParent(Matrix4x4* parent) { parent_ = parent; }
    Beam();
    void Initialize();
    void Update();
    void Draw(Camera* camera);
    void SetPos(const Vector3 startPos, const Vector3 endPos) {
        object3d_->SetPos(startPos, endPos);
    }
private:
    Matrix4x4* parent_ = nullptr;
    std::unique_ptr<Primitive>beam_ = nullptr;
    std::unique_ptr<BeamObject3d> object3d_ = nullptr;
};

