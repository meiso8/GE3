
#include "PSO.h"
#include"Log.h"

#include<cassert>

#include"DirectXCommon.h"

std::unique_ptr<RootSignature>PSO::rootSignature = nullptr;
std::array<std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kCountOfCullMode>, kCountOfBlendMode>PSO::graphicsPipelineStates_;
std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kCountOfBlendMode>PSO::graphicsPipelineStatesParticle_;
Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO::graphicsPipelineStatesLine_;
std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kCountOfBlendMode> PSO::graphicsPipelineStateSprite_;
std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kCountOfBlendMode> PSO::graphicsPipelineStateFont_;
Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO::graphicsPipelineStateSkyBox_;
std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, PSO::kCountOfEffect> PSO::graphicsPipelineStateOffScreen_;
std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kCountOfBlendMode> PSO::graphicsPipelineStateRandom_;

std::unordered_map<PSO::PSOKey, Microsoft::WRL::ComPtr<ID3D12PipelineState>, PSO::PSOKeyHasher> PSO::psoCache_;

Microsoft::WRL::ComPtr <ID3D12PipelineState> PSO::Create(
    const BlendMode& blendMode,
    const CullMode& cullMode,
    const MaskMode& maskMode,
    bool useDepthFormat,
    const RootSignature::TYPE& rootSignatureType,
    const DxcCompiler::VS_TYPE& vsShaderType,
    const  DxcCompiler::PS_TYPE& psShaderType,
    const TopologyType topologyType,
    const InputLayout::InputLayoutType inputLayoutType,
    const std::vector<DXGI_FORMAT>& rtvFormats) {

    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc = {};

    graphicsPipelineStateDesc.pRootSignature = rootSignature->GetRootSignature(rootSignatureType);//RootSignature
    graphicsPipelineStateDesc.InputLayout = inputLayout->GetDescs(inputLayoutType);//InputLayout
    graphicsPipelineStateDesc.VS = {
    DirectXCommon::GetDxcCompiler()->GetVertexShaderBlob(vsShaderType)->GetBufferPointer(),
    DirectXCommon::GetDxcCompiler()->GetVertexShaderBlob(vsShaderType)->GetBufferSize() };//VertexShader
    graphicsPipelineStateDesc.PS = { DirectXCommon::GetDxcCompiler()->GetPixelShaderBlob(psShaderType)->GetBufferPointer(),
    DirectXCommon::GetDxcCompiler()->GetPixelShaderBlob(psShaderType)->GetBufferSize() };//PixelShader
    graphicsPipelineStateDesc.BlendState = blendStates[blendMode].GetDesc();//BlendState
    graphicsPipelineStateDesc.RasterizerState = rasterizerStates[cullMode].GetDesc();//RasterizerState

    // レンダーターゲットの数を設定する
    graphicsPipelineStateDesc.NumRenderTargets = uint32_t(rtvFormats.size());

    for (uint32_t i = 0; i < rtvFormats.size(); ++i) {
        //書き込むRTVの情報
        graphicsPipelineStateDesc.RTVFormats[i] = rtvFormats[i];

    }

    //どのように画面に色を打ち込むかの設定（気にしなくていい）
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;


    //DepthStencilの設定   
    graphicsPipelineStateDesc.DepthStencilState = depthStencils[maskMode].GetDesc();
    if (useDepthFormat) {
        graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    } else {
        graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
    }


    switch (topologyType)
    {
    case PSO::kTriangle:
        graphicsPipelineStateDesc.PrimitiveTopologyType =
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        break;
    case PSO::kLine:
        graphicsPipelineStateDesc.PrimitiveTopologyType =
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        break;
    case PSO::kPoint:
        graphicsPipelineStateDesc.PrimitiveTopologyType =
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
        break;
    default:
        break;
    }

    Microsoft::WRL::ComPtr <ID3D12PipelineState>graphicsPipelineState;

    //実際に生成
    HRESULT hr = DirectXCommon::GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
        IID_PPV_ARGS(&graphicsPipelineState));
    assert(SUCCEEDED(hr));

    return graphicsPipelineState;

}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO::CreateComputeShaderPSO(const RootSignature::TYPE& rootSignatureType, const DxcCompiler::CS_TYPE& csShaderType)
{
    //ComputeShaderStateDescを作成する
    D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
    //RootSignatureを代入
    computePipelineStateDesc.pRootSignature = rootSignature->GetRootSignature(rootSignatureType);//RootSignature
    //CSを入れる
    computePipelineStateDesc.CS = {
        .pShaderBytecode = DirectXCommon::GetDxcCompiler()->GetComputeShaderBlob(csShaderType)->GetBufferPointer(),
        .BytecodeLength = DirectXCommon::GetDxcCompiler()->GetVertexShaderBlob(csShaderType)->GetBufferSize() };
    //PSOの作成
    Microsoft::WRL::ComPtr <ID3D12PipelineState>computePipelineState = nullptr;
    //ComputePipelineStateの作成
    HRESULT hr = DirectXCommon::GetDevice()->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&computePipelineState));
    assert(SUCCEEDED(hr));

    return computePipelineState;
}

void PSO::CreateALLPSO()
{
    //RootSignatureを生成する
    rootSignature = std::make_unique<RootSignature>();
    //具体的にShaderがどこかでデータを読めばいいのかの情報を取りまとめたもの
    rootSignature->Create();
    LogFile::Log("CreateRootSignature");

    //InputLayout
    inputLayout = std::make_unique<InputLayout>();
    inputLayout->Create();
    LogFile::Log("InputLayout");

    //BlendStateの設定を行う
    blendStates.resize(kCountOfBlendMode);
    for (int i = 0; i < blendStates.size(); ++i) {
        blendStates[i].Create(i, 0);
        blendStates[i].Create(i, 1);
        blendStates[i].Create(kBlendModeNone, 2);
    }




    //RasterizerStateの設定を行う
    rasterizerStates.resize(kCountOfCullMode);
    rasterizerStates[kCullModeNone].Create(kCullModeNone, kFillModeSolid);//ソリッドモードカリングなし
    rasterizerStates[kCullModeFront].Create(kCullModeFront, kFillModeSolid);//ソリッドモード裏面
    rasterizerStates[kCullModeBack].Create(kCullModeBack, kFillModeSolid);//ソリッドモード表面
    //rasterizerStates[0].Create(kCullModeNone, kFillModeWireframe);//ワイヤーフレームモード
    LogFile::Log("SetRasterizerState");

    //DepthStencilStateの設定
    depthStencils.resize(kMasks);
    depthStencils[kZero].Create(kZero);
    depthStencils[kAll].Create(kAll);
    depthStencils[kNone].Create(kNone);

    LogFile::Log("Create depthStencilDesc");

    std::vector<DXGI_FORMAT> rtvFormatsForTermoAndObjectID =
    {
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        DXGI_FORMAT_R32_UINT
    };

    std::vector<DXGI_FORMAT> rtvFormatsForTermo =
    {
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
    };
    std::vector<DXGI_FORMAT> rtvFormat = { DXGI_FORMAT_R8G8B8A8_UNORM_SRGB };

    //ノーマル
    for (uint32_t b = 0; b < kCountOfBlendMode; ++b) {
        for (uint32_t c = 0; c < kCountOfCullMode; ++c) {
            graphicsPipelineStates_[b][c] = Create(
                static_cast<BlendMode>(b),
                static_cast<CullMode>(c),
                kAll,
                true,
                RootSignature::NORMAL,
                DxcCompiler::VS_Normal,
                DxcCompiler::PS_Normal,
                kTriangle,
                InputLayout::kInputLayoutTypeNormal,
                rtvFormatsForTermoAndObjectID
            );
        }
    }


    //パーティクル
    for (int b = 0; b < kCountOfBlendMode; ++b) {
        graphicsPipelineStatesParticle_[b] = Create(
            static_cast<BlendMode>(b),
            kCullModeNone,
            kZero,
            true,
            RootSignature::PARTICLE,
            DxcCompiler::VS_Particle,
            DxcCompiler::PS_Particle,
            kTriangle,
            InputLayout::kInputLayoutTypeNormal,
            rtvFormatsForTermo
        );
    }

    //SkyBoxはサーモアリ
    graphicsPipelineStateSkyBox_ =
        Create(
            kBlendModeNone,
            kCullModeNone,
            kZero,
            true,
            RootSignature::SKYBOX,
            DxcCompiler::VS_SkyBox,
            DxcCompiler::PS_SkyBox,
            kTriangle,
            InputLayout::kInputLayoutTypeNormal,
            rtvFormatsForTermo);

    //Lineはサーモなし
    graphicsPipelineStatesLine_ =
        Create(
            kBlendModeNone,
            kCullModeBack,
            kAll,
            true,
            RootSignature::LINE,
            DxcCompiler::VS_Line,
            DxcCompiler::PS_Line,
            kLine,
            InputLayout::kInputLayoutTypeNormal,
            rtvFormat
        );

    //スプライトもサーモあり
    for (int b = 0; b < kCountOfBlendMode; ++b) {
        graphicsPipelineStateSprite_[b] = Create(
            static_cast<BlendMode>(b),
            kCullModeBack,
            kNone,
            false,
            RootSignature::SPRITE,
            DxcCompiler::VS_Sprite,
            DxcCompiler::PS_Sprite,
            kTriangle,
            InputLayout::kInputLayoutTypeNormal,
            rtvFormatsForTermo
        );

    }

    //Fontもサーモあり
    for (int b = 0; b < kCountOfBlendMode; ++b) {
        graphicsPipelineStateFont_[b] = Create(
            static_cast<BlendMode>(b),
            kCullModeBack,
            kNone,
            false,
            RootSignature::FONT,
            DxcCompiler::VS_Sprite,
            DxcCompiler::PS_Font,
            kTriangle,
            InputLayout::kInputLayoutTypeNormal,
            rtvFormatsForTermo
        );
    }


    graphicsPipelineStateOffScreen_[kEffectNone] = Create(
        kBlendModeNone,
        kCullModeBack,
        kNone,
        false,
        RootSignature::OFFSCREEN,
        DxcCompiler::VS_OffScreen,
        DxcCompiler::PS_OffScreen,
        kTriangle,
        InputLayout::kInputLayoutTypeOffScreen,
        rtvFormat
    );

    graphicsPipelineStateOffScreen_[kEffectGrayScale] = Create(
        kBlendModeNone,
        kCullModeBack,
        kNone,
        false,
        RootSignature::GRAYSCALE,
        DxcCompiler::VS_OffScreen,
        DxcCompiler::PS_GrayScale,
        kTriangle,
        InputLayout::kInputLayoutTypeOffScreen,
        rtvFormat
    );

    graphicsPipelineStateOffScreen_[kEffectVignette] = Create(
        kBlendModeNone,
        kCullModeBack,
        kNone,
        false,
        RootSignature::VIGNETTE,
        DxcCompiler::VS_OffScreen,
        DxcCompiler::PS_Vignette,
        kTriangle,
        InputLayout::kInputLayoutTypeOffScreen,
        rtvFormat
    );

    graphicsPipelineStateOffScreen_[kEffectBoxFilter] = Create(
        kBlendModeNone,
        kCullModeBack,
        kNone,
        false,
        RootSignature::BOXFILTER,
        DxcCompiler::VS_OffScreen,
        DxcCompiler::PS_BoxFilter,
        kTriangle,
        InputLayout::kInputLayoutTypeOffScreen,
        rtvFormat
    );

    graphicsPipelineStateOffScreen_[kEffectGaussianFilter] = Create(
        kBlendModeNone,
        kCullModeBack,
        kNone,
        false,
        RootSignature::GAUSSIANFILTER,
        DxcCompiler::VS_OffScreen,
        DxcCompiler::PS_GaussianFilter,
        kTriangle,
        InputLayout::kInputLayoutTypeOffScreen,
        rtvFormat
    );


    graphicsPipelineStateOffScreen_[kEffectLuminanceBasedOutline] = Create(
        kBlendModeNone,
        kCullModeBack,
        kNone,
        false,
        RootSignature::LUMINANCE_BASED_OUTLINE,
        DxcCompiler::VS_OffScreen,
        DxcCompiler::PS_LuminanceBasedOutline,
        kTriangle,
        InputLayout::kInputLayoutTypeOffScreen,
        rtvFormat
    );

    graphicsPipelineStateOffScreen_[kEffectDepthBasedOutline] = Create(
        kBlendModeNone,
        kCullModeBack,
        kNone,
        false,
        RootSignature::DEPTH_BASED_OUTLINE,
        DxcCompiler::VS_OffScreen,
        DxcCompiler::PS_DepthBasedOutline,
        kTriangle,
        InputLayout::kInputLayoutTypeOffScreen,
        rtvFormat
    );


    graphicsPipelineStateOffScreen_[kEffectRadialBlur] = Create(
        kBlendModeNone,
        kCullModeBack,
        kNone,
        false,
        RootSignature::RADIAL_BLUR,
        DxcCompiler::VS_OffScreen,
        DxcCompiler::PS_RadialBlur,
        kTriangle,
        InputLayout::kInputLayoutTypeOffScreen,
        rtvFormat
    );

    graphicsPipelineStateOffScreen_[kEffectDissolve] = Create(
        kBlendModeNone,
        kCullModeBack,
        kNone,
        false,
        RootSignature::DISSOLVE,
        DxcCompiler::VS_OffScreen,
        DxcCompiler::PS_Dissolve,
        kTriangle,
        InputLayout::kInputLayoutTypeOffScreen,
        rtvFormat
    );

    //Romdomならコレになります。kEffectRandomは一応あるけど。。エラーになりマスね。
    for (int b = 0; b < kCountOfBlendMode; ++b) {
        graphicsPipelineStateRandom_[b] = Create(
            static_cast<BlendMode>(b),
            kCullModeBack,
            kNone,
            false,
            RootSignature::RANDOM,
            DxcCompiler::VS_OffScreen,
            DxcCompiler::PS_Random,
            kTriangle,
            InputLayout::kInputLayoutTypeOffScreen,
            rtvFormat

        );
    }

    graphicsPipelineStateOffScreen_[kEffectThermography] = Create(
        kBlendModeNone,
        kCullModeBack,
        kNone,
        false,
        RootSignature::THERMOGRAPHY,
        DxcCompiler::VS_OffScreen,
        DxcCompiler::PS_Thermography,
        kTriangle,
        InputLayout::kInputLayoutTypeOffScreen,
        rtvFormat
    );

};

void PSO::Finalize()
{
    for (auto& pso : graphicsPipelineStateSprite_) {
        if (pso) {
            pso.Reset();
        }
    }

    for (auto& pso : graphicsPipelineStateFont_) {
        if (pso) {
            pso.Reset();
        }
    }

    if (graphicsPipelineStatesLine_) {
        graphicsPipelineStatesLine_.Reset();
    }

    for (auto& pso : graphicsPipelineStatesParticle_) {
        if (pso) {
            pso.Reset();
        }
    }

    for (auto& blendModes : graphicsPipelineStates_) {
        for (auto& pso : blendModes) {
            if (pso) {
                pso.Reset(); // Release() と同じ効果
            }
        }
    }

    if (graphicsPipelineStateSkyBox_) {
        graphicsPipelineStateSkyBox_.Reset();
    }


    for (auto& [key, pso] : psoCache_) {
        if (pso) {
            pso.Reset();
        }
    }

    psoCache_.clear();

    rootSignature.reset();

}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO::GetOrCreatePSO(const PSOKey& key)
{
    // 1. キャッシュに存在するかチェック
    auto it = psoCache_.find(key);
    if (it != psoCache_.end()) {
        return it->second; // すでに生成済みならそれを返す
    }

    // 2. 存在しなければ新規作成 (ここで既存の Create 関数を呼ぶ)
    // ※ instance_ は PSOクラスのシングルトンインスタンス等を想定

    assert(GetInstance()->inputLayout);

    std::vector<DXGI_FORMAT> rtvFormatsForTermoAndObjectID =
    {
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        DXGI_FORMAT_R32_UINT
    };
    Microsoft::WRL::ComPtr<ID3D12PipelineState> newPso = GetInstance()->Create(
        static_cast<BlendMode>(key.blendMode),
        static_cast<CullMode>(key.cullMode),
        static_cast<MaskMode>(key.depthMode),
        (key.depthMode != kNone), // Depthフォーマットを使うかどうかの判定例
        key.rootSignatureType,
        key.vsShaderType,
        key.psShaderType,
        key.topologyType,
        key.inputLayoutType,
        rtvFormatsForTermoAndObjectID
    );

    // 3. キャッシュに保存して返す
    psoCache_[key] = newPso;

    return newPso;
}

