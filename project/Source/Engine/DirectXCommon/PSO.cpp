
#include "PSO.h"

#include<cassert>

#include"DirectXCommon.h"

void PSO::Create(
    RootSignature& rootSignature,
    InputLayout& inputLayout,
    BlendState& blendState,
    RasterizerState& rasterizerState,
    DepthStencil& depthStencil) {

    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc[PSO_MAX]{};

    for (int i = 0; i < PSO_MAX - 1; ++i) {

        graphicsPipelineStateDesc[i].pRootSignature = rootSignature.GetRootSignature(0);//RootSignature
        graphicsPipelineStateDesc[i].InputLayout = inputLayout.GetDesc();//InputLayout
        graphicsPipelineStateDesc[i].VS = { DirectXCommon::GetDxcCompiler()->GetVertexShaderBlob(0)->GetBufferPointer(),
      DirectXCommon::GetDxcCompiler()->GetVertexShaderBlob(0)->GetBufferSize() };//VertexShader
        graphicsPipelineStateDesc[i].PS = { DirectXCommon::GetDxcCompiler()->GetPixelShaderBlob(0)->GetBufferPointer(),
       DirectXCommon::GetDxcCompiler()->GetPixelShaderBlob(0)->GetBufferSize() };//PixelShader
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


    for (int i = PARTICLE; i < PSO_MAX; ++i) {

        graphicsPipelineStateDesc[i].pRootSignature = rootSignature.GetRootSignature(1);//RootSignature
        graphicsPipelineStateDesc[i].InputLayout = inputLayout.GetDesc();//InputLayout
        graphicsPipelineStateDesc[i].VS = { DirectXCommon::GetDxcCompiler()->GetVertexShaderBlob(1)->GetBufferPointer(),
      DirectXCommon::GetDxcCompiler()->GetVertexShaderBlob(1)->GetBufferSize() };//VertexShader
        graphicsPipelineStateDesc[i].PS = { DirectXCommon::GetDxcCompiler()->GetPixelShaderBlob(1)->GetBufferPointer(),
       DirectXCommon::GetDxcCompiler()->GetPixelShaderBlob(1)->GetBufferSize() };//PixelShader
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


    //利用するトポロジ（形状）のタイプ。
    graphicsPipelineStateDesc[TRIANGLE].PrimitiveTopologyType =
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    graphicsPipelineStateDesc[LINE].PrimitiveTopologyType =
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

    graphicsPipelineStateDesc[POINT].PrimitiveTopologyType =
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    graphicsPipelineStateDesc[PARTICLE].PrimitiveTopologyType =
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    for (int i = 0; i < PSO_MAX; ++i) {

        //実際に生成
        HRESULT hr = DirectXCommon::GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc[i],
            IID_PPV_ARGS(&graphicsPipelineState_[i]));
        assert(SUCCEEDED(hr));
    }
}

