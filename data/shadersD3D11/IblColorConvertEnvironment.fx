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

float Script : STANDARDSGLOBAL 
<
    string UIWidget = "none";
    string ScriptClass = "object";
    string ScriptOrder = "standard";
    string ScriptOutput = "color";
    string ToString = "ColorConvertEnvironment";
> = 0.0;

float4x4 worldMatrix : WORLD;
float4x4 worldViewProjection : WORLDVIEWPROJECTION;
float4x4 viewProjection : VIEWPROJECTION;
float4x4 projectionMatrix : PROJECTION;
float4x4 viewInverse : VIEWINVERSE;
float4 eyeLocation : EYELOCATION;
float4 materialDiffuse : MATERIALDIFFUSE;
float4 specularFresnel : SPECULARFRESNEL;
TextureCube diffuseMap : DIFFUSEMAP;
float cameraZNear : CAMERAZNEAR;
float cameraZFar : CAMERAZFAR;
float textureGamma : TEXTUREGAMMA = 2.2;
float4x4 g_mViewCM[6] : CUBEVIEWS; 

float CurrentMipLevel = 0;
float IsMDR = 0;
float Gamma = 2.2;
float LDRExposure = 1.6;
float MDRScale = 5.0;


float4 RGBMEncode( float3 color ) {
  float4 rgbm;
  color *= 1.0 / 5.0;
  rgbm.a = saturate( max( max( color.r, color.g ), max( color.b, 1e-6 ) ) );
  rgbm.a = ceil( rgbm.a * 255.0 ) / 255.0;
  rgbm.rgb = color / rgbm.a;
  return rgbm;
}

// Tonemapping operators thanks to notes from Filmic games.
// http://filmicgames.com/archives/75

const float A = 0.15;
const float B = 0.50;
const float C = 0.10;
const float D = 0.20;
const float E = 0.02;
const float F = 0.30;
const float W = 11.2;

float3 uncharted2Tonemap(float3 x)
{
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}
 
float3 applyUnchartedToneMap( float3 texColor)
{
   // Hardcoded Exposure Adjustment
   //texColor *= 16; 
    
   //float ExposureBias = 2.0f;
   float3 curr = uncharted2Tonemap(4.0*texColor);
    
   float3 whiteScale = 1.0f/uncharted2Tonemap(W);
   return curr*whiteScale;   
}


SamplerState anisotropicSampler
{
    Filter =     MIN_MAG_MIP_LINEAR;
    AddressU  = Wrap;
    AddressV  = Wrap;
    BorderColor = float4(10000,10000,10000,10000);
};

struct VertexShaderIn
{
    float4 position   : POSITION;
    float3 normal     : NORMAL;
};


struct VertexShaderOut
{
    float4 position : SV_POSITION;  
    float3 normal : NORMAL;
};

struct PixelShaderInput
{
    float4 position : SV_POSITION;  
    float3 normal : NORMAL;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

struct PixelShaderOut
{
    float4 output0: SV_TARGET0;
};

VertexShaderOut vs (VertexShaderIn vertexShaderIn)
{
    VertexShaderOut output;
    output.position = mul(vertexShaderIn.position, worldMatrix);
    output.normal = normalize(mul(vertexShaderIn.normal, (float3x3)worldMatrix));    

    return output;
}

[maxvertexcount(18)]
void gs (triangle VertexShaderOut input[3], 
         inout TriangleStream<PixelShaderInput> outStream)
{
    for( int f = 0; f < 6; ++f )
    {
        // Compute screen coordinates
        PixelShaderInput output;
        output.RTIndex = f;
        for( int v = 0; v < 3; v++ )
        {
            output.normal = (input[v].normal.xyz);
            output.position = mul( input[v].position, g_mViewCM[f] );
            output.position = mul( output.position, projectionMatrix );
            outStream.Append( output );
        }
        outStream.RestartStrip();
    }
}

PixelShaderOut ps (PixelShaderInput vertexShaderOut) 
{
    float4 litColor = float4(1,1,1,1);
    PixelShaderOut output;

    float3 envDiffuseCoord = vertexShaderOut.normal;
    litColor = diffuseMap.SampleLevel(anisotropicSampler, vertexShaderOut.normal.xyz, CurrentMipLevel);

    // Tone map if LDR.
    if (IsMDR < 1e-6)
    {
	litColor.rgb *= 1.8; 
	float l = litColor.r * 0.2126 + litColor.g * 0.72152 + litColor.b * 0.0722;
	litColor.rgb = saturate( litColor.rgb / (1.0 + l) );

    }

    // Gamma correct, in both cases
    litColor.rgb = pow(litColor.rgb, 1.0f/2.2f);

    // Apply RGBM if requested.
    // From notes: 
    // http://graphicrants.blogspot.com/2009/04/rgbm-color-encoding.html
    if (IsMDR > 1e-6)
    {
       litColor = RGBMEncode(litColor.rgb);
    }

    output.output0.r = litColor.r;
    output.output0.g = litColor.g;
    output.output0.b = litColor.b;
    output.output0.a = litColor.a;

    return output;
}

technique11 Default
{
    pass p0
    {
        SetVertexShader (CompileShader (vs_5_0, vs()));
        SetGeometryShader(CompileShader (gs_5_0, gs()));
        SetPixelShader (CompileShader (ps_5_0, ps()));
    }
}