

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

SamplerState linearSampler;
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = REPEAT;
    AddressV = REPEAT;
    MipLODBias = 0.0f;
};

void vs(VSInput vsInput)
{
    VSOutput output;

    return output;
}

PSOutput ps(VSOutput psInput)
{
    PSOutput output;

    float4 texel = s_tex.Sample(linearSampler, psInput.v_texcoord0.xy);
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