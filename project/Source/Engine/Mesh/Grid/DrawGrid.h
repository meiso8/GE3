#pragma once

#include<array>
#include"Object3d.h"
#include<memory>
#include"MeshCommon.h"

class DrawGrid {
public:
    static void Finalize();
    static void Create();
    static void Draw(Camera& camera);
private:
    static std::array<std::unique_ptr<Primitive>, 102> line_;
    static  std::array <std::unique_ptr<Primitive>, 2> cube_;

    static std::array< std::unique_ptr<Object3d>, 102> lineTransforms_;



};
