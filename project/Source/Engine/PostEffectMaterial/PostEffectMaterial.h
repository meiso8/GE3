#pragma once
#include"hlslTypeToCpp.h"
#include<stdint.h>
#include"Vector4.h"
#include<d3d12.h>
#include<wrl.h>
#include<array>
#include<PSO.h>
#include"ResourceManager/ResourceManager.h"

class Camera;
class PostEffectMaterial
{
public:
    struct MaterialForRenderTexture {
        float4 color;
        bool useEffect = false;
        uint32_t type = false;
        bool padding[2] = { };
    };

    struct MaterialForVignette
    {
        float correctVal;
        float viignetteVal;
        float padding[2];
    };


    struct MaterialForBoxFilter
    {
        float kernel;
        float padding[3];
    };


    struct MaterialForGaussianFilter
    {
        int32_t kernel;
        float sigma;
        float padding[2];
    };

    struct MaterialForLuminanceBasedOutline
    {
        float weightVal;
        float padding[3];
    };

    struct MaterialForDepthBasedOutline
    {
        float32_t4x4 projectionInverse;
        float32_t lineWidth;
        float32_t3 color;
    };


    struct MaterialForRadialBlur
    {
        float32_t2 center;
        int32_t numSamples;
        float32_t blurWidth;
    };

    struct MaterialForDissolve
    {
        float32_t maskVal;
        float32_t3 rgb;
    };

    struct MaterialForRandom
    {
        float time;
        float padding[3];
    };
    struct MaterialForThermography
    {
        float alpha;
        int32_t kernel;
        float sigma;
        float padding[1];
    };
public:
    void SetCamera(Camera* camera);

    void Update();
    void Create();
    void Clear();
    ~PostEffectMaterial();
    MaterialForGaussianFilter* GetMaterialGaussianFilter() { return materialForGaussianFilter_.data; };
    MaterialForRenderTexture* GetMaterialGrayScale() {return materialForGrayScale_.data; };
    MaterialForVignette* GetMaterialForVignette() { return materialForVignette_.data; };
    MaterialForBoxFilter* GetMaterialForBoxFilter() {return materialForBoxFilter_.data;  };

    MaterialForLuminanceBasedOutline* GetMaterialForLuminanceBasedOutline() { return materialForLuminanceBasedOutline_.data; };
    MaterialForDepthBasedOutline* GetMaterialForDepthBasedOutline(){ return  materialForDepthBasedOutline_.data; };
    MaterialForRadialBlur* GetMaterialForRadialBlur(){ return materialForRadialBlur_.data; };
    MaterialForDissolve* GetMaterialForDissolve(){ return materialForDissolve_.data; };
    MaterialForThermography* GetMaterialThermography() { return materialForThermography_.data; };
    MaterialForRandom* GetmaterialForRandom() { return materialForRandom_.data; };
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress(const PSO::EffectType& type);
private:
    Camera* camera_ = nullptr;   
    const Vector4 sepiaColor_ = { 1.0f,74.0f / 107.0f,43.0f / 107.0f,1.0f };

    CResource <PostEffectMaterial::MaterialForRenderTexture> materialForGrayScale_;
    CResource <PostEffectMaterial::MaterialForVignette> materialForVignette_;
    CResource <PostEffectMaterial::MaterialForBoxFilter> materialForBoxFilter_;
    CResource <PostEffectMaterial::MaterialForRenderTexture> materialForFullScreen_;
    CResource <PostEffectMaterial::MaterialForGaussianFilter> materialForGaussianFilter_;
    CResource <PostEffectMaterial::MaterialForLuminanceBasedOutline> materialForLuminanceBasedOutline_;
    CResource <PostEffectMaterial::MaterialForDepthBasedOutline> materialForDepthBasedOutline_;
    CResource <PostEffectMaterial::MaterialForRadialBlur> materialForRadialBlur_;
    CResource <PostEffectMaterial::MaterialForDissolve> materialForDissolve_;
    CResource <PostEffectMaterial::MaterialForThermography> materialForThermography_;
    CResource <PostEffectMaterial::MaterialForRandom> materialForRandom_;
    std::unordered_map<PSO::EffectType, ID3D12Resource*> resourceMap_;
private:
    void CreateMaterialBufferForGrayScale();
    void CreateMaterialBufferForVignette();
    void CreateMaterialBufferForBoxFilter();
    void CreateMaterialBUfferForFullScreen();
    void CreateMaterialBufferForGaussianFilter();
    void CreateMaterialLuminanceBasedOutline();
    void CreateMaterialDepthBasedOutline();
    void CreateMaterialRadialBlur();
    void CreateMaterialDissolve();
    void CreateMaterialRandom();
    void CreateMaterialThermography();
};

