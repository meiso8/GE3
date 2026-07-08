#include "ComputeShaderPSO.h"
#include"DirectXCommon.h"


void ComputeShaderPSO::CreatePSO(DxcCompiler* dxCompiler,
    RootSignature* rootSignature)
{
    dxCompiler_ = dxCompiler ;
    rootSignature_ = rootSignature;

    computePipelineStatesForSkinning_ =  Create(RootSignature::TYPE::CS_SKINNING, DxcCompiler::CS_Skinning);
}

ComputeShaderPSO::~ComputeShaderPSO()
{
    if (computePipelineStatesForSkinning_) {
        computePipelineStatesForSkinning_.Reset();
    }
}


Microsoft::WRL::ComPtr<ID3D12PipelineState> ComputeShaderPSO::Create(
    const RootSignature::TYPE& rootSignatureType,
    const DxcCompiler::CS_TYPE& csShaderType)
{

    D3D12_COMPUTE_PIPELINE_STATE_DESC computePiplineStateDesc{};

    //シェーダーを取得
    auto& csShader = dxCompiler_->GetComputeShaderBlob(csShaderType);
    //ディスクを作成
    computePiplineStateDesc.CS = {
        .pShaderBytecode = csShader->GetBufferPointer(),
        .BytecodeLength = csShader->GetBufferSize()
    };
    //ルートシグネチャを取得
    computePiplineStateDesc.pRootSignature = rootSignature_->GetRootSignature(rootSignatureType);
    Microsoft::WRL::ComPtr<ID3D12PipelineState> computePipelineState = nullptr;
    HRESULT hr = DirectXCommon::GetDevice()->CreateComputePipelineState(&computePiplineStateDesc, IID_PPV_ARGS(&computePipelineState));

    return computePipelineState;
}
