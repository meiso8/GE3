#include "SkyboxObject3d.h"
#include"DirectXCommon.h"
#include"AABB.h"
#include"SrvManager/SrvManager.h"

SkyboxObject3d::~SkyboxObject3d()
{
    Object3d::Finalize();
}

void SkyboxObject3d::Create()
{

    CreateTransformationMatrix();
    CreateMaterial();
    Initialize();
    Update();

    skyBox_ = std::make_unique<Primitive>();
    AABB aabb = { { -1.0f,-1.0f,-1.0f },{ 1.0f,1.0f,1.0f } };
    skyBox_->Create(PrimitiveGenerator::CreateSkyBox(aabb));
    SetMeshAndMaterial(skyBox_.get());

    //スカイボックステクスチャをセットする
    SetTextureHandle(TextureFactory::SKYBOX_TEX);
}


void SkyboxObject3d::Initialize()
{
    worldTransform_.Initialize();
    worldTransform_.eTransform_.scale = { 20.0f,20.0f,20.0f };
}

void SkyboxObject3d::Update()
{
    WorldTransformUpdate(worldTransform_);
}

void SkyboxObject3d::Draw(Camera& camera)
{
    //データを書き込む
    Matrix4x4 viewMat = camera.GetViewMatrix();
    //平行移動成分を除去する
    viewMat.m[3][0] = 0.0f;
    viewMat.m[3][1] = 0.0f;
    viewMat.m[3][2] = 0.0f;
    viewMat = Multiply(viewMat, camera.GetProjectionMatrix());
    transformationMatrixData_->WVP = Multiply(worldTransform_.matWorld_, viewMat);

    //ここはどこからでもアクセス可能なため　後で変更する
    auto* commandList = DirectXCommon::GetCommandList();

    if (skyBox_) {
        //RootSignatureの設定
        commandList->SetGraphicsRootSignature(PSO::GetRootSignature()->GetRootSignature(RootSignature::SKYBOX));
        //PSOを設定
        commandList->SetPipelineState(PSO::GetGraphicsPipelineStateSkyBox().Get());
        //マテリアルCBufferの場所を設定
        commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
        //wvp用のCBufferの場所を設定
        commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
        //拡散反射テクスチャ 
        SrvManager::SetGraphicsRootDescriptorTable(2, textureHandles_[TEXTURE_USAGE_DIFFUSE]);
        //メッシュの描画
        MeshDraw(commandList);
    }
}

void SkyboxObject3d::CreateMaterial(const Vector4& color)
{

    //マテリアル用のリソースを作る。
    materialResource_ = DirectXCommon::CreateBufferResource(sizeof(MaterialForSkyBox));
    //書き込むためのアドレスを取得
    HRESULT result = materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialForSkyBox_));
    assert(SUCCEEDED(result));
    //マテリアルにデータを書き込む
    materialForSkyBox_->color = color;
}
