#pragma once
#include<d3d12.h>
#include"RootSignature.h"
#include"CompileShader.h"

//ComPtr(コムポインタ)
#include<wrl.h>
class ComputeShaderPSO
{
private:

    DxcCompiler* dxCompiler_ = nullptr;
    RootSignature* rootSignature_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> computePipelineStatesForSkinning_ = nullptr;
private:
    ComputeShaderPSO() = default;
public:

    static ComputeShaderPSO* GetInstance()
    {
        static ComputeShaderPSO instance;
        return &instance;
    }
    Microsoft::WRL::ComPtr<ID3D12PipelineState>& GetSkinningPSO() {
        return computePipelineStatesForSkinning_
            ;
    };
    void CreatePSO(
        DxcCompiler* dxCompiler,
        RootSignature* rootSignature);

    ~ComputeShaderPSO();
    Microsoft::WRL::ComPtr <ID3D12PipelineState> Create(
        const RootSignature::TYPE& rootSignatureType,
        const  DxcCompiler::CS_TYPE& csShaderType);
};

