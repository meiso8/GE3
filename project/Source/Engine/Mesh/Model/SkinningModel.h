#pragma once

#include"Bone.h"
#include"SkinCluster.h"
#include"Primitive.h"
#include<memory>
#include"Depth.h"
#include"VertexData.h"
#include"../ResourceManager/ResourceManager.h"

class Model;
class CbvSrvUavDescriptorHeap;
class SkinningModel :public Primitive
{
public:
    struct SkinningInformation
    {
        uint32_t numVertices;
    };

    struct SkinningCSResorce {
        SRVResource<VertexData> inputVertexResource_;
        UAVResource<VertexData> outputVertexResource_;
        CResource<SkinningInformation> skinningInformationResource_;
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
    CbvSrvUavDescriptorHeap* cbvSrvUavDescriptorHeap_ = nullptr;
    std::unique_ptr< Skeleton> skeleton_ = nullptr;
    std::unique_ptr< SkinCluster> skinCluster_ = nullptr;
    ModelData* modelData_ = nullptr;

    SkinningCSResorce skinningCSResorce_;
};

