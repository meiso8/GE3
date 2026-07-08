#pragma once

#include"Bone.h"
#include"SkinCluster.h"
#include"Primitive.h"
#include<memory>
#include"Depth.h"
#include"VertexData.h"

class Model;
class CbvSrvUavDescriptorHeap;
class SkinningModel :public Primitive
{
public:
    struct Resources {
        Microsoft::WRL::ComPtr <ID3D12Resource> resource = nullptr;
        uint32_t index;
    };

    struct SkinningCSResorce {
        Resources inputVertexResources_;
        Resources outputVertexResources_;
        Microsoft::WRL::ComPtr <ID3D12Resource> skinningInformationResource_ = nullptr;
    };
public:
    SkinningModel();
    ~SkinningModel();
    void SetDescriptorHeap(CbvSrvUavDescriptorHeap* cbvSrvUavDescriptorHeap);
    void SetModel(Model* model);
    void SetRootSignatureAndGraphicsPipeline(
        ID3D12GraphicsCommandList* commandList,
        const BlendMode& blendMode,
        const CullMode& cullMode = CullMode::kCullModeBack,
        const MaskMode maskMode = kAll,
        const bool usePSOKey = false,
        const RootSignature::TYPE rootSignatureType = RootSignature::TYPE::NORMAL,
        const DxcCompiler::VS_TYPE vsType = DxcCompiler::VS_TYPE::VS_Normal,
        const DxcCompiler::PS_TYPE psType = DxcCompiler::PS_TYPE::PS_Normal);

    Skeleton* GetSkeleton() {
        return skeleton_.get();
    };
    SkinCluster* GetSkinCluster() {
        return skinCluster_.get();
    };

    ModelData* GetModelData() {
        return modelData_;
    }
    SkinningCSResorce* GetCSResource() {
        return &skinningCSResorce_;
    };
    Matrix4x4* GetJointMatrix(const std::string jointName);
private:
    void CreateInputVertexResource();
    void CreateOutVertexResource();
    void CreateSkinningInformation();
private:

    struct SkinningInformation
    {
        uint32_t numVertices;
    };

private:
    CbvSrvUavDescriptorHeap* cbvSrvUavDescriptorHeap_ = nullptr;
    std::unique_ptr< Skeleton> skeleton_ = nullptr;
    std::unique_ptr< SkinCluster> skinCluster_ = nullptr;
    ModelData* modelData_ = nullptr;

    VertexData* inputVertexData_ = nullptr;
    VertexData* outputVertexData_ = nullptr;
    SkinningInformation* skinningInformation_ = nullptr;

    SkinningCSResorce skinningCSResorce_;
};

