
cbuffer GridBuffer : register(b0)
{
    float4x4 inv_vp;
    float4x4 vp;
    float3   cam_pos;
    float    _pad;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 ndc : TEXCOORD0;
};

struct PS_OUT
{
    float4 color : SV_TARGET;
    float  depth : SV_DEPTH;
};

PS_OUT main(PS_IN input)
{
    PS_OUT o;

    // Unproject NDC -> world space (near and far points)
    float4 near_h = mul(inv_vp, float4(input.ndc, 0.0, 1.0));
    float4 far_h  = mul(inv_vp, float4(input.ndc, 1.0, 1.0));
    float3 near_w = near_h.xyz / near_h.w;
    float3 far_w  = far_h.xyz  / far_h.w;

    // Ray from near to far
    float3 dir = far_w - near_w;

    // Intersect Y=0 plane: near_w.y + t * dir.y = 0
    float t = -near_w.y / dir.y;

    // Discard rays that don't hit the plane in front of the camera
    clip(t - 0.0001);

    float3 hit = near_w + t * dir;

    // Anti-aliased grid lines using screen-space derivatives
    float2 coord = hit.xz;
    float2 deriv = fwidth(coord);
    float2 grid  = abs(frac(coord - 0.5) - 0.5) / deriv;
    float  nearest = min(grid.x, grid.y);
    float  alpha = 1.0 - min(nearest, 1.0);

    clip(alpha - 0.01);

    // Distance fade
    float dist = length(hit.xz - cam_pos.xz);
    float fade = 1.0 - saturate(dist / 50.0);

    clip(fade - 0.01);

    // Axis coloring: X axis = red, Z axis = blue
    float3 color = float3(0.45, 0.45, 0.45);
    if (abs(hit.z) < deriv.y * 1.5) color = float3(0.9, 0.2, 0.2);
    if (abs(hit.x) < deriv.x * 1.5) color = float3(0.2, 0.2, 0.9);

    // Write correct depth so the grid is occluded by geometry
    float4 clip_pos = mul(vp, float4(hit, 1.0));
    o.depth = clip_pos.z / clip_pos.w;

    o.color = float4(color, alpha * fade);
    return o;
}
