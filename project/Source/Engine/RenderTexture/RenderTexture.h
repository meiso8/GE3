#pragma once
#include<wrl.h>
#include<d3d12.h>
#include"Vector4.h"
#include<stdint.h>
#include<PSO.h>
#include"hlslTypeToCpp.h"
#include"Camera.h"
#include"Texture.h"
#include"TransitionBarrier.h"

class RtvManager;

class RenderTexture

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

    enum RenderTextureType {
        kNormal0,
        kNormal1,
        kThermography,
        kObjectID,
        kMaxRenderTexutre,
    };
    struct RenderTextureData {
        Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
        uint32_t srvIndex = 0;
        D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU = {};
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU = {};
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandleCPU = {};
    };


private:
    Camera* camera_ = nullptr;
    Vector4 kRenderTargetClearValue_ = { 1.0f,1.0f,1.0f,1.0f };
    const Vector4 sepiaColor_ = { 1.0f,74.0f / 107.0f,43.0f / 107.0f,1.0f };
    std::array< RenderTextureData, kMaxRenderTexutre> renderTextureDatas_;
    RenderTextureData thermographyTextureData_;
    std::array<Microsoft::WRL::ComPtr <ID3D12Resource>, PSO::kCountOfEffect> materialResource_;
    //ID用リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> idReadbackResource_;


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
    Microsoft::WRL::ComPtr <ID3D12Resource>materialResourceRandom_;
    MaterialForRandom* materialForRandom_ = nullptr;
public:
    static RenderTexture* GetInstance() {
        static RenderTexture instance;
        return &instance;
    };

    Microsoft::WRL::ComPtr<ID3D12Resource>& GetMaterialResouce(const PSO::EffectType& effectType);
    void Create(RtvManager& rtvManager);
  

    // 1. 描画コマンドの最後にコピー命令を積む関数 (PostDrawの直前に呼ぶ)
    void CopyClickPixelCommand(int mouseX, int mouseY);

    // 2. 次のフレームのUpdateなどで、安全にIDを読み出す関数
    uint32_t GetClickedObjectID();

    const Vector4& GetColor() {
        return kRenderTargetClearValue_;
    }
    RenderTextureData& GetRenderTextureData(const RenderTextureType index) {
        return renderTextureDatas_[index];
    }
    std::array< RenderTextureData, kMaxRenderTexutre>& GetRenderTextureDatas() { return renderTextureDatas_; };
    void Draw(const PSO::EffectType& effectType, const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle, const uint32_t index);
    void DrawOutLine(const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle, const uint32_t index, const uint32_t depthSrvIndex);
    void DrawDissolve(const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle, const uint32_t index, const TextureFactory::Handle& textureHandle);
    void DrawRandom(const BlendMode& blendMode, const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle, const uint32_t index);
    // サーモグラフィー用
    void DrawThermo(const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle);
    void Update();
    void SetCamera(Camera* camera);
    MaterialForThermography* GetMaterialThermography() { return materialForThermography_; };
    MaterialForDissolve* GetMaterialDissolve() { return materialForDissolve_; };

    MaterialForGaussianFilter* GetMaterialGaussianFilter() { return materialForGaussianFilter_; };
    void Clear();
protected:
    /// @brief テクスチャハンドル
    uint32_t textureHandle_ = 0;
private:
    void CreateResource(const uint32_t index, RtvManager& rtvManager, DXGI_FORMAT format, bool createSRV);
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

