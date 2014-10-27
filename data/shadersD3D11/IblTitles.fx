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
float2 ScreenSize : SCREENSIZE
<
	string UIWidget = "slider";
    string ScriptClass = "object";
    string ScriptOrder = "standard";
    string ScriptOutput = "color";
    string ToString = "SCREENSIZE";
>;	

float TitlesTime = 0;
texture2D MeshTexture: POSTPROCESSMAP
<
    string UIWidget = "slider";
    string ScriptClass = "object";
    string ScriptOrder = "standard";
    string ScriptOutput = "color";
    string ToString = "POSTPROCESSMAP";
>;

texture2D TitlesTexture;



sampler screenMapSampler = 
sampler_state
{
    Texture = <MeshTexture>;
    AddressU = Clamp;
    AddressV = Clamp;
    Filter = MIN_MAG_MIP_POINT;    
};

sampler titlesSampler = 
sampler_state
{
    Texture = <TitlesTexture>;
    AddressU = Clamp;
    AddressV = Clamp;
    Filter = MIN_MAG_MIP_LINEAR;    
};



//---------------------------------------------------------------------
// Shader Structures
//---------------------------------------------------------------------

struct VS_OUTPUT
{
	float4 HPosition : POSITION;
	float2 baseTC0 : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 Color : COLOR0;
};

struct VS_INPUT
{
    float4 position:POSITION;
    float2 tex0:TEXCOORD0;
};

VS_OUTPUT CreditsVS(VS_INPUT input)
{
	VS_OUTPUT OUT;

	float4 temppos;
	temppos.xyz = input.position.xyz;
	temppos.w = 1.0f;

	OUT.HPosition =  temppos; 
	OUT.baseTC0.xy = input.tex0.xy;     

	return OUT;
}

///////////////// pixel shader //////////////////
PS_OUTPUT CreditsPS(VS_OUTPUT IN)
{
   PS_OUTPUT OUT;
    
   float4 sceneColor = tex2D(screenMapSampler, IN.baseTC0.xy);
   float4 titlesColor = tex2D(titlesSampler, IN.baseTC0.xy);

   float4 sceneColorR = sceneColor * float4(1,0,0,1);
   float4 black = float4(0,0,0,0);
   if (TitlesTime >= 8 && TitlesTime < 9)
   {
        // blend to black.
       OUT.Color = lerp(titlesColor, black, ((TitlesTime - 8)));
   }
   else if (TitlesTime >= 9 && TitlesTime < 10)
   {
       OUT.Color = lerp(black, sceneColor, ( (TitlesTime - 9)));
   }
   else
   {
        OUT.Color = titlesColor;
   }

  return OUT;
}

technique11 postprocess
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, CreditsVS()));
		SetGeometryShader( NULL );
		SetPixelShader (CompileShader( ps_4_0, CreditsPS()));
	}
}

