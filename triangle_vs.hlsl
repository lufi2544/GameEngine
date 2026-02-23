
cbuffer MVPBuffer : register(b0)
{
    float4x4 mvp;
};

struct VS_IN
{
    float3 pos   : POSITION;
    float4 color : COLOR;
    float2 uv    : TEXCOORD0;
};

struct VS_OUT
{
    float4 pos   : SV_POSITION;
    float4 color : COLOR;
    float2 uv    : TEXCOORD0;
};

VS_OUT main(VS_IN input)
{
    VS_OUT o;
    o.pos   = mul(mvp, float4(input.pos, 1.0f));
    o.color = input.color;
    o.uv    = input.uv;
    return o;
}
