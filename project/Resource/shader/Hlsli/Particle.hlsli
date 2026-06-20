struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

struct ParticleForGPU
{
    float32_t4x4 WVP;
    float32_t4x4 World;
    float4 color;
};