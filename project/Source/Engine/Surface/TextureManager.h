#pragma once

#include <fstream>
//ComPtr(コムポインタ)
#include<wrl.h>
#include<d3d12.h>

//Textureデータを読み込むためにDirectXTex.hをインクルード
#include"../externals/DirectXTex/DirectXTex.h"
//Textureの転送のために
#include"../externals/DirectXTex/d3dx12.h"

#include"CommandList.h"
#include"ShaderResourceView.h"
class TextureManager
{

public:
    //SRVインデックスの開始番号
    static uint32_t kSRVIndexTop;
public:
    //シングルトンインスタンスの取得
    static TextureManager* GetInstance();
    //終了
    void Finalize();
    //初期化
    void Initialize();
    /// @brief テクスチャファイルの読み込み
    /// @param filePath テクスチャファイルのパス
    void LoadTexture(const std::string& filePath);
    //SRVインデックスの開始番号
    uint32_t GetTextureIndexByFilePath(const std::string& filePath);
private:
    static TextureManager* instance;

    //コンストラク・タデストラクタの隠ぺい
    TextureManager() = default;
    ~TextureManager() = default;
    //コピーコンストラクタの封印
    TextureManager(TextureManager&) = delete;
    //コピー代入演算子の封印
    TextureManager& operator=(TextureManager&) = delete;

    struct TextureData {
        std::string filePath;
        DirectX::TexMetadata metadata;
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
        D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
    };

    std::vector<TextureData> textureDatas;


};

