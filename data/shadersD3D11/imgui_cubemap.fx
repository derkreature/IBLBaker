

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

float2 u_imageLodEnabled;
float4x4 u_viewProj;

TextureCube s_tex;

SamplerState linearSampler;
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = REPEAT;
    AddressV = REPEAT;
    MipLODBias = 0.0f;
};

void vs(VSInput input)
{
    VSOutput output;

    output.v_position = mul(u_viewProj, float4(input.a_position.xyz, 1.0));
    output.v_normal = input.a_normal.xyz;

    return output;
}

PSOutput ps(VSOutput psInput)
{
    PSOutput output;

    uniform vec4 u_imageLodEnabled;
    SAMPLERCUBE(s_texColor, 0);

    float3 color = s_tex.SampleLevel(linearSampler, v_normal, u_imageLodEnabled.x).xyz;
    float alpha = 0.2 + 0.8*u_imageLodEnabled.y;

    output.output0 = vec4(color, alpha);
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