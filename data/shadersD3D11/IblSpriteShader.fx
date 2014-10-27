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
    string ToString = "simpleshader";
> = 0.0;

float4 materialDiffuseColor : MATERIALDIFFUSE
<
    string UIWidget 	= "none";
    string ScriptClass 	= "object";
    string ScriptOrder 	= "standard";
    string ScriptOutput = "none";
    string ToString 	= "MATERIALDIFFUSE";
> = 0.0;

//----------------------------------------------
// Mesh Diffuse Texture
//----------------------------------------------
texture MeshTexture : DIFFUSEMAP 
<
	string defaultmap = "data/Textures/default.dds";
    string UIWidget = "slider";
    string ScriptClass = "object";
    string ScriptOrder = "standard";
    string ScriptOutput = "color";
    string ToString = "DIFFUSEMAP";
>;

//---------------------------------------------
// Diffuse Texture sampler
//---------------------------------------------
sampler MeshTextureSampler = 
sampler_state
{
    Texture = <MeshTexture>;    
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
};

float4x4 orthoMatrix;
float4x4 viewMatrix : VIEW;

struct VertexShaderOut
{
    float4 position   	: 	POSITION;  
    float2 texCoord        :    TEXCOORD0;
};

struct PixelShaderOut
{
    float4 rgba : COLOR0;     
};

VertexShaderOut vertexShader (float3 position : POSITION,
					    float2 uv : TEXCOORD0)
{
    VertexShaderOut output;
    float4 outVertex;
    outVertex.xyz = position.xyz;
    outVertex.w = 1.0f;

    float4x4 m = orthoMatrix;
    output.position = mul(outVertex, m);
    output.texCoord = uv;
    return output;
}

PixelShaderOut pixelShader (VertexShaderOut vertexShaderOut) 
{ 
    PixelShaderOut output;
    float4 coord;
    coord.xy = vertexShaderOut.texCoord;
    coord.z = 0;
    coord.w = 0;
    float4 mapColor = tex2Dlod (MeshTextureSampler, coord);

    output.rgba = mapColor  * materialDiffuseColor;        


    return output;
}

VertexShaderOut spriteVS (float3 position : POSITION,
			  float2 uv : TEXCOORD0)
{
    VertexShaderOut output;
    float4 outVertex;

    outVertex.xyz = position.xyz;
    outVertex.w = 1.0f;

    output.position = outVertex;
    output.texCoord = uv;
    return output;
}

PixelShaderOut spritePS (VertexShaderOut vertexShaderOut) 
{ 
    PixelShaderOut output;
    float4 coord;
    coord.xy = vertexShaderOut.texCoord;
    coord.z = 0;
    coord.w = 0;
    float4 mapColor = tex2Dlod (MeshTextureSampler, coord);

    output.rgba = mapColor * materialDiffuseColor ;    

    return output;
}

technique10 basic
{
    pass P0
    {          
	 SetVertexShader (CompileShader( vs_4_0, vertexShader()));
	
      SetPixelShader (CompileShader( ps_4_0, pixelShader()));
    }
}

technique10 sprite
{
    pass P0
    {          
	 SetVertexShader (CompileShader( vs_4_0, spriteVS()));	
      SetPixelShader (CompileShader( ps_4_0, spritePS()));
    }
}
