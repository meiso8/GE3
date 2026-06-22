#include "Skybox.h"
#include"DirectXCommon.h"
#include"MyEngine.h"
#include"AABB.h"
#include"MakeMatrix.h"

ID3D12GraphicsCommandList* SkyboxObject3d::commandList_ = nullptr;

SkyboxObject3d::~SkyboxObject3d()
{
    if (materialResource_) {
        materialResource_->Unmap(0, nullptr);
        materialResource_ = nullptr;
    }

    materialResource_.Reset();
}

void SkyboxObject3d::Create()
{

    commandList_ = DirectXCommon::GetCommandList();
    CreateTransformationMatrix();
    CreateMaterial();
    Initialize();
    Update();

    skyBox_ = std::make_unique<Primitive>();
    AABB aabb = { { -1.0f,-1.0f,-1.0f },{ 1.0f,1.0f,1.0f } };
    skyBox_->Create(PrimitiveGenerator::CreateSkyBox(aabb));
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

    if (skyBox_) {
        commandList_->SetGraphicsRootSignature(PSO::GetRootSignature()->GetRootSignature(RootSignature::SKYBOX));
        commandList_->SetPipelineState(PSO::GetGraphicsPipelineStateSkyBox().Get());//PSOを設定
        //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
        commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
        //wvp用のCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
        //拡散反射テクスチャ
        SrvManager::SetGraphicsRootDescriptorTable(2, textureHandle_);
        skyBox_->Draw(commandList_);
    }
}

void SkyboxObject3d::CreateTransformationMatrix()

{    //Matrix4x4　1つ分のサイズを用意
    transformationMatrixResource_ = DirectXCommon::CreateBufferResource(sizeof(TransformationMatrixFor3DSkybox));
    //データを書き込む
    //書き込むためのアドレスを取得
    transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
    transformationMatrixData_->WVP = MakeIdentity4x4();
}

void SkyboxObject3d::CreateMaterial(const Vector4& color)
{
    //マテリアル用のリソースを作る。
    materialResource_ = DirectXCommon::CreateBufferResource(sizeof(MaterialForSkyBox));
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&material_));
    material_->color = color;
}
