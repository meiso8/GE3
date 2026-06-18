struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2  texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : POSITION0;
};

struct ObjectID
{
    uint id;
    uint padding1;
    uint padding2;
    uint padding3;
    
};

