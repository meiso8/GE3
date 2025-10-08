#pragma once

#include"LineMesh.h"
#include"Cube.h"

class DrawGrid {
public:
    DrawGrid(uint32_t textureHanddle);
    void Draw(Camera& camera);
private:
    LineMesh line_[102];
    Cube cube_[2];
};
