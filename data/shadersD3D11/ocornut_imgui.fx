

struct VSInput
{
    float4 a_position : POSITION;
    float2 a_texcoord0 : TEXCOORD0;
    float4 a_color0 : COLOR0;
};

struct VSOutput
{
    float4 v_position : SV_POSITION;
    float2 v_texcoord0 : TEXCOORD0;
    float4 v_color0 : COLOR0;
};

struct PSOutput
{
    float4 output0: SV_TARGET0;
};

Texture2D s_tex;

float4 u_viewTexel;

SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
    MipLODBias = 0.0f;
};

VSOutput vs(VSInput input)
{
    VSOutput output;

    float2 pos = 2.0*input.a_position.xy*u_viewTexel.xy;
    output.v_position = float4(pos.x - 1.0, 1.0- pos.y, 0.0, 1.0);
    output.v_texcoord0 = input.a_texcoord0;
    output.v_color0 = input.a_color0;

    return output;
}

PSOutput ps(VSOutput input)
{
    PSOutput output;
    float4 texel = s_tex.Sample(linearSampler, input.v_texcoord0.xy);
    output.output0 = texel * input.v_color0;
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