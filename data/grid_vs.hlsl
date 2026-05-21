
struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 ndc : TEXCOORD0;
};

VS_OUT main(uint id : SV_VertexID)
{
    // Full-screen quad: 4 vertices, TRIANGLE_STRIP
    // 0:(-1,+1)  1:(+1,+1)
    // 2:(-1,-1)  3:(+1,-1)
    float2 pos;
    pos.x = (id & 1) ? 1.0 : -1.0;
    pos.y = (id & 2) ? -1.0 : 1.0;

    VS_OUT o;
    o.pos = float4(pos, 0.0, 1.0);
    o.ndc = pos;
    return o;
}
