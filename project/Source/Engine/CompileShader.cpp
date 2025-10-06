#include<Windows.h>
#include "CompileShader.h"
#include"Log.h"

#include<format>
#include<cassert> //assertも利用するため

//HLSLをGPUが解釈できる形にするためのインクルード
#pragma comment(lib,"dxcompiler.lib")
#include"StringUtility.h"


void DxcCompiler::Initialize() {

    //dxcCompilerを初期化
    HRESULT result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
    assert(SUCCEEDED(result));
    result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
    assert(SUCCEEDED(result));

    //現時点ではincludeはしないが、includeに対応するための設定を行っていく
    result = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
    assert(SUCCEEDED(result));

}


void DxcCompiler::ShaderSetting() {

    //Shaderをコンパイルする
    vertexShaderBlob_ = CompileShader(L"resources/shader/Object3D.VS.hlsl",
        L"vs_6_0");
    assert(vertexShaderBlob_ != nullptr);

    pixelShaderBlob_[NORMAL] = CompileShader(L"resources/shader/Object3D.PS.hlsl",
        L"ps_6_0");
    assert(pixelShaderBlob_[NORMAL] != nullptr);

    pixelShaderBlob_[NONE_TEX] = CompileShader(L"resources/shader/Object3DNoneTex.PS.hlsl",
        L"ps_6_0");
    assert(pixelShaderBlob_[NONE_TEX] != nullptr);

}

//CompileShader関数
Microsoft::WRL::ComPtr<IDxcBlob> DxcCompiler::CompileShader(
    //CompilerするShaderファイルへのパス
    const std::wstring& filePath,
    //Compilerに使用するProfile
    const wchar_t* profile) {
    // ここの中身をこの後書いていく
    // 1.hlslファイルを読み込む

#pragma region //1.hlslファイルを読む
//ここからシェーダーをコンパイルする旨をログに出す
    LogFile::Log(StringUtility::ConvertString(std::format(L"Begin CompileShader,path:{},profile:{}\n", filePath, profile)));
    //hlslファイルを読む
    IDxcBlobEncoding* shaderSource = nullptr;
    HRESULT result = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
    //読めなかったら止める
    assert(SUCCEEDED(result));
    //読み込んだファイルの内容を設定する
    DxcBuffer shaderSourceBuffer;
    shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
    shaderSourceBuffer.Size = shaderSource->GetBufferSize();
    shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTF8の文字コードであることを追加

#pragma endregion

    // 2.Compileする
#pragma region//2.Compileする

    LPCWSTR arguments[] = {
    filePath.c_str(),//コンパイル対象のhlslファイル名
    L"-E",L"main",//エントリーポイントの設定。基本的にはmain以外にはしない
    L"-T",profile,//ShaderProfileの設定
    L"-Zi",L"-Qembed_debug",//デバック用の情報を埋め込む
    L"-Od",//最適化を外しておく
    L"-Zpr",//メモリレイアウトは行優先
    };

    //実際にShaderをコンパイルする
    IDxcResult* shaderResult = nullptr;
    result = dxcCompiler_->Compile(
        &shaderSourceBuffer,//読み込んだファイル
        arguments,//コンパイルオプション
        _countof(arguments),//コンパイルオプションの数
        includeHandler_,//includeが含まれた諸々
        IID_PPV_ARGS(&shaderResult)//コンパイル結果
    );


    //コンパイルエラーではなくdxcが起動できないなどの致命的な状況
    assert(SUCCEEDED(result));


#pragma endregion

    // 3.警告・エラーが出ていないか確認する
#pragma region //警告・エラーが出ていないか確認する
//警告・エラーが出ていたらログに出して止める
    IDxcBlobUtf8* shaderError = nullptr;
    result = shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);

    if (shaderError != nullptr && shaderError->GetStringLength() != 0) {

        LogFile::Log(shaderError->GetStringPointer());

        //警告・エラーダメ絶対
        assert(false);
    } else {
        LogFile::Log("No shader compilation errors detected.");
    }
#pragma endregion

    // 4.Compile結果を受け取って返す
#pragma region//Compile結果を受け取って返す
    //コンパイル結果空実行用のバイナリ部分を取得 Blob = BinaryLargeObjectの略
    IDxcBlob* shaderBlob = nullptr;
    result = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
    assert(SUCCEEDED(result));
    //成功したログを出す
    LogFile::Log(StringUtility::ConvertString(std::format(L"Compile Succeeded,path:{}profile;{}\n", filePath, profile)));
    //もう使わないリソース解放
    shaderSource->Release();
    shaderResult->Release();
    //実行用のバイナリを返却
    return shaderBlob;
#pragma endregion

}
