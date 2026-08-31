#pragma once

#include<array>
#include"Object3d.h"
#include<memory>
#include"Primitive.h"
#include"LineObject3d/LineObject3d.h"

class DrawGrid {
public:
    static void Finalize();
    static void Create();
    static void Draw();
private:

    static std::array <std::unique_ptr<Primitive>, 2> cube_;
    static std::array< std::unique_ptr<Object3d>, 2> cubes_;
    static std::array< std::unique_ptr<LineObject3d>, 102> lineTransforms2_;


};
