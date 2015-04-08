

struct VSInput
{
    float2 a_position : POSITION;
    float2 a_texcoord0 : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 v_texcoord0 : TEXCOORD0;
    float2 v_position : TEXCOORD1;

};

struct PSOutput
{
    float4 output0: SV_TARGET0;
};

float2 u_viewSize;
#define EDGE_AA 1

float4x4 u_scissorMat;
float4x4 u_paintMat;
float4 u_innerCol;
float4 u_outerCol;
float4 u_scissorExtScale;
float4 u_extentRadius;
float4 u_params;
Texture2D s_tex;

SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
    MipLODBias = 0.0f;
};

VSOutput vs(VSInput input)
{
    VSOutput output;
    output.v_position = input.a_position;
    output.v_texcoord0 = input.a_texcoord0;
    output.position = float4(2.0*output.v_position.x / u_viewSize.x - 1.0, 1.0 - 2.0*output.v_position.y / u_viewSize.y, 0.0, 1.0);

    return output;
}

float sdroundrect(float2 pt, float2 ext, float rad)
{
    float2 ext2 = ext - float2(rad,rad);
    float2 d = abs(pt) - ext2;
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0) ) - rad;
}

// Scissoring
float scissorMask(float2 p)
{
    float2 sc = abs(mul((float3x3)transpose(u_scissorMat), float3(p.x, p.y, 1.0)).xy) - u_scissorExtScale.xy;
    sc = float2(0.5, 0.5) - sc * u_scissorExtScale.zw;
    return clamp(sc.x, 0.0, 1.0) * clamp(sc.y, 0.0, 1.0);
}

// Stroke - from [0..1] to clipped pyramid, where the slope is 1px.
float strokeMask(float2 _texcoord)
{
    return min(1.0, (1.0 - abs(_texcoord.x*2.0 - 1.0) )*u_params.y) * min(1.0, _texcoord.y);
}

PSOutput ps(VSOutput input)
{
    PSOutput output;

    float4 result = float4(1,0,0,0);
    float scissor = scissorMask(input.v_position);
    float strokeAlpha = strokeMask(input.v_texcoord0);

    if (u_params.w == 0.0) // Gradient
    {
        // Calculate gradient color using box gradient
        float2 pt = mul((float3x3)transpose(u_paintMat), float3(input.v_position, 1.0)).xy;

        float d = clamp( (sdroundrect(pt, u_extentRadius.xy, u_extentRadius.z) + u_params.x*0.5) / u_params.x, 0.0, 1.0);
        float4 color = lerp(u_innerCol, u_outerCol, d);
        // Combine alpha
        color.w *= strokeAlpha * scissor;
        result = color;
    }
    else if (u_params.w == 1.0) // Image
    {
        // Calculate color from texture
        float2 pt = mul((float3x3)transpose(u_paintMat), float3(input.v_position.x, input.v_position.y, 1.0)).xy / u_extentRadius.xy;
        float4 color = s_tex.Sample(linearSampler, pt); 
        color = u_params.z == 0.0 ? color : float4(1.0, 1.0, 1.0, color.x);
        // Combine alpha
        color.w *= strokeAlpha * scissor;
        result = color;
    }
    else if (u_params.w == 2.0) // Stencil fill
    {
        result = float4(1.0, 1.0, 1.0, 1.0);
    }
    else if (u_params.w == 3.0) // Textured tris
    {
        float4 color = s_tex.Sample(linearSampler, input.v_texcoord0.xy);
        color = u_params.z == 0.0 ? color : float4(1.0, 1.0, 1.0, color.x);
        color.w *= scissor;
        result = color * u_innerCol;
    }

    output.output0 = result;
    return output;
}



technique11 basic
{
    pass p0
    {
        SetVertexShader(CompileShader (vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader (ps_5_0, ps()));
    }
}