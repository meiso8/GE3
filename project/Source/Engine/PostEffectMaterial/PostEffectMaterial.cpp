#include "PostEffectMaterial.h"
#include"Camera.h"
#include"DirectXCommon.h"
#ifdef _DEVELOP
#include "DebugUI.h"
#endif
#include"Log.h"
#include"MakeMatrix.h"

void PostEffectMaterial::SetCamera(Camera* camera)
{
    camera_ = camera;
}

Microsoft::WRL::ComPtr<ID3D12Resource>& PostEffectMaterial::GetMaterialResouce(const PSO::EffectType& effectType)
{
    assert(effectType < PSO::kCountOfEffect);
    return materialResource_[effectType];
}

void PostEffectMaterial::Update()
{
    assert(camera_);
    materialForDepthBasedOutline_->projectionInverse = Inverse(camera_->GetProjectionMatrixForOutline());

#ifdef _DEVELOP

    ImGui::Begin("PostEffect");

    
    ImGui::End();

#endif
}

void PostEffectMaterial::Create()
{
    CreateMaterialBUfferForFullScreen();
    CreateMaterialBufferForGrayScale();
    CreateMaterialBufferForVignette();
    CreateMaterialBufferForBoxFilter();
    CreateMaterialBufferForGaussianFilter();
    CreateMaterialLuminanceBasedOutline();
    CreateMaterialDepthBasedOutline();
    CreateMaterialRadialBlur();
    CreateMaterialDissolve();
    CreateMaterialRandom();
    CreateMaterialThermography();
}

void PostEffectMaterial::Clear()
{

    for (auto& mat : materialResource_) {
        mat.Reset();
    }

}

void PostEffectMaterial::CreateMaterialBufferForGrayScale()
{
    //マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectGrayScale] = DirectXCommon::CreateBufferResource(sizeof(MaterialForRenderTexture));
    materialResource_[PSO::kEffectGrayScale]->SetName(L"RenderTexture_GrayScale_MaterialResource");
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectGrayScale]->Map(0, nullptr, reinterpret_cast<void**>(&materialForGrayScale_));
    materialForGrayScale_->color = sepiaColor_;

    LogFile::Log("Rendertexture : Create : MaterialBuffer : GrayScale\n");
}

void PostEffectMaterial::CreateMaterialBufferForVignette()
{    //マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectVignette] = DirectXCommon::CreateBufferResource(sizeof(MaterialForVignette));
    materialResource_[PSO::kEffectVignette]->SetName(L"RenderTexture_Vignette_MaterialResource");
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectVignette]->Map(0, nullptr, reinterpret_cast<void**>(&materialForVignette_));
    materialForVignette_->correctVal = 16.0f;
    materialForVignette_->viignetteVal = 0.8f;

    LogFile::Log("Rendertexture : Create : MaterialBuffer : Vignette\n");
}

void PostEffectMaterial::CreateMaterialBufferForBoxFilter()
{

    materialResource_[PSO::kEffectBoxFilter] = DirectXCommon::CreateBufferResource(sizeof(MaterialForBoxFilter));
    materialResource_[PSO::kEffectBoxFilter]->SetName(L"RenderTexture_BoxFilter_MaterialResource");
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectBoxFilter]->Map(0, nullptr, reinterpret_cast<void**>(&materialForBoxFilter_));
    materialForBoxFilter_->kernel = 1.0f;

    LogFile::Log("Rendertexture : Create : MaterialBuffer : BoxFilter\n");
}

void PostEffectMaterial::CreateMaterialBUfferForFullScreen()
{
    //マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectNone] = DirectXCommon::CreateBufferResource(sizeof(MaterialForRenderTexture));
    materialResource_[PSO::kEffectNone]->SetName(L"RenderTexture_None_MaterialResource");
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectNone]->Map(0, nullptr, reinterpret_cast<void**>(&materialForFullScreen_));
    materialForFullScreen_->color = { 1.0f,1.0f,1.0f,1.0f };

    LogFile::Log("Rendertexture : Create : MaterialBuffer : GrayScale\n");
}

void PostEffectMaterial::CreateMaterialBufferForGaussianFilter()
{

    //マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectGaussianFilter] = DirectXCommon::CreateBufferResource(sizeof(MaterialForGaussianFilter));
    materialResource_[PSO::kEffectGaussianFilter]->SetName(L"RenderTexture_GaussianFilter_MaterialResource");
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectGaussianFilter]->Map(0, nullptr, reinterpret_cast<void**>(&materialForGaussianFilter_));
    materialForGaussianFilter_->sigma = 1.0f;
    materialForGaussianFilter_->kernel = 1;

    LogFile::Log("Rendertexture : Create : MaterialBuffer : GaussianFilter\n");
}

void PostEffectMaterial::CreateMaterialLuminanceBasedOutline()
{
    //マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectLuminanceBasedOutline] = DirectXCommon::CreateBufferResource(sizeof(MaterialForLuminanceBasedOutline));
    materialResource_[PSO::kEffectLuminanceBasedOutline]->SetName(L"RenderTexture_LuminanceBasedOutline_MaterialResource");
    //マテリアルにデータを書き込む
    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectLuminanceBasedOutline]->Map(0, nullptr, reinterpret_cast<void**>(&materialForLuminanceBasedOutline_));
    materialForLuminanceBasedOutline_->weightVal = 0.0f;

    LogFile::Log("Rendertexture : Create : MaterialBuffer : LuminanceBasedOutline\n");
}

void PostEffectMaterial::CreateMaterialDepthBasedOutline()
{

    //マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectDepthBasedOutline] = DirectXCommon::CreateBufferResource(sizeof(MaterialForDepthBasedOutline));
    materialResource_[PSO::kEffectDepthBasedOutline]->SetName(L"RenderTexture_DepthBasedOutline_MaterialResource");
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectDepthBasedOutline]->Map(0, nullptr, reinterpret_cast<void**>(&materialForDepthBasedOutline_));
    materialForDepthBasedOutline_->projectionInverse = MakeIdentity4x4();
    materialForDepthBasedOutline_->lineWidth = 10000.0f;
    materialForDepthBasedOutline_->color = { 0.0f,0.0f,0.0f };

    LogFile::Log("Rendertexture : Create : MaterialBuffer : DepthBasedOutline\n");
}


void PostEffectMaterial::CreateMaterialRadialBlur()
{

    //マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectRadialBlur] = DirectXCommon::CreateBufferResource(sizeof(MaterialForRadialBlur));
    materialResource_[PSO::kEffectRadialBlur]->SetName(L"RenderTexture_Dissolve_MaterialResource");
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectRadialBlur]->Map(0, nullptr, reinterpret_cast<void**>(&materialForRadialBlur_));
    materialForRadialBlur_->center = { 0.5f,0.5f };
    materialForRadialBlur_->numSamples = 1;
    materialForRadialBlur_->blurWidth = 0.01f;

    LogFile::Log("Rendertexture : Create : MaterialBuffer : RadialBlur\n");
}

void PostEffectMaterial::CreateMaterialDissolve() {

    //マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectDissolve] = DirectXCommon::CreateBufferResource(sizeof(MaterialForDissolve));
    materialResource_[PSO::kEffectDissolve]->SetName(L"RenderTexture_Dissolve_MaterialResource");
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectDissolve]->Map(0, nullptr, reinterpret_cast<void**>(&materialForDissolve_));
    materialForDissolve_->maskVal = 1.0f;
    materialForDissolve_->rgb = { 8.0f / 255.0f, 16.0f / 255.0f,0.0f };
    LogFile::Log("Rendertexture : Create : MaterialBuffer : Dissolve\n");
}
void PostEffectMaterial::CreateMaterialRandom()
{
    //マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectRandom] = DirectXCommon::CreateBufferResource(sizeof(MaterialForDissolve));

    materialResource_[PSO::kEffectRandom]->SetName(L"RenderTexture_Random_MaterialResource");

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectRandom]->Map(0, nullptr, reinterpret_cast<void**>(&materialForRandom_));
    //マテリアルにデータを書き込む
    materialForRandom_->time = 1.0f;

    LogFile::Log("Rendertexture : Create : MaterialBuffer : Dissolve\n");
}
void PostEffectMaterial::CreateMaterialThermography()
{//マテリアル用のリソースを作る。
    materialResource_[PSO::kEffectThermography] = DirectXCommon::CreateBufferResource(sizeof(MaterialForThermography));
    //マテリアルにデータを書き込む
    materialResource_[PSO::kEffectThermography]->SetName(L"RenderTexture_Thermography_MaterialResource");
    //書き込むためのアドレスを取得
    HRESULT result = materialResource_[PSO::kEffectThermography]->Map(0, nullptr, reinterpret_cast<void**>(&materialForThermography_));
    materialForThermography_->alpha = { 1.0f };
    materialForThermography_->sigma = 10.0f;
    materialForThermography_->kernel = 14;
    LogFile::Log("Rendertexture : Create : MaterialBuffer : Thermography\n");
};