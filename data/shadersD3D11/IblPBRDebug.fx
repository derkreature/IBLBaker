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
    string ToString = "PBRDebug";
> = 0.0;


float3x3 invert_3x3( float3x3 M )
{
    float D = determinant( M );    
    float3x3 T = transpose( M );    

   return float3x3(
        cross( T[1], T[2] ),
        cross( T[2], T[0] ),
        cross( T[0], T[1] ) ) / (D+1e-6);    
}


float4x4 worldMatrix : WORLD;
float4x4 worldViewProjection : WORLDVIEWPROJECTION;
float4x4 viewProjection : VIEWPROJECTION;
float4 eyeLocation : EYELOCATION;
float4 materialDiffuse : MATERIALDIFFUSE;
Texture2D diffuseMap : DIFFUSEMAP;

float4 specularFresnel : SPECULARFRESNEL;
float4 userAlbedo : USERALBEDO;
float debugTerm : RENDERDEBUGTERM; 
Texture2D specularRMCMap : GLOSSMAP;
Texture2D normalMap : NORMALMAP;
float textureGamma : TEXTUREGAMMA;
float specularIntensity : SPECULARINTENSITY;
float roughnessRescale : ROUGHNESSSCALE;
float4 specularWorkflow : SPECULARWORKFLOW;


TextureCube diffuseProbe : IBLDIFFUSEPROBE; 
TextureCube specularProbe : IBLSPECULARPROBE;

TextureCube diffuseProbeMDR : IBLDIFFUSEPROBEMDR;
TextureCube specularProbeMDR : IBLSPECULARPROBEMDR; 

TextureCube diffuseProbeLDR : IBLDIFFUSEPROBELDR;
TextureCube specularProbeLDR : IBLSPECULARPROBELDR;

Texture2D specularIntegration : IBLBRDF; 
float EnvironmentScale : ENVIRONMENTSCALE = 1.0f;

float RoughnessScale : IBLSPECULARMIPDELTAS;
float4 UserRM : USERRM;

SamplerState anisotropicSampler
{
    Filter =     ANISOTROPIC;
    MaxAnisotropy = 16;
    AddressU  = Wrap; 
    AddressV  = Wrap;
    BorderColor = float4(10000,10000,10000,10000);
};

SamplerState clampSampler
{
    Filter =  MIN_MAG_MIP_LINEAR;
    AddressU  = Clamp;
    AddressV  = Clamp;
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
    float2 uv : TEXCOORD2;
    float3 normal : TEXCOORD3;

};

struct PixelShaderInput
{
    float4 position       :     SV_POSITION;
    float3 positionInWorld : TEXCOORD0;
    float4 positionInWorldViewProj : TEXCOORD1;
    float2 uv : TEXCOORD2;
    float3 normal : TEXCOORD3;

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

    float3 normal = normalize(mul(vertexShaderIn.normal, (float3x3)worldMatrix));    
    float3 worldPos = mul(float4(position.xyz, 1), worldMatrix).xyz;    
    output.positionInWorld = worldPos;
    output.positionInWorldViewProj = mul( float4(worldPos,1), viewProjection);
    output.uv = texCoord;
    output.normal = normal;

    return output;
}

float4
resolveAlbedo(PixelShaderInput vertexShaderOut)
{
    float4 texColor0 = diffuseMap.Sample(anisotropicSampler, vertexShaderOut.uv);
    float4 diffuseColor = float4(pow(texColor0.rgb*materialDiffuse.rgb, textureGamma).rgb, materialDiffuse.a);

    float3 correctedUserAlbedo = pow(userAlbedo.rgb, textureGamma).rgb;
    diffuseColor.rgb = lerp(diffuseColor.rgb, correctedUserAlbedo.rgb, userAlbedo.a);

    return diffuseColor;
}

float3
resolveNormal(PixelShaderInput vertexShaderOut)
{
    float3 normal = float3(0,0,0);
    float3 tangent = float3(0,0,0);
    float3 binormal = float3(0,0,0);

    // Tangent basis from screen space derivatves. 
    normal = vertexShaderOut.normal;
    float3x3 tbnTransform;
    float4 texNormal =  normalMap.Sample(anisotropicSampler, vertexShaderOut.uv);
    texNormal.xyz = normalize((2.0f * texNormal.xyz) - 1.0f);
    float3 dp1 = ddx_fine(vertexShaderOut.positionInWorld.xyz);
    float3 dp2 = ddy_fine(vertexShaderOut.positionInWorld.xyz);
    float2 duv1 = ddx_fine(vertexShaderOut.uv.xy);
    float2 duv2 = ddy_fine(vertexShaderOut.uv.xy);
    float3x3 M = float3x3(dp1, dp2, normalize(normal));
    float3x3 inverseM = invert_3x3(M);
    float3 T = mul(inverseM, float3(duv1.x, duv2.x, 0));
    float3 B = mul(inverseM, float3(duv1.y, duv2.y, 0));
    float scaleT = 1.0f/(dot(T,T) + 1e-6);
    float scaleB = 1.0f/(dot(B,B) + 1e-6);
    tbnTransform[0] = normalize(T*scaleT);
    tbnTransform[1] = -normalize(B*scaleB);
    tbnTransform[2] = normalize(normal);
    tangent = tbnTransform[0];
    normal =  normalize((tbnTransform[0] * texNormal.x) + (tbnTransform[1] * texNormal.y) + (tbnTransform[2] ));
    return normal;
}

float4
shade (float vdotn, 
       float roughness, 
       float metalness, 
       float bakedAO,
       float4 diffuseColor,
       float4 diffuseIBL, 
       float4 specularIBL,
       float3 normal)
{
    roughness = roughness * (1.0f / roughnessRescale);

    float4 brdfTerm = specularIntegration.SampleLevel(clampSampler, float2(vdotn, 1.0-roughness), 0);
    float3 metalSpecularIBL = specularIBL.rgb; 

    float3 dielectricColor = float3(0.04, 0.04, 0.04);
    float3 diffColor = diffuseColor.rgb * (1.0 - metalness);
    float3 specColor = lerp(dielectricColor.rgb, diffuseColor.rgb, metalness) * specularIntensity;
   
    diffuseIBL.rgb = lerp(diffuseIBL.rgb * 0.3f, diffuseIBL.rgb, bakedAO);
    
    float3 albedoByDiffuse = diffColor.rgb * diffuseIBL.rgb;

    float4 litColor;
    litColor.rgb =  (albedoByDiffuse.rgb + (metalSpecularIBL * (specColor * brdfTerm.x + (brdfTerm.y)))) * bakedAO;
    litColor.a = 1.0;

    // Yes, yes, I know, I'm being extremely lazy.
    [branch]
    if (debugTerm > 0)
    {
        [branch]
        if(debugTerm == 1)
            return float4((normal.xyz + 1.0) * 0.5, 1.0);
        else if(debugTerm == 2)
            return float4(bakedAO.xxx, 1.0);
        else if (debugTerm == 3)
            return diffuseColor;
        else if (debugTerm == 4)
            return float4(diffuseIBL.xyz, 1.0);
        else if (debugTerm == 5)
            return float4(specularIBL);
        else if (debugTerm == 6)
            return float4(metalness.xxx, 1.0); 
        else if (debugTerm == 7)
            return float4(roughness.xxx, 1.0); 
        else if (debugTerm == 8) 
            return float4(brdfTerm.x, brdfTerm.y, 0, 0);
    }
    return litColor;
} 

PixelShaderOut psHDR (PixelShaderInput vertexShaderOut) 
{
    PixelShaderOut output;

    float4 diffuseColor = resolveAlbedo(vertexShaderOut);
    float3 normal = resolveNormal(vertexShaderOut);

    // view, vdotn and reflection.
    float3 view = normalize ( eyeLocation.xyz - vertexShaderOut.positionInWorld.xyz);
    float3 reflection = normalize(reflect(-view, normalize(normal)));    
    float  vdotn = (dot (view, normal));

    // Specular map parameters.
    float4 specularRMC = specularRMCMap.Sample(anisotropicSampler, vertexShaderOut.uv);
    float roughness = lerp(specularRMC.x, UserRM.x, UserRM.y);
    float metalness = lerp(specularRMC.y, UserRM.z, UserRM.w);

    roughness = lerp(roughness, 1.0 - roughness, specularWorkflow.x);
    metalness = lerp(metalness, 1.0 - metalness, specularWorkflow.y);

    float bakedAO = specularRMC.b;

    // Diffuse Probe. Convert Irradiance to Radiance.
    float4 diffuseIBL = ((diffuseProbe.SampleLevel(anisotropicSampler, normal, 0)/ 3.14159) * EnvironmentScale) ;

    // Specular Probe.
    float specMipLevel = (roughness) * RoughnessScale;
    float4 specularIBL = specularProbe.SampleLevel(anisotropicSampler, reflection, specMipLevel) * EnvironmentScale;

    output.output0 = shade(vdotn, roughness, metalness, bakedAO, diffuseColor, diffuseIBL, specularIBL, normal);

    return output;
}

technique11 Default
{
    pass p0
    {
        SetVertexShader (CompileShader (vs_5_0, vs()));
        SetGeometryShader (NULL);
        SetPixelShader (CompileShader (ps_5_0, psHDR()));
    }
}

