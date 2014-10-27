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

#define SAMPLE_COUNT_2 2
#define SAMPLE_COUNT_4 4
#define SAMPLE_COUNT_8 8

float sourceWidth;
float sourceHeight;
float arrayOffset;

// Zbuffer to resolve
Texture2DArray<float4> color1Array;
Texture2D color1;
Texture2DMS <float, SAMPLE_COUNT_2> color2;
Texture2DMS <float, SAMPLE_COUNT_4> color4;
Texture2DMS <float, SAMPLE_COUNT_8> color8;

//------------------------------------------------------------
// Shader Structures
//------------------------------------------------------------

struct VS_OUTPUT
{
	float4 HPosition : SV_POSITION;
	float2 Texture0 : TEXCOORD0;
};


struct VS_INPUT

{

    float4 position:POSITION;
    float4 tex0:TEXCOORD0;
};

SamplerState samNearest
{
    Filter   = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};


VS_OUTPUT colorResolveVS(VS_INPUT input)
{
	VS_OUTPUT OUT;

	float4 temppos;

	temppos.xyz = input.position.xyz;
	temppos.w = 1.0f;


	OUT.HPosition =  temppos;
	OUT.Texture0.xy = input.tex0.xy;

	return OUT;
}

float4 colorResolveArrayPS(VS_OUTPUT IN) : SV_TARGET0

{
    return color1Array.Sample(samNearest, float3(IN.Texture0.x, IN.Texture0.y, arrayOffset)); 
}


float4 colorResolvePS(VS_OUTPUT IN) : SV_TARGET0

{
    float2 texCoord;

    texCoord.x = IN.Texture0.x;
    texCoord.y = IN.Texture0.y;

    float4 colorOut = 
	color1.Sample(samNearest, texCoord.xy);
 

    return colorOut;
}
 


float4 colorResolvePS2(VS_OUTPUT IN) : COLOR0

{

    int2 texCoord;

    texCoord.x = IN.Texture0.x * sourceWidth;

    texCoord.y = IN.Texture0.y * sourceHeight;

	

    float4 color = float4(0,0,0,0);

    for (int i = 0; i < SAMPLE_COUNT_2; i++)

    {
	 color += color2.Load(texCoord, i);

    }



    color *= 1.0 / SAMPLE_COUNT_2;

    return color;

}



float4 colorResolvePS4(VS_OUTPUT IN) : COLOR0

{

    int2 texCoord;

    texCoord.x = IN.Texture0.x * sourceWidth;

    texCoord.y = IN.Texture0.y * sourceHeight;

	

    float4 color = float4(0,0,0,0);

    //[loop]

    for (int i = 0; i < SAMPLE_COUNT_4; i++)

    {

	 color += color4.Load(texCoord, i);

    }


    color *= 1.0 / SAMPLE_COUNT_4;
    return color;
}

float4 colorResolvePS8(VS_OUTPUT IN) : COLOR0
{
    int2 texCoord;
    texCoord.x = IN.Texture0.x * sourceWidth;
    texCoord.y = IN.Texture0.y * sourceHeight;

    float4 color = float4(0,0,0,0);

    for (int i = 0; i < SAMPLE_COUNT_8; i++)

    {
	 color += color8.Load(texCoord, i);
    }

    color *= 1.0 / SAMPLE_COUNT_8;
    return color;
}




technique11 postProcessArray
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, colorResolveVS()));
		SetGeometryShader( NULL );
		SetPixelShader (CompileShader( ps_4_0, 
				colorResolveArrayPS()));
	}
}


technique11 postProcess
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, colorResolveVS()));
		SetGeometryShader( NULL );
		SetPixelShader (CompileShader( ps_4_0, 
				    colorResolvePS()));
	}
}


technique11 postProcess2
{
	pass P0
	{

		SetVertexShader( CompileShader( vs_4_0, colorResolveVS()));
		SetGeometryShader( NULL );
		SetPixelShader (CompileShader( ps_4_0, 
				    colorResolvePS2()));

	}
}



technique11 postProcess4
{
	pass P0
	{

		SetVertexShader( CompileShader( vs_4_0, colorResolveVS()));
		SetGeometryShader( NULL );
		SetPixelShader (CompileShader( ps_4_0, 
				    colorResolvePS4()));

	}

}



technique11 postProcess8
{

	pass P0
	{

		SetVertexShader( CompileShader( vs_4_0, colorResolveVS()));
		SetGeometryShader( NULL );
		SetPixelShader (CompileShader( ps_4_0, 

				    colorResolvePS8()));

	}

}