#pragma once
#include<d3d12.h>
#include<wrl.h>
#include"TransformationMatrix.h"  
#include"Camera.h"  
#include"Primitive.h"
#include"WorldTransform.h"
#include<memory>
#include"Mesh/LineMesh/LineMesh.h"

struct MaterialForLine
{
    float4 color;
};

class LineObject3d
{

public:
    WorldTransform worldTransform_;
protected:

    // ==============位置情報==================
    Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResource_ = nullptr;
    TransformationMatrixFor3D* transformationMatrixData_ = nullptr;

    //マテリアルリソース
    Microsoft::WRL::ComPtr <ID3D12Resource>materialResource_ = nullptr;
    MaterialForLine* material_ = nullptr;

    std::unique_ptr<LineMesh>line_ = nullptr;
private:

public:
    // ==============マテリアルデータ==================
    Vector4& GetColor() { return material_->color; };
    void SetColor(const Vector4& color) { material_->color = color; }
    // ==============重要==================
    void SetVertex(const Vector3& start, const Vector3& end);
    void Create(const Vector3& startPos, const Vector3& endPos);
    virtual void Initialize();
    virtual void Update();
    virtual void Draw(
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
private:
    void CreateTransformationMatrix();
    void CreateMaterial(const Vector4& color);
};