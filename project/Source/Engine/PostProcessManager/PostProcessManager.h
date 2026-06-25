#pragma once
#include"PSO/PSO.h"
#include"RenderTexture/RenderTexture.h"
#include"TransitionBarrier.h"
class CommandList;

class PostProcessManager {
public:
    
    // 適用したいエフェクトを動的に追加する
    void AddEffect(PSO::EffectType type) {
        activeEffects_.push_back(type);
    }

    void ClearEffects() { activeEffects_.clear(); }

    /// @brief コマンド実行
    /// @param renderTexture　レンダーテクスチャ 
    /// @param dstRtvHandle RTVハンドル
    /// @param barrier バリア
    /// @param depthSrvIndex　SRVインデックス 
    /// @param randomBlendMode ランダム用ブレンドモード設定
    void Execute(RenderTexture* renderTexture,
        const D3D12_CPU_DESCRIPTOR_HANDLE dstRtvHandle,
        TransitionBarrier* barrier,
        const uint32_t depthSrvIndex,
        const BlendMode& randomBlendMode);
private:
    std::vector<PSO::EffectType> activeEffects_; // 現在有効なエフェクトのリスト
};