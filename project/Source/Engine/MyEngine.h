#pragma once

#pragma region //自作関数
//#include"Window.h"
#include"Input.h"

#include"DirectXCommon.h"
#include "RootSignature.h"

#include"TransitionBarrier.h"

#include"D3DResourceLeakChecker.h"
#include"Depth.h"//StencilTextureの作成関数　奥行き
#include"CompileShader.h"
#include"BlendState.h"
#include"RasterizerState.h"
#include"PSO.h"
#include"ViewPort.h"
#include"ScissorRect.h"
#include"Texture.h"

#include"ShaderResourceView.h"
#include"Model.h"
#include"Sprite.h"
#include"SphereMesh.h"
#include"LineMesh.h"
#include"Sound.h"

#include"Camera/DebugCamera.h"
#include"Camera/Camera.h"
#include"CrashHandler.h"
#include"Log.h"
#include"ImGuiClass.h"
#include"InputLayout.h"


#include"FPSCounter.h"

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
#include"Cube.h"

#include"DebugUI.h"

#pragma endregion



class MyEngine {

public:

public:
    void Create(const std::wstring& title, const int32_t clientWidth, const int32_t clientHeight);
    void Update();
    void PreCommandSet(Vector4& color);
    void PostCommandSet();
    void End();
    Window& GetWC() { return wc; };
    ModelConfig& GetModelConfig() { return modelConfig_; };
    PSO& GetPSO(uint32_t index) { return pso[index]; }
    DirectionalLight& GetDirectionalLightData() { return *directionalLightData; }
    void SetBlendMode(uint32_t blendMode =BlendMode::kBlendModeNormal);
private:
    std::unique_ptr<DirectXCommon> directXCommon = nullptr;
    D3DResourceLeakChecker leakCheck = {};
    LogFile logFile = {};
    Window wc = {};
    Input* input = { nullptr };

    InputLayout inputLayout = {};
    std::vector<BlendState> blendStates = {};
    std::vector<RasterizerState> rasterizerStates = {};
    DepthStencil depthStencil = {};
    PSO pso[kCountOfBlendMode] = {};
    Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightResource = nullptr;
    DirectionalLight* directionalLightData = nullptr;
    RootSignature rootSignature = {};
    ModelConfig modelConfig_ = {};
};

