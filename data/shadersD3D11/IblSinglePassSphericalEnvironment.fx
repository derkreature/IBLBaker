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
    string ToString = "SinglePassSphericalEnvironment";
> = 0.0;

float4x4 worldMatrix : WORLD;
float4x4 worldViewProjection : WORLDVIEWPROJECTION;
float4x4 viewProjection : VIEWPROJECTION;
float4x4 projectionMatrix : PROJECTION;
float4x4 viewInverse : VIEWINVERSE;
float4 eyeLocation : EYELOCATION;
float4 materialDiffuse : MATERIALDIFFUSE;
float4 specularFresnel : SPECULARFRESNEL;
Texture2D diffuseMap : DIFFUSEMAP;
float cameraZNear : CAMERAZNEAR;
float cameraZFar : CAMERAZFAR;
float textureGamma : TEXTUREGAMMA = 2.2;
float4x4 g_mViewCM[6] : CUBEVIEWS;

SamplerState anisotropicSampler
{
    Filter =     ANISOTROPIC;
    MaxAnisotropy = 16;
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
    output.normal = normalize(mul(vertexShaderIn.position.xyz, (float3x3)worldMatrix));    

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

#define INV_PI 1.0 / 3.14159;

float4 texSpherical(float3 dir, float lod)
{
    float n = length(dir.xz);

    float2 pos = float2( (n>0.0000001) ? dir.x / n : 0.0, dir.y);
    pos = acos(pos)*INV_PI;
    pos.x = (dir.z > 0.0) ? pos.x*0.5 : 1.0-(pos.x*0.5);
    pos.x = 1.0-pos.x;
    return diffuseMap.SampleLevel(anisotropicSampler, pos, lod);
}

PixelShaderOut ps (PixelShaderInput vertexShaderOut) 
{
    float4 litColor = float4(1,1,1,1);
    PixelShaderOut output;

    float4 textureGammaColor = materialDiffuse;
    float3 envDiffuseCoord = vertexShaderOut.normal;
    textureGammaColor = texSpherical(vertexShaderOut.normal.xyz, 0);

    float4    diffuseColor = float4(pow(textureGammaColor, textureGamma).rgb, textureGammaColor.a);
    litColor.rgba = diffuseColor.rgba;

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