
struct VSInput
{
    float4 a_position : POSITION;
    float2 a_texcoord0 : TEXCOORD0;
};

struct VSOutput
{
    float4 v_position : SV_POSITION;
    float2 v_texcoord0 : TEXCOORD0;
};

struct PSOutput
{
    float4 output0: SV_TARGET0;
};

Texture2D s_tex;

SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
    MipLODBias = 0.0f;
};

float4x4 u_viewProj;

VSOutput vs(VSInput input)
{
    VSOutput output;

    output.v_position = mul(float4(input.a_position.xy, 0.0, 1.0), u_viewProj);
    output.v_texcoord0 = input.a_texcoord0;

    return output;
}

float4 u_imageLodEnabled;



PSOutput ps(VSOutput input)
{
    PSOutput output;

    float3 texel = s_tex.SampleLevel(linearSampler, input.v_texcoord0, u_imageLodEnabled.x).xyz;
    float alpha = 0.2 + 0.8*u_imageLodEnabled.y;
    output.output0 = float4(texel.x, texel.y, texel.z, alpha);

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