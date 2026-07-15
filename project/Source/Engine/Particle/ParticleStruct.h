#pragma once

#include"hlslTypeToCpp.h"
#include"ResourceManager/ResourceManager.h"
#include"MaterialData.h"
#include"Primitive.h"
#include"Object3d.h"

struct ParticleCS {
    float32_t3 translate;
    float32_t lifeTime;
    float32_t3 scale;
    float32_t currentTime;
    float32_t3 velocity;
    float32_t padding;
    float32_t4 color;
};

struct ParView
{
    float32_t4x4 viewProjection;
    float32_t4x4 billboardMatrix;
};

struct ParticleGroupGPU {

    MaterialData materialData;
    std::unique_ptr<Primitive> primitive = nullptr;
    CResource<Object3d::Material>materialResource;
    CResource<ParView>parViewResource;
    UAVResource<ParticleCS>particleUAVResource_;
    UAVResource<int>particleGFreeCounterUAVResource_;
};