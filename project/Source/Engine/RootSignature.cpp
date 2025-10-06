#include "RootSignature.h"

#include<cassert>

#include"Log.h"

#include<iostream>
#include"DirectXCommon.h"


void RootSignature::Create() {

#pragma region//rootSignature
    //rootSignature作成
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature[2]{};
    descriptionRootSignature[0].Flags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    descriptionRootSignature[1].Flags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
#pragma endregion

#pragma region//DescriptorRange
    //DescriptorRange
    D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
    descriptorRange[0].BaseShaderRegister = 2;//2から始める
    descriptorRange[0].NumDescriptors = 1;//1つ
    descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRV
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//オフセット自動計算

    //Instancing用
    D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
    descriptorRangeForInstancing[0].BaseShaderRegister = 3; // gTransformationMatrices : register(t3)
    descriptorRangeForInstancing[0].NumDescriptors = 1;
    descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    //D3D12_DESCRIPTOR_RANGE waveDescriptorRange[1] = {};
    //waveDescriptorRange[0].BaseShaderRegister = 1; // gWave : register(t1)
    //waveDescriptorRange[0].NumDescriptors = 1;
    //waveDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    //waveDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

#pragma endregion

#pragma region//Sampler
    //Smaplerの設定
    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイナリフィルタ
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0-1の範囲外をリピート
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較せぬ
    staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
    staticSamplers[0].ShaderRegister = 0;
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う

    descriptionRootSignature[0].pStaticSamplers = staticSamplers;
    descriptionRootSignature[0].NumStaticSamplers = _countof(staticSamplers);

    descriptionRootSignature[1].pStaticSamplers = staticSamplers;
    descriptionRootSignature[1].NumStaticSamplers = _countof(staticSamplers);
#pragma endregion



#pragma region//NormalRootParameters
    //CBufferを利用することになったので、RootParameterに設定を追加する
   /* RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform*/
    D3D12_ROOT_PARAMETER rootParameters[6] = {};
    //Material
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う
    //Transform用
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
    rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う

    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Table
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数
    //DirectionalLight
    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParameters[3].Descriptor.ShaderRegister = 1;//レジスタ番号1を使う
    //Wave
    rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;//SRVを使う
    rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
    rootParameters[4].Descriptor.ShaderRegister = 1;//レジスタ番号1を使う

    //rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    //rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    //rootParameters[4].DescriptorTable.pDescriptorRanges = waveDescriptorRange;
    //rootParameters[4].DescriptorTable.NumDescriptorRanges = _countof(waveDescriptorRange);

    //Ballon
    rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
    rootParameters[5].Descriptor.ShaderRegister = 1;//レジスタ番号1を使う
#pragma endregion

#pragma region//ParticleRootParameters
    //CBufferを利用することになったので、RootParameterに設定を追加する
   /* RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform*/
    D3D12_ROOT_PARAMETER rootParametersForInstancing[3] = {};
    //Material
    rootParametersForInstancing[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
    rootParametersForInstancing[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParametersForInstancing[0].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う

    //Transform用
    rootParametersForInstancing[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Table
    rootParametersForInstancing[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
    rootParametersForInstancing[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;//レジスタ番号0を使う
    rootParametersForInstancing[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);//Tableで利用する数
    //Texture?
    rootParametersForInstancing[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Table
    rootParametersForInstancing[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
    rootParametersForInstancing[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
    rootParametersForInstancing[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数
#pragma endregion

    descriptionRootSignature[0].pParameters = rootParameters;//ルートパラメータ配列へのポインタ
    descriptionRootSignature[0].NumParameters = _countof(rootParameters);//配列の長さ

    descriptionRootSignature[1].pParameters = rootParametersForInstancing;//ルートパラメータ配列へのポインタ
    descriptionRootSignature[1].NumParameters = _countof(rootParametersForInstancing);//配列の長さ

    //シリアライズしてバイナリにする
    Microsoft::WRL::ComPtr <ID3DBlob> signatureBlob = nullptr;
    Microsoft::WRL::ComPtr <ID3DBlob> errorBlob = nullptr;

#pragma region//NormalRootParameterシリアライズしてバイナリにする
    HRESULT result = D3D12SerializeRootSignature(&descriptionRootSignature[0],
        D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);

    if (FAILED(result)) {
        LogFile::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }

    //バイナリ元に生成
    result = DirectXCommon::GetDevice()->CreateRootSignature(0,
        signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
        IID_PPV_ARGS(&rootSignatures_[0]));

    assert(SUCCEEDED(result));

#pragma endregion

#pragma region//InstancingRootParameterシリアライズしてバイナリにする

     result = D3D12SerializeRootSignature(&descriptionRootSignature[1],
        D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);

    if (FAILED(result)) {
        Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }

    //バイナリ元に生成
    result = device->CreateRootSignature(0,
        signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
        IID_PPV_ARGS(&rootSignatures_[1]));

    assert(SUCCEEDED(result));

#pragma endregion
}

