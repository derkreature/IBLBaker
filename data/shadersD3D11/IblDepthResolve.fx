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

// Zbuffer to resolve
Texture2D <float> depth;
Texture2DMS <float, SAMPLE_COUNT_2> depth2;
Texture2DMS <float, SAMPLE_COUNT_4> depth4;
Texture2DMS <float, SAMPLE_COUNT_8> depth8;

//------------------------------------------------------------
// Shader Structures
//------------------------------------------------------------

struct VS_OUTPUT
{
    float4 HPosition : POSITION;
    float2 Texture0 : TEXCOORD0;
};

struct VS_INPUT

{

    float4 position:POSITION;

    float4 tex0:TEXCOORD0;

};



VS_OUTPUT depthResolveVS(VS_INPUT input)

{

	VS_OUTPUT OUT;



	float4 temppos;

	temppos.xyz = input.position.xyz;

	temppos.w = 1.0f;



	OUT.HPosition =  temppos;

	OUT.Texture0.xy = input.tex0.xy;

	return OUT;

}



float depthResolvePS(VS_OUTPUT IN) : SV_DEPTH

{

    int2 texCoord;

    texCoord.x = IN.Texture0.x * sourceWidth;

    texCoord.y = IN.Texture0.y * sourceHeight;

	

    return depth.Load(int3(texCoord.x, texCoord.y, 0));

}





float depthResolvePS2(VS_OUTPUT IN) : SV_DEPTH

{

    int2 texCoord;

    texCoord.x = IN.Texture0.x * sourceWidth;

    texCoord.y = IN.Texture0.y * sourceHeight;

	

    float depth = 0;

    for (int i = 0; i < SAMPLE_COUNT_2; i++)

    {

	 depth += depth2.Load(texCoord, i);

    }



    depth *= 1.0 / SAMPLE_COUNT_2;

    return depth;

}



float depthResolvePS4(VS_OUTPUT IN) : SV_DEPTH

{

    int2 texCoord;

    texCoord.x = IN.Texture0.x * sourceWidth;

    texCoord.y = IN.Texture0.y * sourceHeight;

	

    float depth = 0;

    //[loop]

    for (int i = 0; i < SAMPLE_COUNT_4; i++)

    {

	 depth += depth4.Load(texCoord, i);

    }


    depth *= 1.0 / SAMPLE_COUNT_4;
    return depth;
}

float depthResolvePS8(VS_OUTPUT IN) : SV_DEPTH
{
    int2 texCoord;
    texCoord.x = IN.Texture0.x * sourceWidth;
    texCoord.y = IN.Texture0.y * sourceHeight;

    float depth = 0;

    for (int i = 0; i < SAMPLE_COUNT_8; i++)

    {
	 depth += depth8.Load(texCoord, i);
    }

    depth *= 1.0 / SAMPLE_COUNT_8;
    return depth;
}


technique11 postProcess
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, depthResolveVS()));
		SetGeometryShader( NULL );
		SetPixelShader (CompileShader( ps_4_0, 
				    depthResolvePS()));
	}
}


technique11 postProcess2
{
	pass P0
	{

		SetVertexShader( CompileShader( vs_4_0, depthResolveVS()));
		SetGeometryShader( NULL );
		SetPixelShader (CompileShader( ps_4_0, 
				    depthResolvePS2()));

	}
}



technique11 postProcess4
{
	pass P0
	{

		SetVertexShader( CompileShader( vs_4_0, depthResolveVS()));
		SetGeometryShader( NULL );
		SetPixelShader (CompileShader( ps_4_0, 
				    depthResolvePS4()));

	}

}



technique11 postProcess8
{

	pass P0
	{

		SetVertexShader( CompileShader( vs_4_0, depthResolveVS()));
		SetGeometryShader( NULL );
		SetPixelShader (CompileShader( ps_4_0, 

				    depthResolvePS8()));

	}

}