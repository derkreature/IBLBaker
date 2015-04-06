

struct VSInput
{
    float4 a_position : POSITION;
    float4 a_color0 : COLOR0;
};

struct VSOutput
{
    float4 v_position : SV_POSITION;
    float4 v_color0 : COLOR0;
};

struct PSOutput
{
    float4 output0: SV_TARGET0;
};

float4x4 u_viewProj;

VSOutput vs(VSInput input)
{
    VSOutput output;

    output.v_position = mul(float4(input.a_position.xy, 0.0, 1.0), u_viewProj);
    output.v_color0 = input.a_color0;

    return output;
}

PSOutput ps(VSOutput input)
{
    PSOutput output;
    output.output0 = input.v_color0;
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