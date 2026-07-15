
#include"ParticleStruct.h"

class CbvSrvUavDescriptorHeap;

class GPUParticleManager {
public:
  
  
private:
    RootSignature* rootSignature_ = nullptr;
    ID3D12GraphicsCommandList* commandList_ = nullptr;
    CbvSrvUavDescriptorHeap* cbvSrvUavDescriptorHeap_ = nullptr;
    std::unique_ptr<ParticleGroupGPU> particleGroup_ = nullptr;
private:
    void CreateGroup();
public:
    ParticleGroupGPU* GetGroup() { return particleGroup_.get(); };
    void Create(
        RootSignature* rootSignature,
        CbvSrvUavDescriptorHeap* cbvSrvUavDescriptorHeap,
        ID3D12GraphicsCommandList* commandList
        );
    void Initialize();

    void Draw(Camera& camera);
};