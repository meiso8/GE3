#include "RootSignature.h"

#include<cassert>

#include"Log.h"

#include<iostream>
#include"DirectXCommon.h"

void RootSignature::Create() {

#pragma region//rootSignature
    //rootSignature作成
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature[TYPES]{};
    for (uint32_t i = 0; i < TYPES; ++i) {
        descriptionRootSignature[i].Flags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    }

#pragma endregion

#pragma region//DescriptorRange

    //DepthBasedOutline
    D3D12_DESCRIPTOR_RANGE descriptorRangeForDepthBasedOutline[1] = {};
    descriptorRangeForDepthBasedOutline[0].BaseShaderRegister = 1;//1から始める Texture2D<float32_t4> gTexture : register(t1); 
    descriptorRangeForDepthBasedOutline[0].NumDescriptors = 1;//1つ
    descriptorRangeForDepthBasedOutline[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRV
    descriptorRangeForDepthBasedOutline[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//オフセット自動計算

    //DescriptorRange
    D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
    descriptorRange[0].BaseShaderRegister = 2;//2から始める Texture2D<float32_t4> gTexture : register(t2); 
    descriptorRange[0].NumDescriptors = 1;//1つ
    descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRV
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//オフセット自動計算

    //Instancing用
    D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
    descriptorRangeForInstancing[0].BaseShaderRegister = 3; // gTransformationMatrices : register(t3)
    descriptorRangeForInstancing[0].NumDescriptors = 1;
    descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    //PointLight用
    D3D12_DESCRIPTOR_RANGE descriptorRangeForPointLight[1] = {};
    descriptorRangeForPointLight[0].BaseShaderRegister = 4; // : register(t4)
    descriptorRangeForPointLight[0].NumDescriptors = 1;
    descriptorRangeForPointLight[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRangeForPointLight[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    //SpotLight用
    D3D12_DESCRIPTOR_RANGE descriptorRangeForSpotLight[1] = {};
    descriptorRangeForSpotLight[0].BaseShaderRegister = 5; // : register(t5)
    descriptorRangeForSpotLight[0].NumDescriptors = 1;
    descriptorRangeForSpotLight[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRangeForSpotLight[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    //MatrixPalette用
    D3D12_DESCRIPTOR_RANGE descriptorRangeForMatrixPalette[1] = {};
    descriptorRangeForMatrixPalette[0].BaseShaderRegister = 6;// StructuredBuffer<Well> gMatrixPalette : register(t6);
    descriptorRangeForMatrixPalette[0].NumDescriptors = 1;
    descriptorRangeForMatrixPalette[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRangeForMatrixPalette[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


    //DescriptorRange
    D3D12_DESCRIPTOR_RANGE descriptorRangeForSkyBox[1] = {};
    descriptorRangeForSkyBox[0].BaseShaderRegister = 7;//2から始める Texture2D<float32_t4> gTexture : register(t2); 
    descriptorRangeForSkyBox[0].NumDescriptors = 1;//1つ
    descriptorRangeForSkyBox[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRV
    descriptorRangeForSkyBox[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//オフセット自動計算


    //D3D12_DESCRIPTOR_RANGE waveDescriptorRange[1] = {};
    //waveDescriptorRange[0].BaseShaderRegister = 1; // StructuredBuffer<Wave> gWave : register(t1);
    //waveDescriptorRange[0].NumDescriptors = 1;
    //waveDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    //waveDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

#pragma endregion

#pragma region//Sampler
    //Smaplerの設定
    D3D12_STATIC_SAMPLER_DESC staticSamplers[2] = {};
    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイナリフィルタ
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0-1の範囲外をリピート
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較せぬ
    staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
    staticSamplers[0].ShaderRegister = 0;
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う

    staticSamplers[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;//ぽいんとフィルタ
    staticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0-1の範囲外をリピート
    staticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較せぬ
    staticSamplers[1].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
    staticSamplers[1].ShaderRegister = 1;
    staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う

    for (uint32_t i = 0; i < TYPES; ++i) {
        //同じサンプラーをセットする
        descriptionRootSignature[i].pStaticSamplers = staticSamplers;
        descriptionRootSignature[i].NumStaticSamplers = _countof(staticSamplers);
    }

#pragma endregion

#pragma region//NormalRootParameters
    //CBufferを利用することになったので、RootParameterに設定を追加する
   /* RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform*/
    D3D12_ROOT_PARAMETER rootParameters[11] = {};
    //Material b0
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う
    //Transform用 b0
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
    rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う
    //Texture t2
    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Table
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数
    //DirectionalLight b1
    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParameters[3].Descriptor.ShaderRegister = 1;//レジスタ番号1を使う
    //Wave t1
    rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;//SRVを使う
    rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
    rootParameters[4].Descriptor.ShaderRegister = 1;//レジスタ番号1を使う

    //rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    //rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    //rootParameters[4].DescriptorTable.pDescriptorRanges = waveDescriptorRange;
    //rootParameters[4].DescriptorTable.NumDescriptorRanges = _countof(waveDescriptorRange);

    //Ballon b1
    rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
    rootParameters[5].Descriptor.ShaderRegister = 1;//レジスタ番号1を使う

    //Camera
    rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParameters[6].Descriptor.ShaderRegister = 2;//レジスタ番号2を使う
    //PointLight t4
    rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Table
    rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParameters[7].DescriptorTable.pDescriptorRanges = descriptorRangeForPointLight;//Tableの中身の配列を指定
    rootParameters[7].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForPointLight);//Tableで利用する数

    //DirectionalLight b3
    rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParameters[8].Descriptor.ShaderRegister = 3;//レジスタ番号1を使う

    //SpotLight t5
    rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Table
    rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParameters[9].DescriptorTable.pDescriptorRanges = descriptorRangeForSpotLight;//Tableの中身の配列を指定
    rootParameters[9].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForSpotLight);//Tableで利用する数

    //SkyBox Texture t6
    rootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Table
    rootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParameters[10].DescriptorTable.pDescriptorRanges = descriptorRangeForSkyBox;//Tableの中身の配列を指定
    rootParameters[10].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForSkyBox);//Tableで利用する数
#pragma endregion

#pragma region//rootParametersForSkinning

    D3D12_ROOT_PARAMETER rootParametersForSkinning[12] = {};

    for (int i = 0; i < 11; ++i) {
        rootParametersForSkinning[i] = rootParameters[i];
    }

    //MatrixPalette t6
    rootParametersForSkinning[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Table
    rootParametersForSkinning[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
    rootParametersForSkinning[11].DescriptorTable.pDescriptorRanges = descriptorRangeForMatrixPalette;//Tableの中身の配列を指定
    rootParametersForSkinning[11].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForMatrixPalette);//Tableで利用する数
#pragma endregion

#pragma region//ParticleRootParameters
    //CBufferを利用することになったので、RootParameterに設定を追加する
   /* RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform*/
    D3D12_ROOT_PARAMETER rootParametersForInstancing[3] = {};
    //Material b0
    rootParametersForInstancing[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParametersForInstancing[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParametersForInstancing[0].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う

    //Transform用 t3
    rootParametersForInstancing[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Table
    rootParametersForInstancing[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
    rootParametersForInstancing[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;//レジスタ番号0を使う
    rootParametersForInstancing[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);//Tableで利用する数
    //Texture? t2
    rootParametersForInstancing[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Table
    rootParametersForInstancing[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParametersForInstancing[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
    rootParametersForInstancing[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数
#pragma endregion


#pragma region//SpriteParameters
    //CBufferを利用することになったので、RootParameterに設定を追加する
   /* RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform*/
    D3D12_ROOT_PARAMETER rootParametersForSprite[3] = {};
    //Material
    rootParametersForSprite[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParametersForSprite[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParametersForSprite[0].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う
    //Transform用
    rootParametersForSprite[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParametersForSprite[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
    rootParametersForSprite[1].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う
    //Texture?
    rootParametersForSprite[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Table
    rootParametersForSprite[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParametersForSprite[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
    rootParametersForSprite[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数

#pragma endregion

#pragma region//SpriteParametersForFont
    //CBufferを利用することになったので、RootParameterに設定を追加する
   /* RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform*/
    D3D12_ROOT_PARAMETER rootParametersForFont[3] = {};
    //Material
    rootParametersForFont[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParametersForFont[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParametersForFont[0].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う
    //Transform用
    rootParametersForFont[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParametersForFont[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
    rootParametersForFont[1].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う
    //Texture?
    rootParametersForFont[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Table
    rootParametersForFont[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParametersForFont[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
    rootParametersForFont[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数

#pragma endregion

#pragma region//SkyboxParameters

    D3D12_ROOT_PARAMETER rootParametersForSkyBox[3] = {};
    //Material b0
    rootParametersForSkyBox[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParametersForSkyBox[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParametersForSkyBox[0].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う
    //Transform用 b0
    rootParametersForSkyBox[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParametersForSkyBox[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
    rootParametersForSkyBox[1].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う
    //Texture t2
    rootParametersForSkyBox[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Table
    rootParametersForSkyBox[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParametersForSkyBox[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
    rootParametersForSkyBox[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数

#pragma endregion


#pragma region//offScreenParameters

    D3D12_ROOT_PARAMETER rootParametersForOffScreen[2] = {};
    //Texture t2
    rootParametersForOffScreen[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Table
    rootParametersForOffScreen[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParametersForOffScreen[0].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
    rootParametersForOffScreen[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数
    //Material b0
    rootParametersForOffScreen[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParametersForOffScreen[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParametersForOffScreen[1].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う

#pragma endregion

#pragma region//DepthBasedOutline
    D3D12_ROOT_PARAMETER rootParametersForDepthBasedOutline[3] = {};

    //Material b0
    rootParametersForDepthBasedOutline[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParametersForDepthBasedOutline[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParametersForDepthBasedOutline[0].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う

    //DepthTexture t1
    rootParametersForDepthBasedOutline[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Table
    rootParametersForDepthBasedOutline[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParametersForDepthBasedOutline[1].DescriptorTable.pDescriptorRanges = descriptorRangeForDepthBasedOutline;//Tableの中身の配列を指定
    rootParametersForDepthBasedOutline[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForDepthBasedOutline);//Tableで利用する数
    //Texture t2
    rootParametersForDepthBasedOutline[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Table
    rootParametersForDepthBasedOutline[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParametersForDepthBasedOutline[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
    rootParametersForDepthBasedOutline[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数


#pragma endregion
#pragma region//RandomParam

    D3D12_ROOT_PARAMETER rootParametersForRandom[1] = {};
    //Material b0
    rootParametersForRandom[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParametersForRandom[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParametersForRandom[0].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う

#pragma endregion

#pragma region//Thermography

    D3D12_ROOT_PARAMETER rootParameterForThermography[2] = {};
    //Material b0
    rootParameterForThermography[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParameterForThermography[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParameterForThermography[0].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う

    //Texture2D<float4> gTemperatureTexture : register(t2);
    rootParameterForThermography[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Table
    rootParameterForThermography[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParameterForThermography[1].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
    rootParameterForThermography[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数

#pragma endregion

    descriptionRootSignature[NORMAL].pParameters = rootParameters;//ルートパラメータ配列へのポインタ
    descriptionRootSignature[NORMAL].NumParameters = _countof(rootParameters);//配列の長さ

    descriptionRootSignature[PARTICLE].pParameters = rootParametersForInstancing;//ルートパラメータ配列へのポインタ
    descriptionRootSignature[PARTICLE].NumParameters = _countof(rootParametersForInstancing);//配列の長さ

    descriptionRootSignature[SPRITE].pParameters = rootParametersForSprite;//ルートパラメータ配列へのポインタ
    descriptionRootSignature[SPRITE].NumParameters = _countof(rootParametersForSprite);//配列の長さ

    descriptionRootSignature[SKINNING].pParameters = rootParametersForSkinning;//ルートパラメータ配列へのポインタ
    descriptionRootSignature[SKINNING].NumParameters = _countof(rootParametersForSkinning);//配列の長さ

    descriptionRootSignature[FONT].pParameters = rootParametersForFont;//ルートパラメータ配列へのポインタ
    descriptionRootSignature[FONT].NumParameters = _countof(rootParametersForFont);//配列の長さ

    descriptionRootSignature[SKYBOX].pParameters = rootParametersForSkyBox;//ルートパラメータ配列へのポインタ
    descriptionRootSignature[SKYBOX].NumParameters = _countof(rootParametersForSkyBox);//配列の長さ

    descriptionRootSignature[OFFSCREEN].pParameters = rootParametersForOffScreen;//ルートパラメータ配列へのポインタ
    descriptionRootSignature[OFFSCREEN].NumParameters = _countof(rootParametersForOffScreen);//配列の長さ
    descriptionRootSignature[GRAYSCALE].pParameters = rootParametersForOffScreen;//ルートパラメータ配列へのポインタ
    descriptionRootSignature[GRAYSCALE].NumParameters = _countof(rootParametersForOffScreen);//配列の長さ
    descriptionRootSignature[VIGNETTE].pParameters = rootParametersForOffScreen;//ルートパラメータ配列へのポインタ
    descriptionRootSignature[VIGNETTE].NumParameters = _countof(rootParametersForOffScreen);//配列の長さ
    descriptionRootSignature[BOXFILTER].pParameters = rootParametersForOffScreen;//ルートパラメータ配列へのポインタ
    descriptionRootSignature[BOXFILTER].NumParameters = _countof(rootParametersForOffScreen);//配列の長さ
    descriptionRootSignature[GAUSSIANFILTER].pParameters = rootParametersForOffScreen;//ルートパラメータ配列へのポインタ
    descriptionRootSignature[GAUSSIANFILTER].NumParameters = _countof(rootParametersForOffScreen);//配列の長さ
    descriptionRootSignature[LUMINANCE_BASED_OUTLINE].pParameters = rootParametersForOffScreen;//ルートパラメータ配列へのポインタ
    descriptionRootSignature[LUMINANCE_BASED_OUTLINE].NumParameters = _countof(rootParametersForOffScreen);//配列の長さ

    descriptionRootSignature[DEPTH_BASED_OUTLINE].pParameters = rootParametersForDepthBasedOutline;//ルートパラメータ配列へのポインタ
    descriptionRootSignature[DEPTH_BASED_OUTLINE].NumParameters = _countof(rootParametersForDepthBasedOutline);//配列の長さ

    descriptionRootSignature[RADIAL_BLUR].pParameters = rootParametersForOffScreen;//ルートパラメータ配列へのポインタ
    descriptionRootSignature[RADIAL_BLUR].NumParameters = _countof(rootParametersForOffScreen);//配列の長さ

    descriptionRootSignature[DISSOLVE].pParameters = rootParametersForDepthBasedOutline;//ルートパラメータ配列へのポインタ
    descriptionRootSignature[DISSOLVE].NumParameters = _countof(rootParametersForDepthBasedOutline);//配列の長さ

    descriptionRootSignature[RANDOM].pParameters = rootParametersForRandom;
    descriptionRootSignature[RANDOM].NumParameters = _countof(rootParametersForRandom);//配列の長さ

    descriptionRootSignature[THERMOGRAPHY].pParameters = rootParameterForThermography;
    descriptionRootSignature[THERMOGRAPHY].NumParameters = _countof(rootParameterForThermography);//配列の長さ

    //シリアライズしてバイナリにする
    Microsoft::WRL::ComPtr <ID3DBlob> signatureBlob = nullptr;
    Microsoft::WRL::ComPtr <ID3DBlob> errorBlob = nullptr;

#pragma region//NormalRootParameterシリアライズしてバイナリにする

    for (uint32_t i = 0; i < TYPES; ++i) {

        HRESULT result = D3D12SerializeRootSignature(&descriptionRootSignature[i],
            D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);

        if (FAILED(result)) {
            LogFile::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
            assert(false);
        }

        //バイナリ元に生成
        result = DirectXCommon::GetDevice()->CreateRootSignature(0,
            signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
            IID_PPV_ARGS(&rootSignatures_[i]));

        assert(SUCCEEDED(result));
    }

#pragma endregion

}

