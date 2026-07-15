struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct ParticleForGPU
{
    float4x4 WVP;
    float4x4 World;
    float4 color;
};

//GPUParticle
struct ParView
{
    float4x4 viewProjection;
    float4x4 billboardMatrix;
};
struct Particle
{
    float32_t3 translate;
    float32_t lifeTime;
    float32_t3 scale;
    float32_t currentTime;
    float32_t3 velocity;
    float32_t padding;
    float32_t4 color;
};


struct EmitterSphere
{
    float3 translate; 
    float radius;
    uint32_t count;
    float frequency; 
    float frequencyTime;
    uint32_t emit;
};

struct PerFrame
{
    float time;
    float deltaTime;
    float2 padding;
};