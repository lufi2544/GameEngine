
cbuffer GridBuffer : register(b0)
{
    float4x4 inv_vp;
    float4x4 vp;
    float3   cam_pos;
    float    _pad0;
    float3   cam_target;
    float    _pad1;
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

    // Anti-aliased floor tiles using screen-space derivatives.
    float2 coord = hit.xz;
    float2 deriv = fwidth(coord);

    float2 tile_coord = coord * 0.5;
    float2 tile_deriv = fwidth(tile_coord);
    float2 tile_grid = abs(frac(tile_coord - 0.5) - 0.5) / tile_deriv;
    float  tile_line = 1.0 - min(min(tile_grid.x, tile_grid.y), 1.0);

    float2 block_coord = coord * 0.125;
    float2 block_deriv = fwidth(block_coord);
    float2 block_grid = abs(frac(block_coord - 0.5) - 0.5) / block_deriv;
    float  block_line = 1.0 - min(min(block_grid.x, block_grid.y), 1.0);

    // Distance fade
    float dist = length(hit.xz - cam_target.xz);
    float fade = 1.0 - saturate(dist / 95.0);
    fade = lerp(0.70, 1.0, fade);

    float checker = frac(floor(coord.x * 0.5) + floor(coord.y * 0.5)) * 0.008;

    float3 base_color = float3(0.22, 0.22, 0.22) + checker;
    float small_line = tile_line * 0.62;
    float large_line = block_line * 0.74;
    float line_alpha = max(small_line, large_line);

    float3 color = base_color;
    color = lerp(color, float3(0.006, 0.006, 0.006), small_line);
    color = lerp(color, float3(0.002, 0.002, 0.002), large_line);
    float final_alpha = saturate(0.82 * fade + line_alpha * fade);

    // Axis lines
    float axis_x = 1.0 - min(abs(hit.z) / (deriv.y * 2.0), 1.0);
    float axis_z = 1.0 - min(abs(hit.x) / (deriv.x * 2.0), 1.0);
    if (axis_x > 0.0) { color = lerp(color, float3(0.9, 0.15, 0.15), axis_x); final_alpha = max(final_alpha, axis_x * fade); }
    if (axis_z > 0.0) { color = lerp(color, float3(0.15, 0.4, 0.9),  axis_z); final_alpha = max(final_alpha, axis_z * fade); }

    clip(final_alpha - 0.01);

    // Write correct depth so the grid is occluded by geometry
    float4 clip_pos = mul(vp, float4(hit, 1.0));
    o.depth = clip_pos.z / clip_pos.w;

    o.color = float4(color, final_alpha);
    return o;
}
