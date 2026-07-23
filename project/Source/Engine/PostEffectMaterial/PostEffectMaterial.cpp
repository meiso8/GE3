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

void PostEffectMaterial::Update()
{
    assert(camera_);
    materialForDepthBasedOutline_.data->projectionInverse = Inverse(camera_->GetProjectionMatrixForOutline());
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
    CreateMaterialMosaic();
}

void PostEffectMaterial::Clear()
{
    materialForGrayScale_.UnMap();
    materialForGrayScale_.Reset();

    materialForVignette_.UnMap();
    materialForVignette_.Reset();

    materialForBoxFilter_.UnMap();
    materialForBoxFilter_.Reset();

    materialForFullScreen_.UnMap();
    materialForFullScreen_.Reset();

    materialForGaussianFilter_.UnMap();
    materialForGaussianFilter_.Reset();

    materialForLuminanceBasedOutline_.UnMap();
    materialForLuminanceBasedOutline_.Reset();

    materialForDepthBasedOutline_.UnMap();
    materialForDepthBasedOutline_.Reset();

    materialForRadialBlur_.UnMap();
    materialForRadialBlur_.Reset();

    materialForDissolve_.UnMap();
    materialForDissolve_.Reset();

    materialForThermography_.UnMap();
    materialForThermography_.Reset();

    materialForRandom_.UnMap();
    materialForRandom_.Reset();

    materialForMosaic_.UnMap();
    materialForMosaic_.Reset();
}

PostEffectMaterial::~PostEffectMaterial()
{
    Clear();
}

D3D12_GPU_VIRTUAL_ADDRESS PostEffectMaterial::GetGPUVirtualAddress(const PSO::EffectType& type)
{
    if (resourceMap_.contains(type)) {
        return resourceMap_.at(type)->GetGPUVirtualAddress();
    }

    return D3D12_GPU_VIRTUAL_ADDRESS();
}

void PostEffectMaterial::CreateMaterialBufferForGrayScale()
{
    //マテリアル用のリソースを作る。
    materialForGrayScale_.CreateBufferResource(L"GrayScale_MaterialResource");
    //マップする
    resourceMap_[PSO::kEffectGrayScale] = materialForGrayScale_.Get();
    //書き込むためのアドレスを取得
    materialForGrayScale_.Map();
    //マテリアルにデータを書き込む
    materialForGrayScale_.data->color = sepiaColor_;
    materialForGrayScale_.data->effectVol = 0.0f;

    LogFile::Log("Create : MaterialBuffer : GrayScale\n");
}

void PostEffectMaterial::CreateMaterialBufferForVignette()
{    //マテリアル用のリソースを作る。
    materialForVignette_.CreateBufferResource(L"Vignette_MaterialResource");
    
    //マップする
    resourceMap_[PSO::kEffectVignette] = materialForVignette_.Get();

    //書き込むためのアドレスを取得
    materialForVignette_.Map();

    //マテリアルにデータを書き込む
    materialForVignette_.data->correctVal = 16.0f;
    materialForVignette_.data->viignetteVal = 0.8f;

    LogFile::Log("Create : MaterialBuffer : Vignette\n");
}

void PostEffectMaterial::CreateMaterialBufferForBoxFilter()
{
    materialForBoxFilter_.CreateBufferResource(L"BoxFilter_MaterialResource");
  
    //マップする
    resourceMap_[PSO::kEffectBoxFilter] = materialForBoxFilter_.Get();

    //書き込むためのアドレスを取得
    materialForBoxFilter_.Map();
    //マテリアルにデータを書き込む
    materialForBoxFilter_.data->kernel = 1.0f;

    LogFile::Log("Create : MaterialBuffer : BoxFilter\n");
}

void PostEffectMaterial::CreateMaterialBUfferForFullScreen()
{
    //マテリアル用のリソースを作る。
    materialForFullScreen_.CreateBufferResource(L"FullScreen_MaterialResource");

    //マップする
    resourceMap_[PSO::kEffectNone] = materialForFullScreen_.Get();

    //書き込むためのアドレスを取得
    materialForFullScreen_.Map();
    //マテリアルにデータを書き込む
    materialForFullScreen_.data->color = { 1.0f,1.0f,1.0f,1.0f };

    LogFile::Log("Create : MaterialBuffer : GrayScale\n");
}

void PostEffectMaterial::CreateMaterialBufferForGaussianFilter()
{
    //マテリアル用のリソースを作る。
    materialForGaussianFilter_.CreateBufferResource(L"GaussianFilter_MaterialResource");
    
    //マップする
    resourceMap_[PSO::kEffectGaussianFilter] = materialForGaussianFilter_.Get();

    //書き込むためのアドレスを取得
    materialForGaussianFilter_.Map();
    //マテリアルにデータを書き込む
    materialForGaussianFilter_.data->sigma = 1.0f;
    materialForGaussianFilter_.data->kernel = 1;

    LogFile::Log("Create : MaterialBuffer : GaussianFilter\n");
}

void PostEffectMaterial::CreateMaterialLuminanceBasedOutline()
{
    //マテリアル用のリソースを作る。
    materialForLuminanceBasedOutline_.CreateBufferResource(L"LuminanceBasedOutline_MaterialResource");
  
    //マップする
    resourceMap_[PSO::kEffectLuminanceBasedOutline] = materialForLuminanceBasedOutline_.Get();
    //書き込むためのアドレスを取得
    materialForLuminanceBasedOutline_.Map();
    //マテリアルにデータを書き込む
    materialForLuminanceBasedOutline_.data->weightVal = 0.0f;

    LogFile::Log("Create : MaterialBuffer : LuminanceBasedOutline\n");
}

void PostEffectMaterial::CreateMaterialDepthBasedOutline()
{
    //マテリアル用のリソースを作る。
    materialForDepthBasedOutline_.CreateBufferResource(L"DepthBasedOutline_MaterialResource");
    //マップする
    resourceMap_[PSO::kEffectDepthBasedOutline] = materialForDepthBasedOutline_.Get();
    
    //書き込むためのアドレスを取得
    materialForDepthBasedOutline_.Map();
    //マテリアルにデータを書き込む
    materialForDepthBasedOutline_.data->projectionInverse = MakeIdentity4x4();
    materialForDepthBasedOutline_.data->lineWidth = 10000.0f;
    materialForDepthBasedOutline_.data->color = { 0.0f,0.0f,0.0f };

    LogFile::Log("Create : MaterialBuffer : DepthBasedOutline\n");
}


void PostEffectMaterial::CreateMaterialRadialBlur()
{

    //マテリアル用のリソースを作る。
    materialForRadialBlur_.CreateBufferResource(L"Dissolve_MaterialResource");

    //マップする
    resourceMap_[PSO::kEffectRadialBlur] = materialForRadialBlur_.Get();

    //書き込むためのアドレスを取得
    materialForRadialBlur_.Map();
    //マテリアルにデータを書き込む
    materialForRadialBlur_.data->center = { 0.5f,0.5f };
    materialForRadialBlur_.data->numSamples = 1;
    materialForRadialBlur_.data->blurWidth = 0.01f;

    LogFile::Log("Create : MaterialBuffer : RadialBlur\n");
}

void PostEffectMaterial::CreateMaterialDissolve() {

    //マテリアル用のリソースを作る。
    materialForDissolve_.CreateBufferResource(L"Dissolve_MaterialResource");

    //マップする
    resourceMap_[PSO::kEffectDissolve] = materialForDissolve_.Get();

    //書き込むためのアドレスを取得
    materialForDissolve_.Map();
    //マテリアルにデータを書き込む
    materialForDissolve_.data->maskVal = 1.0f;
    materialForDissolve_.data->rgb = { 8.0f / 255.0f, 16.0f / 255.0f,0.0f };
    LogFile::Log("Create : MaterialBuffer : Dissolve\n");
}
void PostEffectMaterial::CreateMaterialRandom()
{
    //マテリアル用のリソースを作る。
    materialForRandom_.CreateBufferResource(L"Random_MaterialResource");
   
    //マップする
    resourceMap_[PSO::kEffectRandom] = materialForRandom_.Get();
    //書き込むためのアドレスを取得
    materialForRandom_.Map();
    //マテリアルにデータを書き込む
    materialForRandom_.data->time = 1.0f;

    LogFile::Log("Create : MaterialBuffer : Dissolve\n");
}
void PostEffectMaterial::CreateMaterialThermography()
{
    //マテリアル用のリソースを作る。
    materialForThermography_.CreateBufferResource(L"Thermography_MaterialResource");
    //マップする
    resourceMap_[PSO::kEffectThermography] = materialForThermography_.Get();
    //書き込むためのアドレスを取得
    materialForThermography_.Map();
    //データを書きこむ
    materialForThermography_.data->alpha = { 1.0f };
    materialForThermography_.data->sigma = 10.0f;
    materialForThermography_.data->kernel = 14;
    LogFile::Log("Create : MaterialBuffer : Thermography\n");
}
void PostEffectMaterial::CreateMaterialMosaic()
{

    //マテリアル用のリソースを作る。
    materialForMosaic_.CreateBufferResource(L"Mosaic_MaterialResource");
    //マップする
    resourceMap_[PSO::kEffectMosaic] = materialForMosaic_.Get();
    //書き込むためのアドレスを取得
    materialForMosaic_.Map();
    //データを書きこむ
    materialForMosaic_.data->useMosaic = false;
    materialForMosaic_.data->size = 64;
    materialForMosaic_.data->vol = 1.0f;
    LogFile::Log("Create : MaterialBuffer : Mosaic\n");
    

}
;