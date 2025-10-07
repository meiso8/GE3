#pragma once

#pragma region //自作関数
//#include"Window.h"
#include"Input.h"

#include"DirectXCommon.h"
#include "RootSignature.h"
#include"D3DResourceLeakChecker.h"

#include"Depth.h"//StencilTextureの作成関数　奥行き
#include"CompileShader.h"
#include"BlendState.h"
#include"RasterizerState.h"
#include"Texture.h"

#include"ShaderResourceView.h"
//#include"Model.h"
#include"Sprite.h"
#include"SphereMesh.h"
#include"LineMesh.h"
#include"Cube.h"

#include"Sound.h"

#include"Camera/DebugCamera.h"
#include"Camera/Camera.h"
#include"CrashHandler.h"
#include"Log.h"
#include"ImGuiClass.h"
#include"InputLayout.h"

#include"Material.h"
#include"VertexData.h"
#include"DirectionalLight.h"
#include"TransformationMatrix.h"
#include"ModelData.h"

#include"Normalize.h"
#include"Transform.h"
#include"MakeIdentity4x4.h"
#include"MakeAffineMatrix.h"
#include"Inverse.h"
#include"MakePerspectiveFovMatrix.h"
#include"MakeOrthographicMatrix.h"
#include"MakeRotateMatrix.h"
#include"Multiply.h"
#include"SphericalCoordinate.h"
#include"Lerp.h"

#include"DrawGrid.h"


#include"DebugUI.h"

#pragma endregion



class MyEngine {


public:
    MyEngine() = default;
    ~MyEngine() = default;
    static MyEngine* GetInstance();
    void Create(const std::wstring& title, const int32_t clientWidth, const int32_t clientHeight);
    void Update();
    void PreCommandSet(Vector4& color);
    void PostCommandSet();
    void End();


    Window& GetWC() { return *wc; };
    static RootSignature* GetRootSignature() { return rootSignature.get(); }
    static PSO* GetPSO(uint32_t index) { return &pso[index]; }
    static DirectionalLight* GetDirectionalLightData() { return directionalLightData; }
    static void SetBlendMode(uint32_t blendMode = BlendMode::kBlendModeNormal);

public:
    static const uint32_t kMaxSRVCount;
private:
    D3DResourceLeakChecker leakCheck = {};
    static MyEngine* instance_;

    std::unique_ptr<DirectXCommon> directXCommon = nullptr;
    std::unique_ptr<LogFile> logFile = nullptr;
    std::unique_ptr<Window> wc = nullptr;
    std::unique_ptr<InputLayout>inputLayout = nullptr;
    std::vector<BlendState> blendStates = {};
    std::vector<RasterizerState> rasterizerStates = {};

    Input* input = nullptr;
    DepthStencil depthStencil = {};

    Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightResource = nullptr;
    
    static std::array<PSO, kCountOfBlendMode> pso;
    static DirectionalLight* directionalLightData;
    static std::unique_ptr<RootSignature>rootSignature;
    static ModelConfig modelConfig_;
};

