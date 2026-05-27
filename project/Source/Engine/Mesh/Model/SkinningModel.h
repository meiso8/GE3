#pragma once

#include"Bone.h"
#include"SkinCluster.h"
#include"MeshCommon.h"
#include<memory>
class Model;
class SkinningModel :public Primitive
{
public:
    SkinningModel();
    void SetModel(Model* model);
    void PreDraw(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode = BlendMode::kBlendModeNormal, const CullMode& cullMode = CullMode::kCullModeBack)override;
    void Draw(ID3D12GraphicsCommandList* commandList)override;
    
    Skeleton* GetSkeleton() {
        return skeleton_.get();
    };
    SkinCluster* GetSkinCluster() {
        return skinCluster_.get();
    };

    ModelData* GetModelData() {
        return modelData_;
    }

private:
    std::unique_ptr< Skeleton> skeleton_;
    std::unique_ptr< SkinCluster> skinCluster_;
    ModelData* modelData_ = nullptr;
};

