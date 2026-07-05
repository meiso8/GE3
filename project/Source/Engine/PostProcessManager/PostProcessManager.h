#pragma once
#include"PSO/PSO.h"
#include"RenderTexture/RenderTexture.h"
#include"TextureFactory.h"

class TransitionBarrier;
class PostEffectMaterial;
class Camera;

class PostProcessManager {
public:

    enum Layer {
        kModel,
        kSprite,
        kMaxLayer,
    };

    struct RenderLayer{
        RenderTexture* renderTexture_ = nullptr;
        std::unique_ptr<PostEffectMaterial> postEffectMaterial_ = nullptr;
        std::vector<PSO::EffectType> activeEffects_; // 現在有効なエフェクトのリスト
    };
public:
    static PostProcessManager* GetInstance() {
        static PostProcessManager instance;
        return &instance;
    };
    PostProcessManager(const PostProcessManager&) = delete;
    PostProcessManager& operator=(const PostProcessManager&) = delete;

    void Create(ID3D12GraphicsCommandList* commandList, SrvDescriptorHeap* srvDescriptorHeap);
    void Update();

    void SetPostEffectMaterialCamera(Camera* camera, const Layer& layer);

    void SetRenderTexture(RenderTexture* renderTexture, const Layer& layer);
    
    // 適用したいエフェクトを動的に追加する
    void AddEffect(PSO::EffectType type, const Layer& layer) {
        renderLayer_[layer].activeEffects_.push_back(type);
    }

    void ClearEffects(const Layer& layer) { renderLayer_[layer].activeEffects_.clear(); }

    /// @brief コマンド実行
    /// @param layer レイヤー
    /// @param dstRtvHandle RTVハンドル
    /// @param barrier バリア
    /// @param depthSrvIndex　SRVインデックス 
    /// @param randomBlendMode ランダム用ブレンドモード設定
    void Execute(const Layer& layer,
        const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle,
        TransitionBarrier* barrier,
        const uint32_t depthSrvIndex,
        const BlendMode& randomBlendMode);

    PostEffectMaterial* GetPostEffectMaterial(const Layer& layer) {
        return renderLayer_[layer].postEffectMaterial_.get();
    }
private:
    PostProcessManager() = default;
    ~PostProcessManager() = default;
    void Draw(const Layer& layer, const PSO::EffectType& effectType, const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle, const uint32_t index);
    void DrawOutLine(const Layer& layer, const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle, const uint32_t index, const uint32_t depthSrvIndex);
    void DrawDissolve(const Layer& layer, const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle, const uint32_t index, const TextureFactory::Handle& textureHandle);
    void DrawRandom(const Layer& layer, const BlendMode& blendMode, const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle, const uint32_t index);
    // サーモグラフィー用
    void DrawThermo(const Layer& layer, const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle);
    void DrawCallforRenderTexture(const Layer& layer, UINT rootParameterIndex, const uint32_t index);
private:

    std::array<RenderLayer, Layer::kMaxLayer> renderLayer_;
    static ID3D12GraphicsCommandList* commandList_;
    static SrvDescriptorHeap* srvDescriptorHeap_;

};