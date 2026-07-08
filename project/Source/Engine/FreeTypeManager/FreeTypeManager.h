#pragma once
//freeTypeを使用するためのインクルードヘッダ
#include <ft2build.h>
//FT_FREETYPE_H はマクロです。
#include FT_FREETYPE_H

//DirectX
#include<wrl.h>
#include<d3d12.h>

#include<vector>
#include<string>
#include<memory>
#include<unordered_map>
#include"Vector2.h"

class Font;
class CommandList;
class CbvSrvUavDescriptorHeap;

struct GlyphKey {
    //フォントの種類のハンドル
    uint32_t handle;
    //文字のインデックス
    FT_UInt glyphIndex;

    bool operator==(const GlyphKey& other) const {
        return handle == other.handle && glyphIndex == other.glyphIndex;
    }
};

//Hash値の設定？
namespace std {
    template <>
    struct hash<GlyphKey> {
        size_t operator()(const GlyphKey& k) const {
            return hash<uint64_t>()((uint64_t(k.handle) << 32) | k.glyphIndex);
        }
    };
}

struct GlyphRun {
    //文字の添え字
    FT_UInt glyphIndex;
    //位置
    Vector2 position;
};



class FreeTypeManager {
public:
    

    //FreeTypeのデータ
    struct FTData {
        //face フォントの種類を格納する
        FT_Face face;
        //フォントデータ
        std::vector<uint8_t> fontData;
    };

    struct FTResource {
        //リソース
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
        Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
    };

    //FreeType用テクスチャデータ
    struct FTTextureData {
        FTResource ftResource;
        uint32_t srvIndex;
        D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
        //文字のサイズ
        Vector2 glyphSize;
        float bearingY = 0.0f;
    };

private:
    //コマンドリストの借り物を格納する
    static ID3D12GraphicsCommandList* commandList_;
    //一つのライブラリで複数のFaceを保持できる
    static FT_Library library_;
    // フォント（←注意）ごとのFTData（faceとfontData）
    static std::unordered_map<uint32_t, FTData> fontFaces_;
    // 文字ごとのテクスチャを格納する
    static std::unordered_map<GlyphKey, FTTextureData> glyphTextures_;
    //文字ごとのFontを格納する
    static std::unordered_map<GlyphKey, std::vector<std::unique_ptr<Font>>> fontPool_;
    static CbvSrvUavDescriptorHeap* srvDescriptorHeap_;
public:
    /// @brief ライブラリの初期化
    FreeTypeManager();
    ~FreeTypeManager();
    // 2. コピーと代入を禁止する
    FreeTypeManager(const FreeTypeManager&) = delete;
    FreeTypeManager& operator=(const FreeTypeManager&) = delete;

     void SetCommandListAndSrvDescriptorHeap(ID3D12GraphicsCommandList* commandList, CbvSrvUavDescriptorHeap* srvDescriptorHeap);
    /// @brief 終了処理
     void Finalize();
     /// @brief エンジンでCommandQueueを送った後にリセットする
     void ResetFontUsage();

    /// @brief Faceの生成
    /// @param fontPath .ttfや.tccを読み込む
    /// @param faceIndex .ttf の場合0でOK　.tccの場合は複数存在する
    static uint32_t CreateFace(const std::string& fontPath, const uint32_t faceIndex);
    /// @brief 文字のIDの取得
    /// @param faceHandle フォントの種類
    /// @param unicode Unicodeで入力する
    /// @param uvs 基本的に0でいい
    /// @return ID
    static FT_UInt GetGlyphID(uint32_t faceHandle, uint32_t unicode, uint32_t uvs);
    /// @brief ビットマップの取得
    /// @param faceHandle faceのハンドル
    /// @param glyphIndex 文字のindex
    /// @param strikeIndex 
    static void GetBitMap(uint32_t faceHandle, FT_UInt glyphIndex, FT_Int strikeIndex);
    /// @brief BitMapGlyphの作成
    /// @param faceHandle faceのハンドル
    /// @param glyphIndex 文字のindex
    static void GetBitMapGlyph(uint32_t faceHandle, FT_UInt glyphIndex);
    /// @brief アウトライン付き文字の取得
    /// @param faceHandle faceのハンドル
    /// @param glyphIndex 文字のindex
    /// @param width 文字の幅
    /// @param height 文字の高さ
    static void GetOutLineGlyph(uint32_t faceHandle, FT_UInt glyphIndex, uint32_t width, uint32_t height);
    /// @brief 文字一つを指定して描画
    /// @param faceHandle faceのハンドル
    /// @param glyphIndex  文字のindex
    static void Draw(uint32_t faceHandle, FT_UInt glyphIndex);
    /// @brief 読み込みとレンダー
    /// @param face face
    /// @param glyphIndex   文字のindex
    /// @param mode FT_Render_Modeの設定
    /// @return 成功したかどうか
    static bool LoadAndRenderGlyph(FT_Face& face, FT_UInt glyphIndex, FT_Render_Mode mode);
    /// @brief Fontの作成
    /// @param faceHandle faceのハンドル
    /// @param glyphIndex 文字のindex
    /// @return Font*
    static Font* CreateFontSprite(uint32_t faceHandle, FT_UInt glyphIndex);
    /// @brief PixelSizeの設定
    /// @param faceHandle  faceのハンドル
    /// @param width 文字の幅
    /// @param height 文字の高さ
    static void SetPixelSizes(uint32_t faceHandle, uint32_t width, uint32_t height);
    /// @brief FontSizeを見せる
    /// @param faceHandle  faceのハンドル
    static void ShowFontSize(uint32_t faceHandle);
    /// @brief Stringのレイアウトを作成する
    /// @param faceHandle  faceのハンドル
    /// @param text text
    /// @param startPos 始点
    /// @return 文字の情報のvectorを取得する
    static std::vector<GlyphRun> LayoutString(uint32_t faceHandle, const std::u32string& text, const Vector2& startPos);
    /// @brief Spriteが作成済みだったら取得し、無かったら作成する
    /// @param key　keyを入れる 
    /// @return Font*
    static Font* GetOrCreateFont(const GlyphKey& key);

    /// @brief FTTextureDataの取得関数
    /// @param key keyを入れる
    /// @return FTTextureData
    static const FTTextureData& GetGlyphTextures(const GlyphKey& key);
    static float GetFontDescender(uint32_t handle);
private:
    /// @brief FreeTypeのResource生成
    /// @param bitmap bitmapを入れる
    /// @return FTResource resource intermediateResource
    static FTResource CreateResourceFromFTBitmap(const FT_Bitmap& bitmap);
    /// @brief 文字のテクスチャ生成
    /// @param handle 
    /// @param glyphIndex 
    static void CreateGlyphTexture(uint32_t handle, FT_UInt glyphIndex);
    /// @brief リソールリリース
    /// @param resource 
    static void ReleaseResource(FTResource& resource);

};
