#pragma once

#include <fstream>
//ComPtr(コムポインタ)
#include<wrl.h>
#include<d3d12.h>

//Textureデータを読み込むためにDirectXTex.hをインクルード
#include"DirectXTex.h"
//Textureの転送のために
#include"../externals/DirectXTex/d3dx12.h"

#include"CommandList.h"
#include<unordered_map>
#include"Application/Loader/TextureFactory.h"
#include<filesystem>

class Texture
{

public:
    static const std::vector<uint32_t>& GetMappedSRVIndexes() { return srvIndexes_; };
    static void LoadAndMapHandle(const std::filesystem::path& filePath, const TextureFactory::Handle& handle);
    static uint32_t AddTextureHandle(const std::filesystem::path& filePath);
    static void AddTextureHandleByIndex(const uint32_t& srvIndex);
    static uint32_t GetSRVHandle(const TextureFactory::Handle& handle) { return srvIndexes_[handle]; }
    static const std::filesystem::path& GetFilePath(const TextureFactory::Handle& handle) { return handleToPath_[srvIndexes_[handle]]; }
    static TextureFactory::Handle GetTextureHandle(const uint32_t& srvIndex);
    //SRVインデックスの開始番号
    static uint32_t kSRVIndexTop;
    struct TextureData {
        DirectX::TexMetadata metadata;
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
        Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
        uint32_t srvIndex = 0;
        D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU{};
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU{};
    };
public:
    static void Finalize();
    //初期化
    static void Initialize();
    //インデックスを返すロード関数
    static uint32_t LoadAndGetIndex(const std::filesystem::path& filePath);
    //SRVインデックスの開始番号
    static uint32_t GetSrvIndexByFilePath(const std::filesystem::path& filePath);

    //テクスチャ番号からGPUハンドルを取得
    static D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::filesystem::path& filePath);
    static const DirectX::TexMetadata& GetMetaData(const uint32_t& handle);
private:
    static std::unordered_map<std::filesystem::path, TextureData> textureDatas;
    static std::vector<uint32_t> srvIndexes_;
    static std::unordered_map<uint32_t,std::filesystem::path> handleToPath_;
private:
    //コンストラク・タデストラクタの隠ぺい
    Texture() = default;
    ~Texture() = default;
    /// @brief テクスチャファイルの読み込み
/// @param filePath テクスチャファイルのパス
    static void LoadTexture(const std::filesystem::path& filePath);


};

