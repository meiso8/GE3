#pragma once
#include<Object3d.h>
#include"../../../Engine/Object3d/BeamObject3d.h"
#include"Line.h"
#include"Mesh/LineMesh/LineMesh.h"

class Beam
{
public:

    enum BeamType {
        kEnemy,
        kPlayer,
    };

    BeamType& GetBeamType() { return  type_; };

    Beam();
    void Initialize();
    void Update();
    void Draw(Camera* camera);
    bool Shot(const Vector3& target, const  BeamType& type, const Vector3& startPos, Matrix4x4* parent);
    bool GetIsActive() { return isActive_; };

    Ray& GetRay() { return ray_; }

private:
    void UpdateObject();
private:

    struct Point
    {
        Vector3 startPos;
        Vector3 endPos;
    };
    Point point_ = { 0.0f };
    BeamType type_;
    Ray ray_;
    Matrix4x4* parent_ = nullptr;
    std::unique_ptr<Primitive>beam_ = nullptr;
    std::unique_ptr<BeamObject3d> object3d_ = nullptr;

#ifdef _DEBUG
    std::unique_ptr<LineMesh>line_ = nullptr;
    std::unique_ptr<Object3d> lineObj_ = nullptr;

#endif



    Vector3 endPos_ = { 0.0f };
    Vector3 startPos_ = { 0.0f };

    bool isActive_ = false;
    float lifeTimer_ = 0.0f;
};

