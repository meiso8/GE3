#pragma once
#include"Camera.h"  
#include"Primitive.h"
#include<memory>
#include"Object3d/Object3d.h"
#include"LineMesh/LineMesh.h"

class LineObject3d :public Object3d
{

public:
    struct MaterialForLine
    {
        float4 color;
    };

protected:
    MaterialForLine* materialForLine_ = nullptr;
    std::unique_ptr<LineMesh>line_ = nullptr;
public:
    /// @brief 描画関数
    /// @param camera カメラのセット
    /// @param useWorldMatrix ワールド行列を使うかの設定
    /// @param blendMode ブレンドモード
    /// @param cullMode カリング
    /// @param maskMode マスク
    /// @param usePSOKey PSOが無かったら生成するかどうか
    /// @param rootSignatureType rootSignatureTypeの設定
    /// @param vsType 頂点シェーダータイプ
    /// @param psType ピクセルシェーダータイプ
    void Draw(
        Camera& camera,
        const bool useWorldMatrix,
        const BlendMode& blendMode = kBlendModeNone,
        const CullMode& cullMode = kCullModeBack,
        const MaskMode maskMode = kAll,
        const bool usePSOKey = false,
        const RootSignature::TYPE rootSignatureType = RootSignature::TYPE::LINE,
        const DxcCompiler::VS_TYPE vsType = DxcCompiler::VS_TYPE::VS_Line,
        const DxcCompiler::PS_TYPE psType = DxcCompiler::PS_TYPE::PS_Line
    );
    LineObject3d();
    ~LineObject3d();
    // ==============マテリアルデータ==================
    Vector4& GetColor() override { return materialForLine_->color; };
    void SetColor(const Vector4& color)override { materialForLine_->color = color; }
    // ==============重要==================
    void SetVertex(const Vector3& start, const Vector3& end);
    void Create(const Vector3& startPos, const Vector3& endPos);
private:
    void CreateMaterial(const Vector4& color);
};