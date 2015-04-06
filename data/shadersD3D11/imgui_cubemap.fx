

struct VSInput
{
    float4 a_position : POSITION;
    float3 a_normal : NORMAL;
};

struct VSOutput
{
    float4 v_position : SV_POSITION;
    float3 v_normal : TEXCOORD0;
};

struct PSOutput
{
    float4 output0: SV_TARGET0;
};

float4 u_imageLodEnabled;
float4x4 u_viewProj;

TextureCube s_tex;

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
    output.v_position = mul(float4(input.a_position.xyz, 1.0), u_viewProj);
    output.v_normal = input.a_normal.xyz;

    return output;
}

PSOutput ps(VSOutput input)
{
    PSOutput output;

    float3 color = s_tex.SampleLevel(linearSampler, input.v_normal, u_imageLodEnabled.x).xyz;
    float alpha = 0.2 + 0.8*u_imageLodEnabled.y;
    output.output0 = float4(color.xyz, alpha);

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