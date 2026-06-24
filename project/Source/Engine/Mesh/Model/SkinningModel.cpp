#include "SkinningModel.h"
#include"SRVmanager/SrvManager.h"
#include"Model.h"

SkinningModel::SkinningModel()
{
    skeleton_ = std::make_unique<Skeleton>();
    skinCluster_ = std::make_unique<SkinCluster>();
}

void SkinningModel::SetModel(Model* model)
{
    //モデルのメッシュ
    modelData_ = model->GetModelData();
    //スケルトンやクラスターを作成する
    *skeleton_ = Bone::CreateSkeleton(modelData_->rootNode);
    *skinCluster_ = CreateSkinCluster(*skeleton_, *modelData_);
}

void SkinningModel::SetRootSignatureAndGraphicsPipeline(
    ID3D12GraphicsCommandList* commandList,
    const BlendMode& blendMode,
    const CullMode& cullMode,
    const MaskMode maskMode,
    const bool usePSOKey,
    const RootSignature::TYPE rootSignatureType,
    const DxcCompiler::VS_TYPE vsType,
    const DxcCompiler::PS_TYPE psType
)
{
    (void)maskMode;
    (void)usePSOKey;
    (void)rootSignatureType;
    (void)vsType;
    (void)psType;
    //InputLayout kInputLayoutTypeSkinning;

    commandList->SetGraphicsRootSignature(PSO::GetRootSignature()->GetRootSignature(RootSignature::SKINNING));
    commandList->SetPipelineState(PSO::GetGraphicsPipelineStatesSkinning(blendMode, cullMode).Get());//PSOを設定
    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。

}

Matrix4x4* SkinningModel::GetJointMatrix(const std::string jointName)
{
    return  Bone::GetJointMatrix(jointName, *skeleton_);
}
