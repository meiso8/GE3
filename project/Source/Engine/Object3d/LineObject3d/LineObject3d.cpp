#include "LineObject3d.h"
#include"MakeMatrix.h"
#include"DirectXCommon.h"

void LineObject3d::SetVertex(const Vector3& start, const Vector3& end)
{
    line_->SetVertex(start, end);
}


void LineObject3d::Create(const Vector3& startPos,const Vector3& endPos)
{

    line_ = std::make_unique<LineMesh>();
    line_->Create(startPos, endPos);

    CreateTransformationMatrix();
    CreateMaterial({ 1.0f,1.0f,1.0f,1.0f });
    Initialize();
}

void LineObject3d::Initialize()
{
    worldTransform_.Initialize();
}

void LineObject3d::Update()
{
    WorldTransformUpdate(worldTransform_);
}

void LineObject3d::Draw(Camera& camera,
    const bool useWorldMatrix,
    const BlendMode& blendMode,
    const CullMode& cullMode,
    const MaskMode maskMode, 
    const bool usePSOKey,
    const RootSignature::TYPE rootSignatureType,
    const DxcCompiler::VS_TYPE vsType,
    const DxcCompiler::PS_TYPE psType)
{
    //データを書き込む

    if (useWorldMatrix) {
        transformationMatrixData_->World = worldTransform_.matWorld_;
    } else {
        transformationMatrixData_->World = MakeIdentity4x4();
    }
    transformationMatrixData_->WorldInverseTranspose = Transpose(Inverse(worldTransform_.matWorld_));
    transformationMatrixData_->WVP = Multiply(worldTransform_.matWorld_, camera.GetViewProjectionMatrix());
    auto* commandlist = DirectXCommon::GetCommandList();

    line_->SetRootSignatureAndGraphicsPipeline(commandlist,blendMode,cullMode,maskMode,usePSOKey,rootSignatureType,vsType,psType);

    //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
    commandlist->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    //wvp用のCBufferの場所を設定
    commandlist->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());

#pragma region MeshDraw
    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    commandlist->IASetPrimitiveTopology(line_->GetTopology());
    commandlist->IASetVertexBuffers(0, 1, &line_->GetVertexBufferView());
    //ラインはインデックス関係なく2点のみのためこれ
    commandlist->DrawInstanced(line_->GetVertexCount(), 1, 0, 0);
#pragma endregion
    
}

void LineObject3d::CreateTransformationMatrix()
{//Matrix4x4　1つ分のサイズを用意
    transformationMatrixResource_ = DirectXCommon::CreateBufferResource(sizeof(TransformationMatrixFor3D));
    //データを書き込む
    //書き込むためのアドレスを取得
    transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
    transformationMatrixData_->WVP = MakeIdentity4x4();
    transformationMatrixData_->World = MakeIdentity4x4();
    transformationMatrixData_->WorldInverseTranspose = MakeIdentity4x4();
}
void LineObject3d::CreateMaterial(const Vector4& color)
{   
    //マテリアル用のリソースを作る。
    size_t bufferSize = (sizeof(MaterialForLine) + 255) & ~255;
    materialResource_ = DirectXCommon::CreateBufferResource(bufferSize);

    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&material_));
    material_->color = color;
}
