#include "Object3d.h"
#include"DirectXCommon.h"
#include"MakeMatrix.h"
#include"Lights/PointLightManager.h"
#include"Lights/DirectionalLightManager.h"
#include"Lights/SpotLightManager.h"
#include"Model.h"
#include"SrvDescriptorHeap.h"
#include"ObjectManager/ObjectManager.h"
#include"Log.h"

ID3D12GraphicsCommandList* Object3d::commandList_ = nullptr;
CbvSrvUavDescriptorHeap* Object3d::cbvSrvUavDescriptorHeap_ = nullptr;

void Object3d::CreateUV()
{
    uvTransform_ = {
      {1.0f,1.0f,1.0f},
      {0.0f,0.0f,0.0f},
      {0.0f,0.0f,0.0f},
    };

    uvTransformMatrix_ = MakeIdentity4x4();

};

void Object3d::UpdateUV() {

    uvTransformMatrix_ = MakeAffineMatrix(uvTransform_.scale, uvTransform_.rotate, uvTransform_.translate);
    material_->uvTransform = uvTransformMatrix_;
}


//Object3d::~Object3d()
//{
//    Finalize();
//}
//
//void Object3d::Finalize()
//{
//    transformationMatrixResource_.UnMap();
//    transformationMatrixResource_.Reset();
//
//    materialResource_.UnMap();
//    materialResource_.Reset();
//
//    expansionResource_.UnMap();
//    expansionResource_.Reset();
//
//    waveResource_.UnMap();
//    waveResource_.Reset();
//
//    idResource_.UnMap();
//    idResource_.Reset();
//}

void Object3d::SetCommandListAndSrvDescriptorHeap(ID3D12GraphicsCommandList* commandList, CbvSrvUavDescriptorHeap* srvDescriptorHeap)
{
    commandList_ = commandList;
    assert(commandList_);

    cbvSrvUavDescriptorHeap_ = srvDescriptorHeap;
    assert(cbvSrvUavDescriptorHeap_);
}

void Object3d::InitBalloonData()
{
    //データを書き込む
    if (expansionResource_.data) {
        expansionResource_.data->expansion = 0.0f;
        expansionResource_.data->sphere = 0.0f;
        expansionResource_.data->cube = 0.0f;
        expansionResource_.data->isSphere = false;
    }
}

void Object3d::InitWaveData()
{
    InitWaveDataIndex(0);
    InitWaveDataIndex(1);
}

void Object3d::InitWaveDataIndex(const uint32_t& index)
{
    if (index > 1) {
        return;
    }

    if (waveResource_.data) {
      waveResource_.data[index].direction = { 1.0f,0.0f,0.0f };
      waveResource_.data[index].time = 0.0f;
      waveResource_.data[index].amplitude = 0.0f;
      waveResource_.data[index].frequency = 4;
    }

}

void Object3d::Draw(Camera& camera, const BlendMode& blendMode, const CullMode& cullMode, const MaskMode maskMode, const bool usePSOKey, const TextureFactory::Handle skyBoxTexture)
{
    //データを書き込む

    transformationMatrixResource_.data->World = worldTransform_.matWorld_;
    transformationMatrixResource_.data->WorldInverseTranspose = Transpose(Inverse(worldTransform_.matWorld_));
    transformationMatrixResource_.data->WVP = Multiply(worldTransform_.matWorld_, camera.GetViewProjectionMatrix());

    if (primitive_) {

        primitive_->SetRootSignatureAndGraphicsPipeline(commandList_, blendMode, cullMode, maskMode, usePSOKey);
        //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
        commandList_->SetGraphicsRootConstantBufferView(0, materialResource_.resource->GetGPUVirtualAddress());
        //wvp用のCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_.GetGPUVirtualAddress());
        //SrvDescriptorHeapクラスでテクスチャを設定する
        cbvSrvUavDescriptorHeap_->SetGraphicsRootDescriptorTable(2, textureHandles_[TEXTURE_USAGE_DIFFUSE], commandList_);
        //cameraのCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(3, camera.GetResource()->GetGPUVirtualAddress());
        //ID
        commandList_->SetGraphicsRootConstantBufferView(4, idResource_.GetGPUVirtualAddress());
        //ライト
        DirectionalLightManager::SetGraphicsRootConstantBufferView(5, commandList_);
        //expansionのCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(6, expansionResource_.GetGPUVirtualAddress());
        //timeのSRVの場所を設定
        commandList_->SetGraphicsRootShaderResourceView(7, waveResource_.GetGPUVirtualAddress());
        //ライトのCBufferの場所を設定
        cbvSrvUavDescriptorHeap_->SetGraphicsRootDescriptorTable(8, PointLightManager::GetSrvIndex(), commandList_);
        //SpotLightのDescriptorTableの設定をする
        cbvSrvUavDescriptorHeap_->SetGraphicsRootDescriptorTable(9, SpotLightManager::GetSrvIndex(), commandList_);

        cbvSrvUavDescriptorHeap_->SetGraphicsRootDescriptorTable(10, Texture::GetSRVHandle(skyBoxTexture), commandList_);

        MeshDraw();

    }
}

void Object3d::MeshDraw()

{
    if (!primitive_) {
        return;
    }

    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    commandList_->IASetPrimitiveTopology(primitive_->GetTopology());
    commandList_->IASetVertexBuffers(0, 1, &primitive_->GetVertexBufferView());

    if (auto model = dynamic_cast<Model*>(primitive_)) {
        //モデルデータの取得
        auto* modelData = model->GetModelData();
        DrawModel(modelData);

    } else {

        if (primitive_->GetIndexCount() > 0) {
            commandList_->IASetIndexBuffer(&primitive_->GetIndexBufferView());
            commandList_->DrawIndexedInstanced(primitive_->GetIndexCount(), 1, 0, 0, 0);

        } else {
            // ラインなどインデックスが無い場合
            commandList_->DrawInstanced(primitive_->GetVertexCount(), 1, 0, 0);
        }

    }
}

void Object3d::DrawModel(ModelData* modelData)
{

    commandList_->IASetIndexBuffer(&primitive_->GetIndexBufferView());

    // 2. マテリアルごとにループを回して描画
    for (const auto& section : modelData->sections) {

        // セクション名に対応するマテリアルデータを取得
        const auto& material = modelData->materials.at(section.materialName);

        // 1. このセクション用のディフューズテクスチャのSRVインデックスを取得
        int32_t srvIndex = material.textureData_[TEXTURE_USAGE_DIFFUSE].textureSrvIndex;

        if (srvIndex == -1) {
            // テクスチャが無い場合は白1x1テクスチャ
            srvIndex = Texture::GetSRVHandle(TextureFactory::WHITE_1X1);
        }

        //拡散反射テクスチャ
        cbvSrvUavDescriptorHeap_->SetGraphicsRootDescriptorTable(2, srvIndex, commandList_);
        // 3. インデックスの開始位置と個数を指定して描画コールを呼ぶ
        commandList_->DrawIndexedInstanced(
            section.indexCount,  // 描画するインデックス数
            1,
            section.indexStart,  // ★開始インデックス位置
            0,
            0
        );
    }
}

TextureFactory::Handle Object3d::GetTextureHandle(const TEXTURE_USAGE& textureUsage)
{
    //テクスチャハンドルを取得する
    return Texture::GetTextureHandle(textureHandles_[textureUsage]);
}

void Object3d::SetMeshAndMaterial(Primitive* mesh)
{
    //メッシュをセットする
    primitive_ = mesh;
    //メッシュが無かったらアサート
    assert(primitive_);

    if (auto model = dynamic_cast<Model*>(primitive_)) {
        //一旦マテリアル0

        //マテリアルリソースが無かったらアサート
        assert(materialResource_.Get());

        for (auto& [name, material] : model->GetModelData()->materials) {
            for (int i = 0; i < material.textureData_.size(); ++i) {
                textureHandles_[i] = material.textureData_[i].textureSrvIndex;
            }

           material_->shininess = material.shininess;

            if (textureHandles_[TEXTURE_USAGE_DIFFUSE] == -1) {
                textureHandles_[TEXTURE_USAGE_DIFFUSE] = Texture::GetSRVHandle(TextureFactory::WHITE_1X1);
            }

        }

    }

}

void Object3d::Create()
{
    CreateTransformationMatrix();
    CreateMaterial();
    CreateUV();
    CreateWaveData();
    CreateBalloonData();
    CreateID();
    //生成と同時に初期化する
    Initialize();
}

void Object3d::Initialize()
{
    worldTransform_.Initialize();
}


void Object3d::RegisterObject()
{
    ObjectManager::GetInstance()->RegisterObject(this);
}

void Object3d::UnRegisterObject()
{
    ObjectManager::GetInstance()->UnregisterObject(this);
}

void Object3d::Update()
{
    WorldTransformUpdate(worldTransform_);
}

//===============================================//Resourceの作成//===================================================================

void Object3d::CreateTransformationMatrix() {

    //Matrix4x4　1つ分のサイズを用意
    transformationMatrixResource_.CreateBufferResource(L"Object3d_Transformation_Matrix_Resource");

    //データを書き込む
    //書き込むためのアドレスを取得
    transformationMatrixResource_.Map();

    transformationMatrixResource_.data->WVP = MakeIdentity4x4();
    transformationMatrixResource_.data->World = MakeIdentity4x4();
    transformationMatrixResource_.data->WorldInverseTranspose = MakeIdentity4x4();

}

void Object3d::CreateWaveData()
{
    int waveCount = 2;
    size_t bufferSize = (sizeof(Wave) * waveCount + 255) & ~255;
    waveResource_.CreateBufferResource(L"Object3d_WaveDataResoource",bufferSize);

    //書き込むためのアドレスを取得
    waveResource_.Map();

    InitWaveData();
}

void Object3d::CreateBalloonData()
{
    expansionResource_.CreateBufferResource(L"Object3d_Expansion_Resource");
    //書き込むためのアドレスを取得
    expansionResource_.Map();

    //データを初期化する
    InitBalloonData();

}

void Object3d::CreateID()
{

    idResource_.CreateBufferResource(L"Object3d_Id_Resource");
    //書き込むためのアドレスを取得
    idResource_.Map();

    idResource_.data->id = 0;
}

void Object3d::CreateMaterial(
    const float temperature,
    const Vector4& color,
    const uint32_t lightType,
    const float shininess,
    const float  environmentCoefficient
) {

    for (auto& textureHandle : textureHandles_) {
        textureHandle = -1;
    }

    //マテリアル用のリソースを作る。
    materialResource_.resource = ResourceFactory::CreateBufferResource(sizeof(Material));
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_.resource->Map(0, nullptr, reinterpret_cast<void**>(&material_));
    material_->color = color;
    material_->lightMode = lightType;
    material_->uvTransform = MakeIdentity4x4();
    material_->shininess = shininess;
    material_->environmentCoefficient = environmentCoefficient;
    //体温
    material_->temperature = temperature;

    //白色にしておく
    textureHandles_[TEXTURE_USAGE_DIFFUSE] = Texture::GetSRVHandle(TextureFactory::WHITE_1X1);
}