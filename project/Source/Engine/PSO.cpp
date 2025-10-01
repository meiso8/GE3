
#include "PSO.h"

#include<cassert>


void PSO::Create(
    RootSignature& rootSignature,
    InputLayout& inputLayout,
    DxcCompiler& dxcCompiler,
    BlendState& blendState,
    RasterizerState& rasterizerState,
    DepthStencil& depthStencil,
    const Microsoft::WRL::ComPtr<ID3D12Device>& device) {

    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc[PSO_MAX]{};

    for (int i = 0; i < PSO_MAX-1; ++i) {

        graphicsPipelineStateDesc[i].pRootSignature = rootSignature.GetRootSignature(0).Get();//RootSignature
        graphicsPipelineStateDesc[i].InputLayout = inputLayout.GetDesc(0);//InputLayout
        graphicsPipelineStateDesc[i].VS = { dxcCompiler.GetVertexShaderBlob()->GetBufferPointer(),
       dxcCompiler.GetVertexShaderBlob()->GetBufferSize() };//VertexShader
        graphicsPipelineStateDesc[i].PS = { dxcCompiler.GetPixelShaderBlob(DxcCompiler::NORMAL)->GetBufferPointer(),
       dxcCompiler.GetPixelShaderBlob(DxcCompiler::NORMAL)->GetBufferSize() };//PixelShader
        graphicsPipelineStateDesc[i].BlendState = blendState.GetDesc();//BlendState
        graphicsPipelineStateDesc[i].RasterizerState = rasterizerState.GetDesc();//RasterizerState
        //書き込むRTVの情報
        graphicsPipelineStateDesc[i].NumRenderTargets = 1;
        graphicsPipelineStateDesc[i].RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

        //どのように画面に色を打ち込むかの設定（気にしなくていい）
        graphicsPipelineStateDesc[i].SampleDesc.Count = 1;
        graphicsPipelineStateDesc[i].SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
        //DepthStencilの設定   
        graphicsPipelineStateDesc[i].DepthStencilState = depthStencil.GetDesc();
        graphicsPipelineStateDesc[i].DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    }


        graphicsPipelineStateDesc[NONE_TEX].pRootSignature = rootSignature.GetRootSignature(1).Get();//RootSignature
        graphicsPipelineStateDesc[NONE_TEX].InputLayout = inputLayout.GetDesc(1);//InputLayout
        graphicsPipelineStateDesc[NONE_TEX].VS = { dxcCompiler.GetVertexShaderBlob()->GetBufferPointer(),
       dxcCompiler.GetVertexShaderBlob()->GetBufferSize() };//VertexShader
        graphicsPipelineStateDesc[NONE_TEX].PS = { dxcCompiler.GetPixelShaderBlob(DxcCompiler::NONE_TEX)->GetBufferPointer(),
       dxcCompiler.GetPixelShaderBlob(DxcCompiler::NONE_TEX)->GetBufferSize() };//PixelShader
        graphicsPipelineStateDesc[NONE_TEX].BlendState = blendState.GetDesc();//BlendState
        graphicsPipelineStateDesc[NONE_TEX].RasterizerState = rasterizerState.GetDesc();//RasterizerState
        //書き込むRTVの情報
        graphicsPipelineStateDesc[NONE_TEX].NumRenderTargets = 1;
        graphicsPipelineStateDesc[NONE_TEX].RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

        //どのように画面に色を打ち込むかの設定（気にしなくていい）
        graphicsPipelineStateDesc[NONE_TEX].SampleDesc.Count = 1;
        graphicsPipelineStateDesc[NONE_TEX].SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
        //DepthStencilの設定   
        graphicsPipelineStateDesc[NONE_TEX].DepthStencilState = depthStencil.GetDesc();
        graphicsPipelineStateDesc[NONE_TEX].DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


    //利用するトポロジ（形状）のタイプ。
    graphicsPipelineStateDesc[TRIANGLE].PrimitiveTopologyType =
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    graphicsPipelineStateDesc[LINE].PrimitiveTopologyType =
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

    graphicsPipelineStateDesc[POINT].PrimitiveTopologyType =
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

    graphicsPipelineStateDesc[NONE_TEX].PrimitiveTopologyType =
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    for (int i = 0; i < PSO_MAX-1; ++i) {

        //実際に生成
        HRESULT hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc[i],
            IID_PPV_ARGS(&graphicsPipelineState_[i]));
        assert(SUCCEEDED(hr));
    }
}

