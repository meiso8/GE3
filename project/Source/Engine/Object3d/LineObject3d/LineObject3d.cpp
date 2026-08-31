#include "LineObject3d.h"
#include"MakeMatrix.h"
#include"DirectXCommon.h"
#include"Log.h"
#include"ObjectManager/ObjectManager.h"

void LineObject3d::SetVertex(const Vector3& start, const Vector3& end)
{
    line_->SetVertex(start, end);
}

void LineObject3d::Create(const Vector3& startPos, const Vector3& endPos)
{

    line_ = std::make_unique<LineMesh>();
    line_->Create(startPos, endPos);

    //座標のリソース作成
    CreateTransformationMatrix();
    //マテリアルリソース作成
    CreateMaterial({ 1.0f,1.0f,1.0f,1.0f });
    //メッシュを設定
    SetMeshAndMaterial(line_.get());
    //生成と同時に初期化
    Initialize();
}

void LineObject3d::Draw(
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

    useWorldMatrix_ = useWorldMatrix;
    blendMode_ = blendMode;
    cullMode_ = cullMode;
    maskMode_ = maskMode;

    usePSOKey_ = usePSOKey;
    rootSignatureType_ = rootSignatureType;

    vsType_ = vsType;
    psType_ = psType;

    ObjectManager::GetInstance()->SetDrawObject(materialForLine_->color.w, this);
}

void LineObject3d::DrawCommand(Camera& camera)
{
    if (useWorldMatrix_) {
        transformationMatrixResource_.data->World = worldTransform_.matWorld_;
    } else {
        transformationMatrixResource_.data->World = MakeIdentity4x4();
    }

    transformationMatrixResource_.data->WorldInverseTranspose = Transpose(Inverse(worldTransform_.matWorld_));
    transformationMatrixResource_.data->WVP = Multiply(worldTransform_.matWorld_, camera.GetViewProjectionMatrix());

    line_->SetRootSignatureAndGraphicsPipeline(commandList_, blendMode_, cullMode_, maskMode_, usePSOKey_, rootSignatureType_, vsType_, psType_);

    //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
    commandList_->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());
    //wvp用のCBufferの場所を設定
    commandList_->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_.GetGPUVirtualAddress());

#pragma region MeshDraw

    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    commandList_->IASetPrimitiveTopology(line_->GetTopology());
    commandList_->IASetVertexBuffers(0, 1, &line_->GetVertexBufferView());
    //ラインはインデックス関係なく2点のみのためこれ
    commandList_->DrawInstanced(line_->GetVertexCount(), 1, 0, 0);
#pragma endregion

}

LineObject3d::LineObject3d()
{
}

LineObject3d::~LineObject3d()
{

}

void LineObject3d::CreateMaterial(const Vector4& color)
{
    //マテリアル用のリソースを作る。
    materialResource_.resource = ResourceFactory::CreateBufferResource(sizeof(MaterialForLine));
    //マテリアルにデータを書き込む
    materialResource_.resource->SetName(L"LineObject3d_materialResource");
    //書き込むためのアドレスを取得
    HRESULT result = materialResource_.resource->Map(0, nullptr, reinterpret_cast<void**>(&materialForLine_));
    materialForLine_->color = color;
}
