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
    Primitive::~Primitive();
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
    skinningCSResorce_.inputVertexResources_.resource =
        DirectXCommon::CreateBufferResource(vertexBufferSize);
    skinningCSResorce_.inputVertexResources_.resource->SetName(L"inputVertexResource");

    //頂点リソースに頂点をマッピング
    skinningCSResorce_.inputVertexResources_.resource->Map(0, nullptr, reinterpret_cast<void**>(&inputVertexData_));
    std::memcpy(inputVertexData_, modelData_->vertices.data(), vertexBufferSize); // vectorの中身を一気にコピー！
    skinningCSResorce_.inputVertexResources_.resource->Unmap(0, nullptr);
    
    skinningCSResorce_.inputVertexResources_.index = cbvSrvUavDescriptorHeap_->Allocate();
    cbvSrvUavDescriptorHeap_->CreateSRVforStructuredBuffer(skinningCSResorce_.inputVertexResources_.index, skinningCSResorce_.inputVertexResources_.resource.Get(),UINT( modelData_->vertices.size()), sizeof(VertexData));

    LogFile::Log("Create InputVertexresource");
}

void SkinningModel::CreateOutVertexResource()
{

    UINT vertexBufferSize = sizeof(VertexData) *UINT( modelData_->vertices.size());
    //UAVの作成をするよ

    skinningCSResorce_.outputVertexResources_.resource =
        DirectXCommon::CreateBufferResourceForUAV(vertexBufferSize);
    skinningCSResorce_.outputVertexResources_.resource->SetName(L"outputVertexResource");

    skinningCSResorce_.outputVertexResources_.index = cbvSrvUavDescriptorHeap_->Allocate();
    cbvSrvUavDescriptorHeap_->CreateUAV(
        skinningCSResorce_.outputVertexResources_.index,
        skinningCSResorce_.outputVertexResources_.resource.Get(),
        UINT(modelData_->vertices.size()), 
        sizeof(VertexData)
    );
    // 1. 頂点バッファの作成とデータ転送

    // 3. 頂点バッファビューの作成 上書き
    vertexBufferView_.BufferLocation = skinningCSResorce_.outputVertexResources_.resource->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = vertexBufferSize;
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

    LogFile::Log("Create  OutputVertexresource");
}
void SkinningModel::CreateSkinningInformation()
{
    skinningCSResorce_.skinningInformationResource_ = DirectXCommon::CreateBufferResource(sizeof(SkinningInformation));
    skinningCSResorce_.skinningInformationResource_->SetName(L"inputVertexResource");
    //書き込むためのアドレスを取得
    skinningCSResorce_.skinningInformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&skinningInformation_));
    LogFile::Log("Create SkinningInformation");
    skinningInformation_->numVertices = static_cast<uint32_t>(modelData_->vertices.size());
}