#include"Texture.h"
#include"Camera/Camera.h"
#include"ModelData.h"
#include"MaterialResource.h"
#include"TransformationMatrix.h"
#include"ModelConfig.h"
#include"Balloon.h"
#include"Wave.h"
#include"Transform.h"
#include"PSO.h"
#include"CommandList.h"
#include<d3d12.h>
#include<memory>

class Model
{
public:

    Model() = default;
    ~Model();
    void Create(const ModelData& modeldata, uint32_t index);

    void UpdateUV();

    void PreDraw(PSO& pso, PSO::PSOType type = PSO::PSOType::TRIANGLE);
    void Draw(const Matrix4x4& worldMatrix, Camera& camera, uint32_t lightType = MaterialResource::LIGHTTYPE::NONE);

    Material* GetMaterial() { return materialResource_.GetMaterial(); };

    VertexData* GetVertexData() {
        return vertexData_;
    }

    Balloon& GetExpansionData() {
        return *expansionData_;
    }

    Wave& GetWaveData(size_t index) { return waveData_[index]; };
    Transform& GetUVTransform() { return uvTransform_; }
    Vector4& GetColor() { return materialResource_.GetMaterial()->color; }
    void SetColor(const Vector4& color);


private:
    void CreateWorldVPResource();
private:
    ModelConfig* modelConfig_ = nullptr;
    ShaderResourceView srv_;

    Camera* camera_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
    TransformationMatrix* wvpDate_ = nullptr;

    ID3D12GraphicsCommandList* commandList_ =nullptr;
    Matrix4x4 worldViewProjectionMatrix_ = { 0.0f };
    MaterialResource materialResource_;
    const ModelData* modelData_;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    VertexData* vertexData_ = nullptr;

    Texture* texture_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> expansionResource_;
    Balloon* expansionData_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> waveResource_;
    Wave* waveData_ = nullptr;

    Transform uvTransform_;
    Matrix4x4 uvTransformMatrix_{};

};
