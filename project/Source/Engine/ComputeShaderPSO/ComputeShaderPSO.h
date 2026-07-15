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
    Microsoft::WRL::ComPtr<ID3D12PipelineState> computePipelineStatesForInitializeParticle_ = nullptr;
private:
    ComputeShaderPSO() = default;
    ~ComputeShaderPSO() = default;
public:

    static ComputeShaderPSO* GetInstance()
    {
        static ComputeShaderPSO instance;
        return &instance;
    }

    ComputeShaderPSO(const ComputeShaderPSO&) = delete;
    ComputeShaderPSO& operator=(const ComputeShaderPSO&) = delete;

    Microsoft::WRL::ComPtr<ID3D12PipelineState>& GetSkinningPSO() {
        return computePipelineStatesForSkinning_
            ;
    };

    Microsoft::WRL::ComPtr<ID3D12PipelineState>& GetInitializeParticlePSO() {
        return computePipelineStatesForInitializeParticle_
            ;
    };

    void CreatePSO(
        DxcCompiler* dxCompiler,
        RootSignature* rootSignature);
    void Finalize();

    Microsoft::WRL::ComPtr <ID3D12PipelineState> Create(
        const RootSignature::TYPE& rootSignatureType,
        const  DxcCompiler::CS_TYPE& csShaderType);
};

