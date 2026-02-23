
cbuffer DebugBuffer : register(b1)
{
    float4 debugColor;
    int useDebugColor;
};

Texture2D Texture0 : register(t0);
SamplerState Sampler0 : register(s0);

struct PS_IN
{
    float4 pos   : SV_POSITION;
    float4 color : COLOR;
    float2 uv    : TEXCOORD0;
};

float4 main(PS_IN input) : SV_TARGET
{
    if(useDebugColor != 0)
    {	
        return debugColor;
    }
    else
    {
        return Texture0.Sample(Sampler0, input.uv);
    }
}
