#include "SkinningModel.h"
#include"Model.h"
#include"DirectXCommon.h"
#include"Log.h"
#include"SrvDescriptorHeap.h"
SkinningModel::SkinningModel()
{
    skeleton_ = std::make_unique<Skeleton>();
    skinCluster_ = std::make_unique<SkinCluster>();
}

SkinningModel::~SkinningModel()
{

    //Skin::ReleaseSkinCluster(*skinCluster_);

    //skinningCSResorce_.inputVertexResource_.Reset();
    //skinningCSResorce_.outputVertexResource_.Reset();
    //skinningCSResorce_.skinningInformationResource_.Reset();

}

void SkinningModel::SetDescriptorHeap(CbvSrvUavDescriptorHeap* cbvSrvUavDescriptorHeap)
{
    cbvSrvUavDescriptorHeap_ = cbvSrvUavDescriptorHeap;
}

void SkinningModel::SetModel(Model* model)
{
    //モデルのメッシュ
    modelData_ = model->GetModelData();
    //スケルトンやクラスターを作成する
    *skeleton_ = Bone::CreateSkeleton(modelData_->rootNode);
    *skinCluster_ = Skin::CreateSkinCluster(*skeleton_, *modelData_);
    
    CreateSkinningInformation();
    CreateInputVertexResource();
    CreateOutVertexResource();

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

    commandList->SetGraphicsRootSignature(PSO::GetRootSignature()->GetRootSignature(RootSignature::NORMAL));
    commandList->SetPipelineState(PSO::GetGraphicsPipelineState(blendMode, cullMode).Get());//PSOを設定

}

Matrix4x4* SkinningModel::GetJointMatrix(const std::string jointName)
{
    return  Bone::GetJointMatrix(jointName, *skeleton_);
}

void SkinningModel::CreateInputVertexResource()
{

    UINT vertexBufferSize = sizeof(VertexData) *UINT( modelData_->vertices.size());
    skinningCSResorce_.inputVertexResource_.CreateBufferResource(L"inputVertexResource", vertexBufferSize);

    //頂点リソースに頂点をマッピング
    skinningCSResorce_.inputVertexResource_.Map();

    std::memcpy(skinningCSResorce_.inputVertexResource_.data, modelData_->vertices.data(), vertexBufferSize); // vectorの中身を一気にコピー！
    skinningCSResorce_.inputVertexResource_.UnMap();
    skinningCSResorce_.inputVertexResource_.Allocate(cbvSrvUavDescriptorHeap_);
    skinningCSResorce_.inputVertexResource_.CreateSRVforStructuredBuffer(cbvSrvUavDescriptorHeap_, UINT(modelData_->vertices.size()));

    LogFile::Log("Create InputVertexresource");
}

void SkinningModel::CreateOutVertexResource()
{

    UINT vertexBufferSize = sizeof(VertexData) *UINT( modelData_->vertices.size());
    //UAVの作成をするよ
    skinningCSResorce_.outputVertexResource_.CreateBufferResourceForUAV(L"outputVertexResource",vertexBufferSize);
    skinningCSResorce_.outputVertexResource_.Allocate(cbvSrvUavDescriptorHeap_);
    skinningCSResorce_.outputVertexResource_.CreateUAV(cbvSrvUavDescriptorHeap_,UINT(modelData_->vertices.size()));

    // 頂点バッファビューの作成 上書き
    vertexBufferView_.BufferLocation = skinningCSResorce_.outputVertexResource_.GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = vertexBufferSize;
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

    LogFile::Log("Create  OutputVertexresource");
}
void SkinningModel::CreateSkinningInformation()
{
    skinningCSResorce_.skinningInformationResource_.CreateBufferResource(L"inputVertexResource");
    //書き込むためのアドレスを取得
    skinningCSResorce_.skinningInformationResource_.Map();
    LogFile::Log("Create SkinningInformation");
    skinningCSResorce_.skinningInformationResource_.data->numVertices = static_cast<uint32_t>(modelData_->vertices.size());
}