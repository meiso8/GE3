#pragma once

#include"LineMesh.h"
#include"Cube.h"

class DrawGrid {
public:
    DrawGrid(ModelConfig& mc, PSO& pso);
    void Draw(ShaderResourceView& srv, Camera& camera);
private:
    LineMesh line_[102];
    Cube cube_[2];
    PSO* pso_ = nullptr;
};
