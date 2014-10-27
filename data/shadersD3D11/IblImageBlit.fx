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
    string ToString = "imageblit";
> = 0.0;

//back buffer in
texture2D PostProcessTexture : DIFFUSEMAP
<
    string defaultmap = "data/Textures/default.dds";
    string UIWidget = "slider";
    string ScriptClass = "object";
    string ScriptOrder = "standard";
    string ScriptOutput = "color";
    string ToString = "DIFFUSEMAP";
>;			

//---------------------------------------------------------------------
//Texture information
//---------------------------------------------------------------------

sampler MeshTextureSampler = 
sampler_state
{
    Texture = <PostProcessTexture>;    
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
};

float4 materialDiffuse : MATERIALDIFFUSE;

//---------------------------------------------------------------------
// Shader Structures
//---------------------------------------------------------------------

struct VS_OUTPUT
{
	float4 HPosition : POSITION;
	float2 Texture0 : TEXCOORD0;
};


struct PS_OUTPUT
{
	float4 RGBColor : COLOR0;
};

//---------------------------------------------------------------------
// Functions ... (Vertex Shader and Pixel Shader)
//---------------------------------------------------------------------
struct VS_INPUT
{
    float4 position:POSITION;
    float2 tex0:TEXCOORD0;
};

VS_OUTPUT imageVS (VS_INPUT input)
{
	VS_OUTPUT OUT;

	OUT.HPosition =  input.position;
	OUT.HPosition.w = 1;
	OUT.Texture0 = input.tex0;

	return OUT;
}

PS_OUTPUT imagePS(VS_OUTPUT IN)
{   
    PS_OUTPUT OUT;  
    OUT.RGBColor =  tex2D(MeshTextureSampler, IN.Texture0) * materialDiffuse;
    return OUT;
}

technique10 postprocess
{
	pass P0
	{
		VertexShader = compile vs_4_0 imageVS();
		PixelShader = compile ps_4_0 imagePS();
	}
}



