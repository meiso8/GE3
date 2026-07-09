#pragma once
#include<d3d12.h>
#include"hlslTypeToCpp.h"
#include<wrl.h>
#include"TransformationMatrix.h"  
#include"Camera.h"  
#include"Primitive.h"
#include"WorldTransform.h"
#include"Balloon.h"
#include"Wave.h"
#include"Transform.h"
#include<memory>
#include"ResourceManager/ResourceManager.h"
class Model;
class CbvSrvUavDescriptorHeap;

class Object3d
{
public:

    struct ObjectID {
        uint32_t id;
        uint32_t padding[3];
    };

    enum LightMode {
        kLightModeNone,
        kLightModeLReflectance,
        kLightModeHalfL,
    };

    struct Material
    {
        //色
        float4 color;
        //ライト
        int32_t lightMode;
        //輝度
        float32_t shininess;
        //環境反射度
        float32_t environmentCoefficient;
        //温度
        float32_t temperature;
        //UV
        float32_t4x4 uvTransform;
    };

protected:

    //コマンドリストの借り物
    static ID3D12GraphicsCommandList* commandList_;
    static CbvSrvUavDescriptorHeap* cbvSrvUavDescriptorHeap_;
    // ==============位置情報==================
    WorldTransform worldTransform_;
    CResource<TransformationMatrixFor3D>transformationMatrixResource_;

    // ==============表示非表示切り替え==================
    bool disabled_ = false;
    // ==============オブジェクト名==================
    std::string objectName_ = "Object";
    std::string objectType_ = "EMPTY";
    //移動先シーン名
    std::string nextStageName_ = "";

    // ==============マテリアル==================
    //マテリアルに関しては継承先で色々と変えるためどうするか迷う。
    IResource materialResource_;
    Material* material_ = nullptr;

    EulerTransform uvTransform_ = { 0.0f };
    Matrix4x4 uvTransformMatrix_{};
    //テクスチャハンドル
    std::array<int32_t, TEXTURE_USAGE_COUNT> textureHandles_;

    // ==============膨張データ==================
    CResource<Balloon>expansionResource_;
    // ==============波データ==================
    CResource<Wave>waveResource_;
    // ==============ID情報==================
    CResource<ObjectID>idResource_;
    // ==============メッシュ情報==================
    Primitive* primitive_ = nullptr;

private:

public:
    /// @brief コマンドリストのとsrvDescriptorHeapの借り物を入れる
    /// @param commandList コマンドリストクラス
    ///  @param srvDescriptorHeap srvDescriptorHeap
    static void SetCommandListAndSrvDescriptorHeap(
        ID3D12GraphicsCommandList* commandList,
        CbvSrvUavDescriptorHeap* srvDescriptorHeap
    );

    // ==============位置情報==================
    WorldTransform& GetWorldTransform() { return worldTransform_; };
    EulerTransform& GetTransform() { return worldTransform_.eTransform_; }
    void SetTransform(const EulerTransform& transform) { worldTransform_.eTransform_ = transform; }
    const Matrix4x4& GetWorldMatrix() { return worldTransform_.matWorld_; };
    void SetWorldMatrix(const Matrix4x4 mat) { worldTransform_.matWorld_ = mat; };
    const Vector3& GetScale()const { return worldTransform_.eTransform_.scale; };
    const Vector3& GetRotate()const { return worldTransform_.eTransform_.rotate; };
    const Vector3& GetTranslate() const { return worldTransform_.eTransform_.translate; };
    void SetScale(const Vector3& scale) { worldTransform_.eTransform_.scale = scale; };
    void SetRotate(const Vector3& rotate) { worldTransform_.eTransform_.rotate = rotate; };
    void SetTranslate(const Vector3& translate) { worldTransform_.eTransform_.translate = translate; };

    // ==============オブジェクト名==================
    const std::string& GetObjectName() { return objectName_; };
    void SetObjectName(const std::string& name) { objectName_ = name; }

    // ==============オブジェクトタイプ==================
    const std::string& GetObjectType() { return objectType_; };
    void SetObjectType(const std::string& name) { objectType_ = name; }
    // ==============次のステージをセットする==================
    const std::string& GetNextStageName() { return nextStageName_; };
    void SetNextStageName(const std::string& stageName) { nextStageName_ = stageName; }

    // ==============ID情報==================
    void SetObjectID(uint32_t id) { if (idResource_.data) { idResource_.data->id = id; } }
    uint32_t GetObjectID() const { if (idResource_.data) { return idResource_.data->id; }; return 0; }

    // ==============表示非表示切り替え==================
    void SetDisabled(const bool flag) { disabled_ = flag; };
    bool GetDisabled() { return disabled_; };

    // ==============膨張データ==================

    Balloon& GetBalloonData() { return *expansionResource_.data; }
    void InitBalloonData();

    // ==============波データ==================

    Wave& GetWaveData(size_t index) { return waveResource_.data[index]; };
    void InitWaveData();
    void InitWaveDataIndex(const uint32_t& index);

    // ==============マテリアルデータ==================
    Material& GetMaterial() { return *material_; };
    //色の取得
    virtual Vector4& GetColor() { return material_->color; };
    //色の設定
    virtual void SetColor(const Vector4& color) { material_->color = color; }
    //ライトの設定
    int32_t& GetLightMode() { return material_->lightMode; };
    void SetLightMode(const LightMode& lightMode) { material_->lightMode = lightMode; }
    //輝度
    void SetShininess(const float32_t& shininess) { material_->shininess = shininess; }
    //輝度
    float32_t& GetShininess() { return material_->shininess; }
    //環境反射度
    void SetEnvironmentCoefficient(const float& environmentCoefficient) { material_->environmentCoefficient = environmentCoefficient; }
    //環境反射度
    float32_t& GetEnvironmentCoefficient() { return material_->environmentCoefficient; }
    //温度の設定
    void SetTemperature(const float temp) { material_->temperature = temp; }
    //温度の設定
    float32_t GetTemperature() { return material_->temperature; }

    // ==============UVデータ==================

    Vector3& GetUVScale() { return uvTransform_.scale; };
    Vector3& GetUVRotate() { return uvTransform_.rotate; };
    Vector3& GetUVTranslate() { return uvTransform_.translate; };
    EulerTransform& GetUVTransform() { return uvTransform_; }
    void SetUV(const EulerTransform& transform) { uvTransform_ = transform; };
    void UpdateUV();

    // ==============テクスチャデータ==================

    //SRVIndexの取得
    uint32_t GetSrvIndex(const TEXTURE_USAGE& textureUsage) { return textureHandles_[textureUsage]; }
    /// @brief テクスチャハンドルの設定
    /// @param textureHandle テクスチャハンドル
    /// @param textureUsage 何用のテクスチャか　デフォルト値は拡散反射テクスチャ
    virtual void SetTextureHandle(
        const TextureFactory::Handle& textureHandle,
        const TEXTURE_USAGE& textureUsage = TEXTURE_USAGE_DIFFUSE
    ) {
        textureHandles_[textureUsage] = Texture::GetSRVHandle(textureHandle);
    };

    /// @brief テクスチャハンドルの取得関数
    /// @param textureUsage テクスチャ利用
    /// @return ファクトリーで設定したテクスチャハンドル
    TextureFactory::Handle GetTextureHandle(const TEXTURE_USAGE& textureUsage = TEXTURE_USAGE_DIFFUSE);
   
    // ==============メッシュ==================
    Primitive* GetPrimitive() { return primitive_; }

    // ==============重要==================
    Object3d() =default;
    ~Object3d() = default;
    /// @brief メッシュの情報を設定しその情報からマテリアルを設定する
    /// @param mesh メッシュ
    void SetMeshAndMaterial(Primitive* mesh);
    /// @brief オブジェクトの生成
    void Create();
    /// @brief 初期化
    virtual void Initialize();
    /// @brief オブジェクトの登録
    void RegisterObject();
    //  @brief オブジェクトの除外
    void UnRegisterObject();
    /// @brief 更新
    virtual void Update();
    /// @brief 描画関数
    /// @param camera カメラ
    /// @param blendMode ブレンドモード
    /// @param cullMode カリング
    /// @param maskMode マスク
    /// @param usePSOKey PSOが無かったら生成するかどうか
    /// @param skyBoxTexture 環境テクスチャの設定
    virtual void Draw(Camera& camera,
        const BlendMode& blendMode = kBlendModeNormal,
        const CullMode& cullMode = kCullModeBack,
        const MaskMode maskMode = kAll,
        const bool usePSOKey = false,
        const TextureFactory::Handle skyBoxTexture = TextureFactory::Handle::SKYBOX_TEX);
protected:
    /// @brief モデルの描画
    /// @param modelData モデルデータを入れる
    /// @param commandList コマンドリストの挿入
    void DrawModel(ModelData* modelData);

    void CreateTransformationMatrix();
    /// @brief メッシュデータの描画処理
    /// @param commandList コマンドリストの設定
    virtual void MeshDraw();
private:
    void CreateUV();
    void CreateMaterial(const float temperature = 0.0f, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f }, const uint32_t lightType = LightMode::kLightModeHalfL, const float shininess = 50.0f, const float  environmentCoefficient = 0.0f);
    void CreateWaveData();
    void CreateBalloonData();
    void CreateID();
};

