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
    string ToString = "ProjectParaboloidToEnvironment";
> = 0.0;


float4x4 mWorldViewProj : WORLDVIEWPROJECTION;
float4x4 mWorld : WORLD;
float4x4 mView : VIEW;
float4x4 mProj : PROJECTION;
float3 vEye : EYELOCATION;

float4x4 g_mViewCM[6] : CUBEVIEWS;
Texture2DArray paraboloidMap : PARABOLOIDMAP;

SamplerState g_samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

//----------------------------
// Cubemap via Geometry Shader
//----------------------------
struct VS_CUBEMAP_IN
{
	float4 Pos		: POSITION;
	float3 Normal		: NORMAL;
	float2 Tex		: TEXCOORD0;
};

struct GS_CUBEMAP_IN
{
    float4 Pos		: SV_POSITION;
    float3 Normal       : NORMAL;
    float2 Tex		: TEXCOORD0;
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

    // Compute world position
    output.Pos = mul( input.Pos, mWorld );
    output.Normal = normalize(mul(input.Normal, (float3x3)(mWorld)).xyz);

    // Propagate tex coord
    output.Tex = input.Tex;

    return output;
}

[maxvertexcount(18)]
void GS_CubeMap( triangle GS_CUBEMAP_IN input[3], inout TriangleStream<PS_CUBEMAP_IN> CubeMapStream )
{
    for( int f = 0; f < 6; ++f )
    {
        // Compute screen coordinates
        PS_CUBEMAP_IN output;
        output.RTIndex = f;
        for( int v = 0; v < 3; v++ )
        {
	    output.Normal = (input[v].Normal.xyz);
            output.Pos = mul( input[v].Pos, g_mViewCM[f] );
            output.Pos = mul( output.Pos, mProj );
            output.Tex = input[v].Tex;
		
            CubeMapStream.Append( output );
        }
        CubeMapStream.RestartStrip();
    }
}


float4 PS_CubeMap( PS_CUBEMAP_IN input ) : SV_Target
{
    float3 R = normalize(input.Normal); //mul(, mView); // Paraboloid view.
	
    float2 front;
	front.x = R.x / (R.z + 1);
	front.y = R.y / (R.z + 1);
	front.x = .5f * front.x + .5f; //bias and scale to correctly sample a d3d texture
	front.y = -.5f * front.y + .5f;
	
	// calculate the back paraboloid map texture coordinates
	float2 back;
	back.x = R.x / (1 - R.z);
	back.y = R.y / (1 - R.z);
    back.x = .5f * back.x + .5f; //bias and scale to correctly sample a d3d texture
    back.y = -.5f * back.y + .5f;

#if 1
    if (R.z < 0)
    {
       return paraboloidMap.Sample( g_samLinear, float3(back.x, back.y, 0) );	
    }
    else
    {
       return paraboloidMap.Sample( g_samLinear, float3(front.x, front.y, 1) );	
    }
#endif
#if 0
    return float4((R.xyz + 1.0f) * 0.5f, 1);
#endif
}


//-----------
// Techniques
//-----------

technique11 basic
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_5_0, VS_CubeMap() ) );
        SetGeometryShader( CompileShader( gs_5_0, GS_CubeMap() ) );
        SetPixelShader( CompileShader( ps_5_0, PS_CubeMap() ) );
    }
};

