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

float4x4 worldViewProjection : WORLDVIEWPROJECTION 
<
    string UIWidget 	= "none";
    string ScriptClass 	= "object";
    string ScriptOrder 	= "standard";
    string ScriptOutput = "none";
    string ToString 	= "WORLDVIEWPROJECTION";
> = 0.0;

struct VertexShaderOut
{
    float4 position   	: 	POSITION;  
};

struct PixelShaderOut
{
    float4 rgba : COLOR0;     
};

VertexShaderOut vertexShader (float4 position : POSITION)
{
    VertexShaderOut output;
    output.position = mul(position, worldViewProjection);
    return output;
}

PixelShaderOut pixelShader (VertexShaderOut vertexShaderOut) 
{ 
    PixelShaderOut output;
    output.rgba = materialDiffuseColor;    
    return output;
}

technique11 basic
{
    pass P0
    {          
	 SetVertexShader (CompileShader( vs_5_0, vertexShader()));
	
      SetPixelShader (CompileShader( ps_5_0, pixelShader()));
    }
}
