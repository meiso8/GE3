#define NOMINMAX
#include"FreeTypeManager.h"
#include<cstdint>
#include<fstream>
#include<cassert>
#include"DirectXCommon.h"
#include"SRVmanager/SrvManager.h"
#include"Mesh/Font/Font.h"
#include"Log.h"

using namespace std;

FT_Library FreeTypeManager::library_;

// フォントごとのFTData（faceとfontData）
std::unordered_map<uint32_t, FreeTypeManager::FTData> FreeTypeManager::fontFaces_;
unordered_map<GlyphKey, FreeTypeManager::FTTextureData> FreeTypeManager::glyphTextures_;
std::unordered_map<GlyphKey, std::vector<std::unique_ptr<Font>>> FreeTypeManager::fontPool_;

ID3D12GraphicsCommandList* FreeTypeManager::commandList_ = nullptr;

FreeTypeManager::FreeTypeManager()
{    
    //ライブラリの初期化　error　== 0 で成功 
    FT_Error error = FT_Init_FreeType(&library_);

    if (error == 0) {
        //初期化に成功
        LogFile::Log("FT_Init_FreeType Success!\n");
    } else {
        //失敗
        std::string msg = "FT_Init_FreeType failed!: " + std::to_string(error) + "\n";
        LogFile::Log(msg);
        assert(false);
    }

    LogFile::Log("Initialize FreeType Manager");
}

FreeTypeManager::~FreeTypeManager()
{

    LogFile::Log("Delete FreeType Manager");
}


uint32_t FreeTypeManager::CreateFace(const string& fontPath, const uint32_t index)
{
    //サイズからハンドルを取得
    uint32_t handle = static_cast<uint32_t>(fontFaces_.size());

    //すでにあったらreturn
    if (fontFaces_.contains(handle)) {
        return handle;
    }

    //fontDataの箱を用意
    FTData data;

    {
        // ファイルをバイナリで開く
        std::ifstream file(fontPath, std::ios::binary | std::ios::ate);
        if (!file) {
            std::string msg = "Cannot Open File: " + fontPath + "\n";
            LogFile::Log(msg);
            assert(false);
        }

        // ファイルサイズを取得してバッファ確保
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        data.fontData.resize(static_cast<size_t>(size));

        if (!file.read(reinterpret_cast<char*>(data.fontData.data()), size)) {
            LogFile::Log("CannotLoadFontData\n");
            assert(false);
        }

    }

    //Faceの作成
    FT_Error err = FT_New_Memory_Face(
        library_,
        data.fontData.data(),
        static_cast<FT_Long>(data.fontData.size()),
        index,
        &data.face);

    if (err == FT_Err_Unknown_File_Format)
    {
        LogFile::Log("FT_Err_Unknown_File_Format\n");
    } else if (err)
    {
        std::string msg = "FT_New_Memory_Face failed!: " + std::to_string(err) + "\n";
        LogFile::Log(msg);
    }

    assert(!err);

    //登録！
    fontFaces_.emplace(handle, std::move(data));

    //ハンドルを返す
    return handle;

}

void FreeTypeManager::SetCommandList(ID3D12GraphicsCommandList* commandList)

{    //コマンドリストをセットする
    commandList_ = commandList;
    assert(commandList_);
    
    LogFile::Log("SetCommandList to FreeType");

    Font::SetCommandList(commandList_);
}

void FreeTypeManager::Finalize()
{

    for (auto& data : fontFaces_) {
        //faceの破棄
        FT_Done_Face(data.second.face);
        data.second.fontData.clear();
    }

    //一旦明示的にglyphTextures_を解放しておく
    for (auto& glyphTexture : glyphTextures_) {
        ReleaseResource(glyphTexture.second.ftResource);
    }

    glyphTextures_.clear();

    //一旦明示的に解放しておく
    for (auto& [key, fonts] : fontPool_) {
        for (auto& font : fonts) {
            if (font != nullptr) {
                font.reset();
                font = nullptr;
            }
        }
        fonts.clear();
    }

    fontPool_.clear();

    //libraryの破棄
    FT_Done_FreeType(library_);

    LogFile::Log("Finalize FreeType Manager");
}

void FreeTypeManager::GetBitMapGlyph(uint32_t faceHandle, FT_UInt glyphIndex)
{

    auto& ftData = fontFaces_.at(faceHandle);

    if (!LoadAndRenderGlyph(ftData.face, glyphIndex, FT_RENDER_MODE_NORMAL))
    {
        LogFile::Log("LoadAndRenderGlyph faild\n");
        return;
    };

}

void FreeTypeManager::GetOutLineGlyph(uint32_t faceIndex, FT_UInt glyphIndex, uint32_t width, uint32_t height)
{

    auto& ftData = fontFaces_.at(faceIndex);
    FT_Face face = ftData.face;

    //生のアウトラインデータを取得する場合
    if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_NO_SCALE | FT_LOAD_NO_AUTOHINT | FT_LOAD_NO_BITMAP)) { LogFile::Log("Failed to Load_Glyph\n"); return; }

    if (!face) {
        std::string msg = "face[" + std::to_string(faceIndex) + "] is invalid\n";
        LogFile::Log(msg);
        assert(false);
    }

    auto& glyph = face->glyph;

    if (glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
        LogFile::Log("This Glyph is not OutLineGlyph type\n");
        assert(false);
    }

    //サイズにスケーリング・ヒンティングしたものを取得する場合
    if (FT_Set_Pixel_Sizes(face, width, height)) { LogFile::Log("Failed to Set_Pixel_Sizes\n");  assert(false); }

    //(オプション)glyph->outlineをラスタライズする
    if (FT_Render_Glyph(glyph, FT_RENDER_MODE_LCD)) { LogFile::Log("Glyph rasterization failed!\n");  assert(false); }
}

void FreeTypeManager::Draw(uint32_t faceIndex, FT_UInt glyphIndex)
{

    GlyphKey key{ faceIndex, glyphIndex };
    auto& ftData = fontFaces_.at(faceIndex);

    FT_Size_Metrics metrics = ftData.face->size->metrics;

    float height = metrics.height / 64.0f;
    float width = metrics.max_advance / 64.0f;

    Font::PreDraw();
    auto font = GetOrCreateFont(key);
    font->Draw();
}

bool FreeTypeManager::LoadAndRenderGlyph(FT_Face& face, FT_UInt glyphIndex, FT_Render_Mode mode)
{

    //グリフの読み込み
    if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT|FT_LOAD_COLOR)) {
        LogFile::Log("FT_Load_Glyph faild\n"); 
        assert(false);
        return false;
    }

    if (FT_Render_Glyph(face->glyph, mode)) {
        LogFile::Log("FT_Render_Glyph failed!\n");
        assert(false);
        return false;
    }

    auto glyph = face->glyph;
    if (glyph->format == FT_GLYPH_FORMAT_BITMAP &&
        glyph->bitmap.pixel_mode == FT_PIXEL_MODE_BGRA) {
        //カラー絵文字（PNG） face->glyph->bitmapにBGRAビットマップが入ってる
        LogFile::Log("This is color glyph\n");
    }

    return true;
}

void FreeTypeManager::ShowFontSize(uint32_t faceHandle)
{
    auto& ftData = fontFaces_.at(faceHandle);
    FT_Face face = ftData.face;

    for (int i = 0; i < face->num_fixed_sizes; ++i) {
        LogFile::Log("Size[" + to_string(i) + "]:" + to_string(face->available_sizes[i].width) + "x" + to_string(face->available_sizes[i].height) + "\n");
    }
}

void FreeTypeManager::ResetFontUsage()
{
    for (auto& [key, pool] : fontPool_) {
        for (auto& font : pool) {
            font->SetInUse(false);
        }
    }
}

const FreeTypeManager::FTTextureData& FreeTypeManager::GetGlyphTextures(const GlyphKey& key) {


    if (!glyphTextures_.contains(key)) {
        // なければ生成
        CreateGlyphTexture(key.handle, key.glyphIndex);
    }


    auto it = glyphTextures_.find(key);
    if (it == glyphTextures_.end()) {

        LogFile::Log("Glyph texture not found for key: " + std::to_string(key.handle) + ", " + std::to_string(key.glyphIndex) + "\n");
        assert(false); // or return a default value if you prefer
    }
    return it->second;
}

float FreeTypeManager::GetFontDescender(uint32_t handle)
{
    auto& ftData = fontFaces_.at(handle);
    FT_Face face = ftData.face;

    if (!face || !face->size) return 0.0f;

    // descenderは負の値なので符号を反転して正にする
    return -static_cast<float>(face->descender) * face->size->metrics.y_ppem / face->units_per_EM / 64.0f;
}


void FreeTypeManager::ReleaseResource(FTResource& resource)
{

    if (resource.resource != nullptr) {
        resource.resource.Reset();
        resource.resource = nullptr;
    }
}

FreeTypeManager::FTResource FreeTypeManager::CreateResourceFromFTBitmap(const FT_Bitmap& bitmap)
{
    using namespace Microsoft::WRL;

    FTResource result;

    int width = bitmap.width;
    int height = bitmap.rows;
    DXGI_FORMAT format = DXGI_FORMAT_R8_UNORM;

    auto device = DirectXCommon::GetDevice();

    // 1. デフォルトヒープにテクスチャ作成
    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.Format = format;
    texDesc.SampleDesc.Count = 1;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;


    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC texResourceDesc(texDesc);

    //テクスチャリソースの作成
    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &texResourceDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&result.resource)
    );

    if (FAILED(hr)) {
        LogFile::Log("Texture creation failed!\n");
    }

    // 2. アップロードヒープにデータをコピー
    UINT64 uploadBufferSize;
    device->GetCopyableFootprints(&texDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);

    CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC uploadDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

    //中間リソース用バッファの作成
    hr = device->CreateCommittedResource(
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &uploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&result.intermediateResource)
    );

    if (FAILED(hr)) {
        LogFile::Log("Create　IntermediateResource failed!\n");
    }


    // 3. 中間リソースにビットマップを書き込む
    D3D12_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.pData = bitmap.buffer;
    subResourceData.RowPitch = bitmap.pitch;
    subResourceData.SlicePitch =bitmap.pitch * bitmap.rows;

    if (bitmap.pixel_mode != FT_PIXEL_MODE_GRAY) {
        LogFile::Log("bitmap.pixel_mode is not GRAY!\n");
    }


    UpdateSubresources(commandList_, result.resource.Get(), result.intermediateResource.Get(), 0, 0, 1, &subResourceData);

    // 4. コピー先を PIXEL_SHADER_RESOURCE に遷移 テクスチャをシェーダーで使える状態に切り替える
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        result.resource.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
    );
    commandList_->ResourceBarrier(1, &barrier);

    return result;
}

void FreeTypeManager::CreateGlyphTexture(uint32_t faceHandle, FT_UInt glyphIndex)
{
    //faceHandleとglyphIndexからkeyを作成
    GlyphKey key{ faceHandle, glyphIndex };

    //glyphTextures_に含まれていたらreturn
    if (glyphTextures_.contains(key)) return;

    auto& ftData = fontFaces_.at(faceHandle);
    FT_Face& face = ftData.face;

    if (!LoadAndRenderGlyph(face, glyphIndex, FT_RENDER_MODE_NORMAL)) {
        assert(false);
        return;
    };

    //faceからBitMapを取得
    FT_Bitmap& bitmap = face->glyph->bitmap;
    if (!bitmap.buffer) {
        LogFile::Log("bitmap.buffer is null\n");
        assert(false);
    }

    FTTextureData texData;
    texData.ftResource = CreateResourceFromFTBitmap(bitmap);
  
    DirectX::TexMetadata metadata = {};
    metadata.miscFlags = metadata.IsCubemap();
    metadata.format = DXGI_FORMAT_R8_UNORM;
    metadata.mipLevels = 1;

    texData.srvIndex = SrvManager::Allocate();
    Texture::AddTextureHandleByIndex(texData.srvIndex);
    texData.srvHandleCPU = SrvManager::GetCPUDescriptorHandle(texData.srvIndex);
    texData.srvHandleGPU = SrvManager::GetGPUDescriptorHandle(texData.srvIndex);
    SrvManager::CreateSRVforTexture(texData.srvIndex, texData.ftResource.resource.Get(), metadata);
    texData.glyphSize = { (float)bitmap.width, (float)bitmap.rows };
    texData.bearingY = face->glyph->metrics.horiBearingY / 64.0f;

    glyphTextures_[key] = std::move(texData);
}

Font* FreeTypeManager::CreateFontSprite(uint32_t faceHandle, FT_UInt glyphIndex)
{
    GlyphKey key = { faceHandle ,glyphIndex };

    if (!glyphTextures_.contains(key)) {
        // なければ生成
        CreateGlyphTexture(key.handle, key.glyphIndex);
    }

    auto& data = glyphTextures_.at(key);

    auto& srvIndex = data.srvIndex;
    auto& ftData = fontFaces_.at(key.handle);
    FT_Face face = ftData.face;

    if (data.ftResource.resource) {
        auto& pool = fontPool_[key];
        auto& glyphSize = data.glyphSize;
        auto newFont = std::make_unique<Font>();
        newFont->Create(Texture::GetTextureHandle(srvIndex), { 100.0f, 100.0f },{1.0f,1.0f,1.0f,1.0f}, glyphSize);
        newFont->SetInUse(true);
        Font* ptr = newFont.get();
        pool.push_back(std::move(newFont));
        return ptr;

    }
    assert(false);

    return nullptr;
}

void FreeTypeManager::SetPixelSizes(uint32_t faceHandle, uint32_t width, uint32_t height)
{
    auto& face = fontFaces_.at(faceHandle).face;

    if (!face) { LogFile::Log("face is null!\n"); return; }

    FT_Error err = FT_Set_Pixel_Sizes(face, width, height);
    if (err) {
        LogFile::Log("Failed to Set_Pixel_Sizes: error code = " + std::to_string(err) + "\n");
        return;
    }
}

FT_UInt FreeTypeManager::GetGlyphID(uint32_t faceHandle, uint32_t unicode, uint32_t uvs)
{

    auto& ftData = fontFaces_.at(faceHandle);
    auto& face = ftData.face;

    //Unicodeのコードポイントから
    FT_UInt result = FT_Face_GetCharVariantIndex(face, unicode, uvs);

    if (result == 0) {
        //0が帰ってきたらフォールバックする

        FT_UInt glyphIndex = FT_Get_Char_Index(face, unicode);

        if (glyphIndex == 0) {
            LogFile::Log("Glyph not found: U+" + std::to_string(unicode) + "\n");

            // フォールバック文字（例: '?'）
            glyphIndex = FT_Get_Char_Index(face, '?');

            if (glyphIndex == 0) {
                LogFile::Log("Fallback glyph '?' not found either!\n");
            }

        }

        return glyphIndex;

    } else {
        return result;
    }

}

void FreeTypeManager::GetBitMap(uint32_t faceHandle, FT_UInt glyphIndex, FT_Int strikeIndex)
{
    auto& face = fontFaces_.at(faceHandle).face;

    //ビットマップグリフを取得する 
    if (FT_Select_Size(face, strikeIndex)) { assert(false); }
    //グリフを読み込む
    if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT)) { assert(false); }

    //face->glyph->format == FT_GLYPH_FORMAT_BITMAPなら埋め込みビットマップ 
    // face->glyph->bitmapに（多くの場合モノクロの）ビットマップが入ってる

}

std::vector<GlyphRun> FreeTypeManager::LayoutString(uint32_t handle, const std::u32string& text, const Vector2& startPos)
{
    std::vector<GlyphRun> runs;

    auto& ftData = fontFaces_.at(handle);
    FT_Face& face = ftData.face;

    float maxDescender = GetFontDescender(handle);

    float penX = startPos.x;
    float penY = startPos.y + maxDescender;
    FT_UInt prevGlyph = 0;

    for (char32_t ch : text) {


        if (ch == U'\n' || ch == U'\r') {
            penX = startPos.x;
            penY += face->size->metrics.height / 64.0f;
            prevGlyph = 0;
            continue;
        }

        if (ch == U'　') {
            //日本語入力の時
            penX += face->glyph->advance.x / 32.0f;
            continue;
        }

        if (ch == U'\t' || ch == U' ') {
            penX += face->glyph->advance.x / 64.0f;
            continue;
        }


        FT_UInt glyphIndex = GetGlyphID(handle, ch, 0);
        if (glyphIndex == 0) continue;

        if (prevGlyph && glyphIndex) {
            FT_Vector kerning;
            if (FT_Get_Kerning(face, prevGlyph, glyphIndex, FT_KERNING_DEFAULT, &kerning) == 0) {
                penX += kerning.x / 64.0f;
            }
        }

        if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT) != 0) continue;

        runs.push_back({ glyphIndex, { penX, penY } });

        penX += face->glyph->advance.x / 64.0f;
        prevGlyph = glyphIndex;
    }

    return runs;
}

Font* FreeTypeManager::GetOrCreateFont(const GlyphKey& key)
{
    auto& pool = fontPool_[key];
    if (!glyphTextures_.contains(key)) {
        // なければ生成
        CreateGlyphTexture(key.handle, key.glyphIndex);
    }
    auto& texData = glyphTextures_.at(key);

    // 未使用のスプライトを探す
    for (auto& font : pool) {
        if (!font->IsInUse()) {
            font->SetInUse(true);
            font->SetSize(texData.glyphSize);
            return font.get();
        }
    }
    //無かったら作成する
    return CreateFontSprite(key.handle, key.glyphIndex);
}
