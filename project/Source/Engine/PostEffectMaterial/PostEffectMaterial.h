#pragma once
#include"hlslTypeToCpp.h"
#include<stdint.h>
#include"Vector4.h"
#include<d3d12.h>
#include<wrl.h>
#include<array>
#include<PSO.h>

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

    Microsoft::WRL::ComPtr<ID3D12Resource>& GetMaterialResouce(const PSO::EffectType& effectType);

    void Update();
    void Create();
    void Clear();
  
    MaterialForGaussianFilter* GetMaterialGaussianFilter() { return materialForGaussianFilter_; };
    MaterialForRenderTexture* GetMaterialGrayScale() {return materialForGrayScale_; };
    MaterialForVignette* GetMaterialForVignette() { return materialForVignette_; };
    MaterialForBoxFilter* GetMaterialForBoxFilter() {return materialForBoxFilter_;  };

   /* MaterialForRenderTexture* materialForFullScreen_ = nullptr;*/

    MaterialForLuminanceBasedOutline* GetMaterialForLuminanceBasedOutline() { return materialForLuminanceBasedOutline_; };
    MaterialForDepthBasedOutline* GetMaterialForDepthBasedOutline(){ return  materialForDepthBasedOutline_; };
    MaterialForRadialBlur* GetMaterialForRadialBlur(){ return materialForRadialBlur_; };
    MaterialForDissolve* GetMaterialForDissolve(){ return materialForDissolve_; };
    MaterialForThermography* GetMaterialThermography() { return materialForThermography_; };
    MaterialForRandom* GetmaterialForRandom() { return materialForRandom_; };
private:
    Camera* camera_ = nullptr;   
    const Vector4 sepiaColor_ = { 1.0f,74.0f / 107.0f,43.0f / 107.0f,1.0f };

    MaterialForRenderTexture* materialForGrayScale_ = nullptr;
    MaterialForVignette* materialForVignette_ = nullptr;
    MaterialForBoxFilter* materialForBoxFilter_ = nullptr;
    MaterialForRenderTexture* materialForFullScreen_ = nullptr;
    MaterialForGaussianFilter* materialForGaussianFilter_ = nullptr;
    MaterialForLuminanceBasedOutline* materialForLuminanceBasedOutline_ = nullptr;
    MaterialForDepthBasedOutline* materialForDepthBasedOutline_ = nullptr;
    MaterialForRadialBlur* materialForRadialBlur_ = nullptr;
    MaterialForDissolve* materialForDissolve_ = nullptr;
    MaterialForThermography* materialForThermography_ = nullptr;
    MaterialForRandom* materialForRandom_ = nullptr;

    std::array<Microsoft::WRL::ComPtr <ID3D12Resource>, PSO::kCountOfEffect> materialResource_;
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

