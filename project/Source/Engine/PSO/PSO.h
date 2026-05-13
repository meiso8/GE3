#pragma once

#include<d3d12.h>

//ComPtr(コムポインタ)
#include<wrl.h>
#include"RootSignature.h"
#include"InputLayout.h"
#include"CompileShader.h"
#include"BlendState.h"
#include"RasterizerState.h"
#include"Depth.h"//StencilTextureの作成関数　奥行き
#include<memory>
#include<array>
#include<vector>
#include <functional>

#include<unordered_map>

class PSO {
public:


    enum TopologyType {
        kTriangle,
        kLine,
        kPoint,
        TopologyTypes
    };

    enum EffectType {
        kEffectNone,
        kEffectGrayScale,
        kEffectVignette,
        kEffectBoxFilter,
        kEffectGaussianFilter,
        kEffectLuminanceBasedOutline,
        kEffectDepthBasedOutline,
        kEffectRadialBlur,
        kEffectDissolve,
    /*    kEffectRandom,*/
        kCountOfEffect,

    };


    // PSOの構成要素をまとめたキー構造体
    struct PSOKey {
        RootSignature::TYPE rootSignatureType;
        DxcCompiler::VS_TYPE vsShaderType;
        DxcCompiler::PS_TYPE psShaderType;
        uint32_t blendMode;
        uint32_t cullMode;
        uint32_t depthMode;     // 追加：Depthのモード (kAll, kZero, kNoneなど)
        PSO::TopologyType topologyType;
        InputLayout::InputLayoutType inputLayoutType;

        // unordered_mapのキーとして使うための比較演算子
        bool operator==(const PSOKey& other) const {
            return rootSignatureType == other.rootSignatureType &&
                vsShaderType == other.vsShaderType &&
                psShaderType == other.psShaderType &&
                blendMode == other.blendMode &&
                cullMode == other.cullMode &&
                depthMode == other.depthMode &&
                topologyType == other.topologyType &&
                inputLayoutType == other.inputLayoutType;
        }
    };

    struct PSOKeyHasher {
        std::size_t operator()(const PSOKey& key) const {
            // 簡易的なハッシュの結合
            std::size_t h1 = std::hash<int>()(static_cast<int>(key.rootSignatureType));
            std::size_t h2 = std::hash<int>()(static_cast<int>(key.vsShaderType));
            std::size_t h3 = std::hash<int>()(static_cast<int>(key.psShaderType));
            std::size_t h4 = std::hash<int>()(static_cast<int>(key.blendMode));
            std::size_t h5 = std::hash<int>()(static_cast<int>(key.cullMode));
            std::size_t h6 = std::hash<int>()(static_cast<int>(key.depthMode));
            std::size_t h7 = std::hash<int>()(static_cast<int>(key.topologyType));
            std::size_t h8 = std::hash<int>()(static_cast<int>(key.inputLayoutType));
            // 必要に応じて他の要素もXORで混ぜる
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^ (h6 << 5) ^ (h7 << 6) ^ (h8 << 7);
        }
    };

    static PSO* GetInstance()
    {
        static PSO instance;
        return &instance;
    }

    static Microsoft::WRL::ComPtr <ID3D12PipelineState>& GetGraphicsPipelineState(uint32_t blendMode,uint32_t cullMode ) {
        return graphicsPipelineStates_[blendMode][cullMode];
    }
    static Microsoft::WRL::ComPtr <ID3D12PipelineState>& GetGraphicsPipelineStatesSkinning(uint32_t blendMode, uint32_t cullMode) {
        return graphicsPipelineStatesSkinning_[blendMode][cullMode];
    }
    static Microsoft::WRL::ComPtr <ID3D12PipelineState>& GetGraphicsPipelineStateParticle(uint32_t blendMode) {
        return graphicsPipelineStatesParticle_[blendMode];
    }

    static Microsoft::WRL::ComPtr <ID3D12PipelineState>& GetGraphicsPipelineStateLine() {
        return graphicsPipelineStatesLine_;
    }
    static Microsoft::WRL::ComPtr <ID3D12PipelineState>& GetGraphicsPipelineStateSkyBox() {
        return graphicsPipelineStateSkyBox_;
    }

    static Microsoft::WRL::ComPtr <ID3D12PipelineState>& GetGraphicsPipelineStateSprite(uint32_t blendMode) {
        return graphicsPipelineStateSprite_[blendMode];
    }
    static Microsoft::WRL::ComPtr <ID3D12PipelineState>& GetGraphicsPipelineStateFont(uint32_t blendMode) {
        return graphicsPipelineStateFont_[blendMode];
    }

    static Microsoft::WRL::ComPtr <ID3D12PipelineState>& GetGraphicsPipelineStateOffScreen(uint32_t effectType){
        return graphicsPipelineStateOffScreen_[effectType];
    }
    static Microsoft::WRL::ComPtr <ID3D12PipelineState>& GetGraphicsPipelineStateRandom(uint32_t blendMode) {
        return graphicsPipelineStateRandom_[blendMode];
    }

    void CreateALLPSO();

    static RootSignature* GetRootSignature() { return rootSignature.get(); }
    ~PSO();
    
    // この関数経由でPSOを取得する
    static Microsoft::WRL::ComPtr<ID3D12PipelineState> GetOrCreatePSO(const PSOKey& key);

private:
    Microsoft::WRL::ComPtr <ID3D12PipelineState> Create(
        const BlendMode& blendMode,
        const CullMode& cullMode,
        const MaskMode& maskMode,
        bool useDepthFormat,
        const RootSignature::TYPE& rootSignatureType,
        const DxcCompiler::VS_TYPE& vsShaderType,
        const  DxcCompiler::PS_TYPE& psShaderType,
        const TopologyType topologyType,
        const InputLayout::InputLayoutType inputLayoutType);
public:
    static std::unique_ptr<RootSignature>rootSignature;
private:
    static std::array<std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kCountOfCullMode>, kCountOfBlendMode> graphicsPipelineStates_;

    static std::array<std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kCountOfCullMode>, kCountOfBlendMode> graphicsPipelineStatesSkinning_;
    static std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState> ,kCountOfBlendMode> graphicsPipelineStatesParticle_;
    static Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStatesLine_;
    static std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kCountOfBlendMode> graphicsPipelineStateSprite_;
    static std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kCountOfBlendMode> graphicsPipelineStateFont_;

    static Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateSkyBox_;
    static std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kCountOfEffect> graphicsPipelineStateOffScreen_;
    static std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kCountOfBlendMode> graphicsPipelineStateRandom_;
    

    static std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kCountOfBlendMode> graphicsPipelineStatesForEffectObject_;

    static std::unordered_map<PSOKey, Microsoft::WRL::ComPtr<ID3D12PipelineState>, PSOKeyHasher> psoCache_;


    std::unique_ptr<InputLayout>inputLayout = nullptr;
    std::vector<BlendState> blendStates = {};
    std::vector<RasterizerState> rasterizerStates = {};
    std::vector<DepthStencil>  depthStencils = {};
};