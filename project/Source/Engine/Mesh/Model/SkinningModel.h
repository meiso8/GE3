#pragma once

#include"Bone.h"
#include"SkinCluster.h"
#include"Primitive.h"
#include<memory>
#include"Depth.h"

class Model;
class SkinningModel :public Primitive
{
public:
    SkinningModel();
    void SetModel(Model* model);
    void SetRootSignatureAndGraphicsPipeline(
        ID3D12GraphicsCommandList* commandList,
        const BlendMode& blendMode,
        const CullMode& cullMode = CullMode::kCullModeBack,
        const MaskMode maskMode = kAll,
        const bool usePSOKey = false,
        const RootSignature::TYPE rootSignatureType = RootSignature::TYPE::SKINNING,
        const DxcCompiler::VS_TYPE vsType = DxcCompiler::VS_TYPE::VS_Skinning,
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
    Matrix4x4* GetJointMatrix(const std::string jointName);

private:
    std::unique_ptr< Skeleton> skeleton_;
    std::unique_ptr< SkinCluster> skinCluster_;
    ModelData* modelData_ = nullptr;
};

