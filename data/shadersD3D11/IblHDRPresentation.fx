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
float exposure : EXPOSURE;
float applyFilmicTone : APPLYFILMICTONE;
float applyBloom : APPLYBLOOM;
float bloomMagnitude : BLOOMMAGNITUDE;
float KeyValue : BLOOMKEYVALUE;
float gamma  : GAMMA;

float4 lensDistortion : LENSDISTORTION;
float4 vignetteParameters : VIGNETTEPARAMETERS;
float4 vignetteColor : VIGNETTECOLOR;
texture2D postProcessTexture: POSTPROCESSMAP;
texture2D bloomTexture : BLOOMMAP;
Texture2D lumTex : ADAPTEDLUMINANCEMAP;

sampler PointSampler 
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = clamp;
    AddressV = clamp;
    MipLODBias = 0.0f;
};


SamplerState LinearSampler
{
	Filter =  MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
	MipLODBias = 0.0f;
};


//--------------------------------------------------------------
// Shader Structures
//--------------------------------------------------------------

struct VS_OUTPUT
{
	float4 HPosition : POSITION;
	float2 Texture0 : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 RGBColor : COLOR0;
};

struct VS_INPUT
{
    float4 position:POSITION;
    float4 tex0:TEXCOORD0;
};

VS_OUTPUT HDRVS(VS_INPUT input)
{
	VS_OUTPUT OUT;

	float4 temppos;
	temppos.xyz = input.position.xyz;
	temppos.w = 1.0f;

	OUT.HPosition =  temppos;
	OUT.Texture0.xy = input.tex0.xy;
	return OUT;
}

// Approximates luminance from an RGB value
float CalcLuminance(float3 color)
{
    return max(dot(color, float3(0.299f, 0.587f, 0.114f)), 0.0001f);
}


float3 filmicTonemap(float3 x) {
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    return ((x*(A*x+C*B)+D*E) / (x*(A*x+B)+D*F))- E / F;
}

float3 applyFilmicToneMap(float3 color) 
{
    color = 2.0f * filmicTonemap(exposure * color);
    float3 whiteScale = 1.0f / filmicTonemap(11.2);
    color *= whiteScale;
    return color;
}


// Applies exposure and tone mapping to the specific color, and applies
// the threshold to the exposure value.
float3 ToneMap(float3 color, float avgLuminance, float threshold, out float exposureInOut)
{
    float pixelLuminance = CalcLuminance(color);

    avgLuminance = max(avgLuminance, 1e-3);
    float linearExposure = (KeyValue / (avgLuminance));

    exposureInOut = log2(max(linearExposure, 1e-3));
    exposureInOut -= threshold;
    exposureInOut = max(exposureInOut, 1e-3);
    

    color = 2.0f * filmicTonemap(exposureInOut * color);
    float3 whiteScale = 1.0f / filmicTonemap(11.2);
    color *= whiteScale;

    return color;
}

float2 lensWarp(float2 tex)
{
	float k = 1.35;
     float kcube = 1.5;                         
	float r2 = (tex.x-0.5) * (tex.x-0.5) + (tex.y-0.5) * (tex.y-0.5);  
      float f = 0;
      if( kcube == 0.0)
      {                 
		f = 1 + r2 * k;         
	 }
      else
      {
		f = 1 + r2 * (k + kcube * sqrt(r2));         
	 };                 
	return float2(f*(tex.x-0.5)+0.5, f*(tex.y-0.5)+0.5);
}



float3 Yxy2xyz(float3 Yxy) {
    float3 xyz;
    xyz.g = Yxy.r;
    if (Yxy.b > 0.0) {
        xyz.r = Yxy.r * Yxy.g / Yxy.b;
        xyz.b = Yxy.r * (1 - Yxy.g - Yxy.b) / Yxy.b;
    } else {
        xyz.rb = 0.0;
    }
    return xyz;
}

float3 xyz2rgb(float3 xyz) {
    const float3x3 m  = {  2.5651, -1.1665, -0.3986,
                          -1.0217,  1.9777,  0.0439, 
                           0.0753, -0.2543,  1.1892 };
    return mul(m, xyz);
}


float3 rgb2xyz(float3 rgb) {
    const float3x3 m = { 0.5141364, 0.3238786,  0.16036376,
                         0.265068,  0.67023428, 0.06409157,
                         0.0241188, 0.1228178,  0.84442666 };
    return mul(m, rgb);
}

float3 xyz2Yxy(float3 xyz) {
    float w = xyz.r + xyz.g + xyz.b;
    if (w > 0.0) {
        float3 Yxy;
        Yxy.r = xyz.g;
        Yxy.g = xyz.r / w;
        Yxy.b = xyz.g / w;
        return Yxy;
    } else {
        return 0.0;
    }
}


const float A = 0.15;
const float B = 0.50;
const float C = 0.10;
const float D = 0.20;
const float E = 0.02;
const float F = 0.30;
const float W = 11.2;
 
float3 Uncharted2Tonemap(float3 x)
{
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}
 
float3 applyUnchartedToneMap( float3 texColor)
{
   texColor *= 16;  // Hardcoded Exposure Adjustment
    
   //float ExposureBias = 2.0f;
   float3 curr = Uncharted2Tonemap(exposure*texColor);
    
   float3 whiteScale = 1.0f/Uncharted2Tonemap(W);
   return curr*whiteScale;   
}


PS_OUTPUT HDRPS(VS_OUTPUT IN)
{
    PS_OUTPUT OUT;

	// lensWarp
    float2 texCoord = (IN.Texture0.xy);

    // Purturb coordinates. (heat, lens, etc). 
    float4 color = postProcessTexture.Sample(PointSampler, texCoord);

    
    if (applyFilmicTone > 0)
    {
	   float avgLuminance = 
		exp(lumTex.SampleLevel(LinearSampler, float2(0.5, 0.5), 10).x);

           float exposureInOut = 0;
	   color.rgb = ToneMap(color.rgb, avgLuminance, 0, exposureInOut);
    }
    else
    {
    	//color.rgb = color.rgb * pow(2.0, exposure);
        color.rgb = applyFilmicToneMap(color.rgb);
    }

    color.rgb = pow(color.rgb, 1.0f / gamma);

 
    if (applyBloom > 0)
    {
	  float3 bloom = bloomTexture.Sample(LinearSampler, texCoord).rgb;
          bloom = bloom * bloomMagnitude;
	  color.rgb = color.rgb + bloom.rgb;
    }

    // Now vignette.
    if (vignetteParameters.x == 1.0f)
    {
	    float2 dist =  IN.Texture0.xy - float2(0.5f, 0.5f);
	    dist.x = 1 - dot(dist, dist);
	    float vignette = pow(dist.x, 5.5f);
	    float4 vignetteVector = 
		    float4(vignette, vignette, vignette,1) ;
	    color *= 	
	       (lerp(vignetteVector, float4(1,1,1,1), vignetteColor));
    }
    OUT.RGBColor = color;
    return OUT;
}

technique11 postprocess
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_5_0, HDRVS()));
		SetGeometryShader( NULL );
		SetPixelShader (CompileShader( ps_5_0, HDRPS()));
	}
}



