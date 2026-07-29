#pragma once

#include"hlslTypeToCpp.h"
#include"ResourceManager/ResourceManager.h"
#include"../ParticleStruct.h"
struct EmitterSphere {
    float3 translate;//位置
    float radius;//半径
    uint32_t count;
    float frequency;//射出間隔
    float frequencyTime;//射出間隔調整時間
    uint32_t emit;//射出許可
};

struct PerFrame
{
    float time;
    float deltaTime;
    float2 padding;
};

class CbvSrvUavDescriptorHeap;

class GPUParticleEmitter {
public:
    static void SetCommandListAndCbvSrvUavDescriptorHeap(
        ID3D12GraphicsCommandList* commandList,
        CbvSrvUavDescriptorHeap* cbvSrvUavDescriptorHeap
    );
    void Create();
    void Initialize();
    void Update();
    void SetParticleGroup(ParticleGroupGPU* particleGroup);
private:
    static CbvSrvUavDescriptorHeap* cbvSrvUavDescriptorHeap_;
    static ID3D12GraphicsCommandList* commandList_;
    ParticleGroupGPU*particleGroup_ = nullptr;

    //エミッターリソース
    CResource<EmitterSphere>emitterResource_;
    CResource<PerFrame>perFrameResource_;

};