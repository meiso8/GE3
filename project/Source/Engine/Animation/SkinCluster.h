#pragma once
#include<vector>
#include"Matrix4x4.h"
#include<wrl.h>
#include<d3d12.h>
#include<span>//所有権を持たずに参照範囲を限定するクラス
#include<array>

//インフルエンス 最大4Jointの影響を受ける
const uint32_t kNumMaxInfluence = 4;

struct Skeleton;
struct ModelData;
class CbvSrvUavDescriptorHeap;

struct VertexInfluence {
    std::array<float, kNumMaxInfluence> weights;
    std::array<int32_t, kNumMaxInfluence> jointIndices;
};

struct WellForGPU {
    Matrix4x4 skeletonSpaceMatrix;//位置用
    Matrix4x4 skeletonSpaceInverseTransposeMatrix;//法線用
};


struct SkinCluster {
    std::vector<Matrix4x4> inverseBindPoseMatrices;

    Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
    D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
    std::span<VertexInfluence> mappedInfluence;

    Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
    std::span <WellForGPU> mappedPalette;
    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;
    
    uint32_t paletteSrvIndex;
    uint32_t influenceSrvIndex;
};

class Skin {
private:
    static CbvSrvUavDescriptorHeap* srvDescriptorHeap_;
public:
    static void SetSrvDescriptorHeap(CbvSrvUavDescriptorHeap* srvDescriptorHeap);
    static SkinCluster CreateSkinCluster(const Skeleton& skeleton, const ModelData& modelData);
    static void UpdateSkinCluster(SkinCluster& skinCluster, const Skeleton& skeleton);
    static void ReleaseSkinCluster(SkinCluster& slinCluster);
};

