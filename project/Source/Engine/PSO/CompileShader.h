#pragma once

#include <dxcapi.h>
#include<string>
#include<wrl.h>
#include<array>

class DxcCompiler
{
public:
    //CompileShader関数
    enum VS_TYPE {
        VS_Normal,
        VS_Particle,
        //GPU Particle
        VS_Particle_GPU,

        VS_Sprite,
        //VS_Skinning,
        VS_SkyBox,
        VS_OffScreen,
        VS_Beam,
        VS_Line,
        VS_Shaders,
    };

    enum PS_TYPE {

        PS_Normal,
        PS_Particle,
        PS_Sprite,
        PS_Font,
        PS_SkyBox,
        PS_OffScreen,
        PS_GrayScale,
        PS_Vignette,
        PS_BoxFilter,
        PS_GaussianFilter,
        PS_LuminanceBasedOutline,
        PS_DepthBasedOutline,
        PS_RadialBlur,
        PS_Dissolve,
        PS_Random,
        PS_Thermography,
        PS_Beam,
        PS_Line,
        PS_Shaders,
    };

    enum CS_TYPE {
        CS_Skinning,
        CS_Particle_GPU,
        CS_Shaders,
    };

    void Initialize();
    void ShaderSetting();
    Microsoft::WRL::ComPtr <IDxcBlob>& GetVertexShaderBlob(uint32_t index) { return vertexShaderBlobs_[index]; };
    Microsoft::WRL::ComPtr <IDxcBlob>& GetPixelShaderBlob(uint32_t index) { return pixelShaderBlobs_[index]; };

    Microsoft::WRL::ComPtr <IDxcBlob>& GetComputeShaderBlob(uint32_t index) { return computeShaderBlobs_[index]; };
private:
    IDxcUtils* dxcUtils_ = nullptr;
    IDxcCompiler3* dxcCompiler_ = nullptr;
    IDxcIncludeHandler* includeHandler_ = nullptr;

    std::array<Microsoft::WRL::ComPtr<IDxcBlob>, VS_Shaders> vertexShaderBlobs_;
    std::array<Microsoft::WRL::ComPtr<IDxcBlob>, PS_Shaders>pixelShaderBlobs_;

    //ComputeShader用を作成
    std::array<Microsoft::WRL::ComPtr<IDxcBlob>, CS_Shaders>computeShaderBlobs_;

private:
    Microsoft::WRL::ComPtr<IDxcBlob>CompileShader(
        //CompilerするShaderファイルへのパス
        const std::wstring& filePath,
        //Compilerに使用するProfile
        const wchar_t* profile);

};