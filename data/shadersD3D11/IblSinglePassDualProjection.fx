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
    string ToString = "SinglePassDualProjection";
> = 0.0;

float4x4 worldMatrix : WORLD;
float4x4 worldViewProjection : WORLDVIEWPROJECTION;
float4x4 viewProjection : VIEWPROJECTION;
float4x4 viewInverse : VIEWINVERSE;
float4 eyeLocation : EYELOCATION;
float4 materialDiffuse : MATERIALDIFFUSE;
float4 specularFresnel : SPECULARFRESNEL;
TextureCube diffuseMap : DIFFUSEMAP;
float cameraZNear : CAMERAZNEAR;
float cameraZFar : CAMERAZFAR;
float textureGamma : TEXTUREGAMMA = 2.2;

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
    float2 uv         : TEXCOORD0;
};


struct VertexShaderOut
{
    float4 position       :     SV_POSITION;
    float3 positionInWorld : TEXCOORD0;
    float4 positionInWorldViewProj : TEXCOORD1;
    float3 normal : TEXCOORD2;
    float ModifiedZ : TEXCOORD3;

};

struct PixelShaderInput
{
    float4 position       :     SV_POSITION;
    float3 positionInWorld : TEXCOORD0;
    float4 positionInWorldViewProj : TEXCOORD1;
    float3 normal : TEXCOORD2;
    float ModifiedZ : TEXCOORD3;
    uint rtIndex    : SV_RENDERTARGETARRAYINDEX;

};

struct PixelShaderOut
{
    float4 output0: SV_TARGET0;
};

VertexShaderOut vs (VertexShaderIn vertexShaderIn)
{
    VertexShaderOut output;
    output.position = mul(vertexShaderIn.position, worldViewProjection);
    float3 position = vertexShaderIn.position.xyz;
    float2 texCoord = vertexShaderIn.uv;
    float3 normal = normalize(mul(vertexShaderIn.normal, (float3x3)worldMatrix));    float3 worldPos = mul(float4(position.xyz, 1), worldMatrix).xyz;    output.positionInWorld = worldPos;
    output.positionInWorldViewProj = mul( float4(worldPos,1), viewProjection);
    output.normal = normal;
    output.ModifiedZ = 0.0f;

    return output;
}

[maxvertexcount(12)]
void gs (triangle VertexShaderOut input[3], 
         inout TriangleStream<PixelShaderInput> outStream)
{
  float dpDir[2] = {-1, 1};
  for(int split = 0; split < 2; split++)
  {
{    PixelShaderInput output;
    output.positionInWorld = input[0].positionInWorld;
    output.normal = input[0].normal;
    output.ModifiedZ = input[0].ModifiedZ;
    output.rtIndex = split;
{
output.position = input[0].position;
output.position.z = output.position.z * dpDir[split];
float L = length( output.position.xyz );
output.position = output.position / L;
output.ModifiedZ = output.position.z;
output.position.z = output.position.z + 1;
output.position.x = output.position.x / output.position.z;
output.position.y = output.position.y / output.position.z;
output.position.z = (L - cameraZNear) / (cameraZFar - cameraZNear);output.position.w = 1;
}
        output.positionInWorldViewProj = output.position;
        outStream.Append(output);
 }
{    PixelShaderInput output;
    output.positionInWorld = input[1].positionInWorld;
    output.positionInWorldViewProj = input[1].positionInWorldViewProj;
    output.normal = input[1].normal;
    output.ModifiedZ = input[1].ModifiedZ;
    output.rtIndex = split;
{
output.position = input[1].position;
output.position.z = output.position.z * dpDir[split];
float L = length( output.position.xyz );
output.position = output.position / L;
output.ModifiedZ = output.position.z;
output.position.z = output.position.z + 1;
output.position.x = output.position.x / output.position.z;
output.position.y = output.position.y / output.position.z;
output.position.z = (L - cameraZNear) / (cameraZFar - cameraZNear);output.position.w = 1;
}
        output.positionInWorldViewProj = output.position;
        outStream.Append(output);
 }
{    PixelShaderInput output;
    output.positionInWorld = input[2].positionInWorld;
    output.positionInWorldViewProj = input[2].positionInWorldViewProj;
    output.normal = input[2].normal;
    output.ModifiedZ = input[2].ModifiedZ;
    output.rtIndex = split;
{
output.position = input[2].position;
output.position.z = output.position.z * dpDir[split];
float L = length( output.position.xyz );
output.position = output.position / L;
output.ModifiedZ = output.position.z;
output.position.z = output.position.z + 1;
output.position.x = output.position.x / output.position.z;
output.position.y = output.position.y / output.position.z;
output.position.z = (L - cameraZNear) / (cameraZFar - cameraZNear);output.position.w = 1;
}
        output.positionInWorldViewProj = output.position;
        outStream.Append(output);
 }
    outStream.RestartStrip();
  }
}

PixelShaderOut ps (PixelShaderInput vertexShaderOut) 
{
    float4 litColor = float4(1,1,1,1);
    PixelShaderOut output;

    float4 textureGammaColor = materialDiffuse;
    float3 envDiffuseCoord = vertexShaderOut.positionInWorld.xyz;
    envDiffuseCoord = normalize(float3(-envDiffuseCoord.x, envDiffuseCoord.z, envDiffuseCoord.y));
    textureGammaColor = diffuseMap.SampleLevel(anisotropicSampler, vertexShaderOut.normal.xyz, 0);
clip(vertexShaderOut.ModifiedZ);
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