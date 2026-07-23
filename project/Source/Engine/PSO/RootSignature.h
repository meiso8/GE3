#pragma once

#include<d3d12.h>
#include<wrl.h>
#include<cstdint>
#include<array>
#include<cassert>
class RootSignature
{
public:

    enum TYPE {
        NORMAL,
        PARTICLE,
        PARTICLE_GPU,
        SPRITE,
        FONT,
        SKYBOX,
        OFFSCREEN,
        GRAYSCALE,
        VIGNETTE,
        BOXFILTER,
        GAUSSIANFILTER,
        LUMINANCE_BASED_OUTLINE,
        DEPTH_BASED_OUTLINE,
        RADIAL_BLUR,
        DISSOLVE,
        RANDOM,
        //モザイク
        MOSAIC,
        //サーモグラフィー
        THERMOGRAPHY,
        //ビーム用
        BEAM,
        //ライン用
        LINE,
        //ComputeShader用Skinning
        CS_SKINNING,
        CS_INITIALIZE_PARTICLE,
        CS_EMIT_PARTICLE,
        CS_UPDATE_PARTICLE,
        TYPES,
    };

    void Create();
    ID3D12RootSignature* GetRootSignature(TYPE index) {
        assert(index < rootSignatures_.size());
        return
            rootSignatures_[index].Get();
    };

private:
    std::array<Microsoft::WRL::ComPtr <ID3D12RootSignature>, TYPES> rootSignatures_ = { nullptr };
};

