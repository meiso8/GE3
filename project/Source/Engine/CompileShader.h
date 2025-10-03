#pragma once

#include <dxcapi.h>
#include<string>
#include<wrl.h>
#include<vector>

//CompileShader関数
IDxcBlob* CompileShader(
    //CompilerするShaderファイルへのパス
    const std::wstring& filePath,
    //Compilerに使用するProfile
    const wchar_t* profile,
    //初期化で生成されたものを3つ
    IDxcUtils* dxcUtils,
    IDxcCompiler3* dxcCompiler,
    IDxcIncludeHandler* includeHandler);

class DxcCompiler
{
public:
    enum PSType {
        NORMAL,
        PS_MAX
    };
    void Initialize();
    void ShaderSetting();
    Microsoft::WRL::ComPtr <IDxcBlob>& GetVertexShaderBlob(uint32_t index) { return vertexShaderBlobs_[index]; };
    Microsoft::WRL::ComPtr <IDxcBlob>& GetPixelShaderBlob(uint32_t index) { return pixelShaderBlobs_[index]; };
private:
    IDxcUtils* dxcUtils_ = nullptr;
    IDxcCompiler3* dxcCompiler_ = nullptr;
    IDxcIncludeHandler* includeHandler_ = nullptr;
    std::vector<Microsoft::WRL::ComPtr<IDxcBlob>> vertexShaderBlobs_;
    std::vector<Microsoft::WRL::ComPtr<IDxcBlob>>pixelShaderBlobs_;
};