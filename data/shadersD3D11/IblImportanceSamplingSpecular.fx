//------------------------------------------------------------------------------------//
//                                                                                    //
//    ._____________.____   __________         __                                     //
//    |   \______   \    |  \______   \_____  |  | __ ___________                     //
//    |   ||    |  _/    |   |    |  _/\__  \ |  |/ // __ \_  __ \                    //
//    |   ||    |   \    |___|    |   \ / __ \|    <\  ___/|  | \/                    //
//    |___||______  /_______ \______  /(____  /__|_ \\___  >__|                       //
//                \/        \/      \/      \/     \/    \/                           //
//                                                                                    //
//    IBLBaker is provided under the MIT License(MIT)                                 //
//    IBLBaker uses portions of other open source software.                           //
//    Please review the LICENSE file for further details.                             //
//                                                                                    //
//    Copyright(c) 2014 Matt Davidson                                                 //
//                                                                                    //
//    Permission is hereby granted, free of charge, to any person obtaining a copy    //
//    of this software and associated documentation files(the "Software"), to deal    //
//    in the Software without restriction, including without limitation the rights    //
//    to use, copy, modify, merge, publish, distribute, sublicense, and / or sell     //
//    copies of the Software, and to permit persons to whom the Software is           //
//    furnished to do so, subject to the following conditions :                       //
//                                                                                    //
//    1. Redistributions of source code must retain the above copyright notice,       //
//    this list of conditions and the following disclaimer.                           //
//    2. Redistributions in binary form must reproduce the above copyright notice,    //
//    this list of conditions and the following disclaimer in the                     //
//    documentation and / or other materials provided with the distribution.          //
//    3. Neither the name of the copyright holder nor the names of its                //
//    contributors may be used to endorse or promote products derived                 //
//    from this software without specific prior written permission.                   //
//                                                                                    //
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      //
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        //
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE      //
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          //
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   //
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN       //
//    THE SOFTWARE.                                                                   //
//                                                                                    //
//------------------------------------------------------------------------------------//

float ShaderName : SHADERNAME 
<
    string ToString = "ImportanceSamplingSpecular";
> = 0.0;

float4x4 mWorldViewProj : WORLDVIEWPROJECTION;
float4x4 mWorld : WORLD;
float4x4 mView : VIEW;
float4x4 mProj : PROJECTION;
float3 vEye : EYELOCATION;
float4 IBLCorrection : IBLCORRECTION;

TextureCube ConvolutionSrc : CONVOLUTIONSRC;
TextureCube LastResult : LASTRESULT;

float4x4 ConvolutionViews[6] : CUBEVIEWS; 

float ConvolutionSamplesOffset = 0;
float ConvolutionSampleCount = 0;
float ConvolutionMaxSamples = 0;
float ConvolutionRoughness = 0;
float ConvolutionMip = 0;

float EnvironmentScale : IBLSOURCEENVIRONMENTSCALE;
float MaxLod : IBLSOURCEMIPCOUNT;
float4 IblMaxValue : IBLMAXVALUE;

SamplerState EnvMapSampler
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 16;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VS_CUBEMAP_IN
{
    float4 Pos      : POSITION;
    float3 Normal   : NORMAL;
    float2 Tex      : TEXCOORD0;
};

struct GS_CUBEMAP_IN
{
    float4 Pos      : SV_POSITION;
    float3 Normal   : NORMAL;
    float2 Tex      : TEXCOORD0;
};

struct PS_CUBEMAP_IN
{
    float4 Pos : SV_POSITION;  
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD0; 
    uint RTIndex : SV_RenderTargetArrayIndex;
};

GS_CUBEMAP_IN VS_CubeMap( VS_CUBEMAP_IN input )
{
    GS_CUBEMAP_IN output = (GS_CUBEMAP_IN)0.0f;
    output.Pos = mul( input.Pos, mWorld );
    output.Normal = normalize(mul(input.Normal, (float3x3)(mWorld)).xyz);
    output.Tex = input.Tex;
    return output;
}

[maxvertexcount(18)]
void GS_CubeMap( triangle GS_CUBEMAP_IN input[3], inout TriangleStream<PS_CUBEMAP_IN> CubeMapStream )
{
    for (int face = 0; face < 6; face++)
    {
        PS_CUBEMAP_IN output;
        output.RTIndex = face;
        for( int v = 0; v < 3; v++ )
        {
            output.Normal = (input[v].Normal.xyz);
            output.Pos = mul(input[v].Pos, ConvolutionViews[face]);
            output.Pos = mul( output.Pos, mProj );
            output.Tex = input[v].Tex;
            CubeMapStream.Append( output );
        }
        CubeMapStream.RestartStrip();
    }
}

//
// Attributed to:
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// Holger Dammertz.
// 

float2 Hammersley(uint i, uint N) 
{
    float ri = reversebits(i) * 2.3283064365386963e-10f;
    return float2(float(i) / float(N), ri);
}

float3x3 QuaternionToMatrix(float4 quat)
{
    float3 cross = quat.yzx * quat.zxy;
    float3 square= quat.xyz * quat.xyz;
    float3 wi = quat.w * quat.xyz;

    square = square.xyz + square.yzx;

    float3 diag = 0.5 - square;
    float3 a = (cross + wi);
    float3 b = (cross - wi);

    return float3x3(
    2.0 * float3(diag.x, b.z, a.y),
    2.0 * float3(a.z, diag.y, b.x),
    2.0 * float3(b.y, a.x, diag.z));
}


float3 rescaleHDR(float3 hdrPixel)
{
   if (hdrPixel.x < 0)
    hdrPixel.x = 0;
   if (hdrPixel.y < 0)
    hdrPixel.y = 0;
   if (hdrPixel.z < 0)
    hdrPixel.z = 0;



   float intensity  = float(dot(hdrPixel, float3(0.299,0.587,0.114)));
#if 0 
   if (intensity > 1)
   {
  
       hdrPixel = hdrPixel - IBLCorrection.x * (hdrPixel - IblMaxValue.rgb) * hdrPixel * (hdrPixel - (IblMaxValue.rgb * 0.5));
   }
   else
   {
        hdrPixel = hdrPixel - IBLCorrection.x * (hdrPixel - IblMaxValue.rgb) * hdrPixel * (hdrPixel - (0.5));
   }
#endif

   // Saturation adjustment
   hdrPixel = lerp(intensity.xxx, hdrPixel, IBLCorrection.y);

   // Hue adjustment      
   const float3 root3 = float3(0.57735, 0.57735, 0.57735);
   float half_angle = 0.5 * radians(IBLCorrection.z); // Hue is radians of 0 tp 360 degree
   float4 rot_quat = float4( (root3 * sin(half_angle)), cos(half_angle));
   float3x3 rot_Matrix = QuaternionToMatrix(rot_quat);     
   hdrPixel = mul(rot_Matrix, hdrPixel);

   hdrPixel = hdrPixel * EnvironmentScale;
   return hdrPixel;
}


float3 ImportanceSample (float3 R )
{
    float3 N = R;
    float3 V = R;
    float4 result = float4(0,0,0,0);

    float SampleStep = (ConvolutionMaxSamples / ConvolutionSampleCount);
    uint sampleId = ConvolutionSamplesOffset;

    uint cubeWidth, cubeHeight;
    ConvolutionSrc.GetDimensions(cubeWidth, cubeHeight);

    for(uint i = 0; i < ConvolutionSampleCount; i++ )
    {
        float2 Xi = Hammersley(sampleId, ConvolutionMaxSamples);

        float3 H = importanceSampleGGX( Xi, ConvolutionRoughness, N);
        float3 L = 2 * dot( V, H ) * H - V;
        float NoL = max(dot( N, L ), 0);
        float VoL = max (dot(V, L), 0);
        float NoH = max(dot( N, H ), 0);
        float VoH = max(dot( V, H ), 0);
        if( NoL > 0 )
        {
            //
            // Compute pdf of BRDF
            // Taken from Epic's Siggraph 2013 Lecture:
            // http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
            //
            float Dh = specularD(ConvolutionRoughness, NoH);
            float pdf = Dh * NoH / (4*VoH);
            float solidAngleTexel = 4 * PI / (6 * cubeWidth * cubeWidth);
            float solidAngleSample = 1.0 / (ConvolutionSampleCount * pdf);
            float lod = ConvolutionRoughness == 0 ? 0 : 0.5 * log2((float)(solidAngleSample/solidAngleTexel));

            float3 hdrPixel = rescaleHDR(ConvolutionSrc.SampleLevel(EnvMapSampler, L, lod).rgb);
            result = sumSpecular(hdrPixel, NoL, result);
        }
        sampleId += SampleStep;
   }

    if (result.w == 0)
        return result.xyz;
   else
       return (result.xyz / result.w);
}

float4 PS_CubeMap( PS_CUBEMAP_IN input ) : SV_Target
{
    float3 R = normalize(input.Normal);
    float4 sampledColor = float4(0,0,0,1);

    // Sample source cubemap at specified mip.
    float3 importanceSampled = ImportanceSample(R);

    if (ConvolutionSamplesOffset >= 1)
    {
        float3 lastResult = LastResult.SampleLevel(EnvMapSampler, R, ConvolutionMip).rgb;
        sampledColor.rgb = lerp(lastResult.xyz, importanceSampled.xyz, 1.0 / (ConvolutionSamplesOffset));
    }
    else
    {
        sampledColor.xyz = importanceSampled.xyz;
    }

    return sampledColor;
}

technique11 basic
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_CubeMap() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS_CubeMap() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_CubeMap() ) );
    }
};

