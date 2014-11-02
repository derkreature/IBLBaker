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
#define PI 3.14159265358979323f
#define INV_PI 0.31830988618379067239521257108191f

//------------------------------------------------------------------------------------//
// LUT compute functions used by IblBrdf.hlsl                                         //
//------------------------------------------------------------------------------------//
// Geometry term
// http://graphicrants.blogspot.com.au/2013/08/specular-brdf-reference.html
// I could not have arrived at this without the notes at :
// http://www.gamedev.net/topic/658769-ue4-ibl-glsl/

float GGX(float NoV, float roughness)
{
    // http://graphicrants.blogspot.com.au/2013/08/specular-brdf-reference.html
    // Schlick-Beckmann G.
    float k = roughness/2;
    return NoV / (NoV * (1.0f - k) + k);
}

float geometryForLut(float roughness, float NoL)
{
    return GGX(NoL, roughness * roughness) ;
}

// Visibility term
float visibilityForLut(float roughness, float NoV)
{
    return GGX(NoV, roughness * roughness);
}

// Fresnel Term.
// Inputs, view dot half angle.
float fresnelForLut(float VoH)
{
    return pow(1.0-VoH, 5);
}


// Summation of Lut term while iterating over samples
float2 sumLut(float2 current, float G, float V, float F, float VoH, float NoL, float NoH, float NoV)
{
    G = G * V;
    float G_Vis = G * VoH / (NoH * NoV);
    current.x += (1.0 - F) * G_Vis;
    current.y += F * G_Vis;

    return current;
}

//------------------------------------------------------------------------------------//
// Used by IblBrdf.hlsl generation and IblImportanceSamplingSpecular.fx               //
// Inputs:                                                                            //
//   Spherical hammersley generated coordinate and roughness.                         //
//   Roughness                                                                        //
//   Normal                                                                           //
// Base on GGX example in:                                                            //
// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
//------------------------------------------------------------------------------------//
float3 importanceSampleGGX(float2 Xi, float roughness, float3 N)
{
    float a = roughness * roughness;

    float Phi = 2 * PI * Xi.x;
    float CosTheta = sqrt((1 - Xi.y) / (1 + (a*a - 1) * Xi.y));
    float SinTheta = sqrt(1 - CosTheta * CosTheta);

    float3 H;
    H.x = SinTheta * cos(Phi);
    H.y = SinTheta * sin(Phi);
    H.z = CosTheta;

    float3 UpVector = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
    float3 TangentX = normalize(cross(UpVector, N));
    float3 TangentY = cross(N, TangentX);

    return TangentX * H.x + TangentY * H.y + N * H.z;
}


//------------------------------------------------------------------------------------//
// Shader functions used by IblImportanceSamplingSpecular.fx                          //
//------------------------------------------------------------------------------------//
// D(h) for GGX.
// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
float specularD(float roughness, float NoH)
{
    float r2 = roughness * roughness;
    float NoH2 = NoH * NoH;
    float a = 1.0/(3.14159*r2*pow(NoH, 4));
    float b = exp((NoH2 - 1) / r2 * NoH2);
    return  a * b;
}

float4
sumSpecular (float3 hdrPixel, float NoL, float4 result)
{
    result.xyz += (hdrPixel * NoL);
    result.w += NoL;
    return result;
}

//------------------------------------------------------------------------------------//
// Shader functions used by IblImportanceSamplingDiffuse.fx                           //
//------------------------------------------------------------------------------------//
//
// Derived from GGX example in:
// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
// Image Based Lighting.
//
float3 importanceSampleDiffuse(float2 Xi, float3 N )
{
    float CosTheta = 1.0-Xi.y;
    float SinTheta = sqrt(1.0-CosTheta*CosTheta);
    float Phi = 2*PI*Xi.x;

    float3 H;
    H.x = SinTheta * cos( Phi );
    H.y = SinTheta * sin( Phi );
    H.z = CosTheta;

    float3 UpVector = abs(N.z) < 0.999 ? float3(0,0,1) : float3(1,0,0);
    float3 TangentX = normalize( cross( UpVector, N ) );
    float3 TangentY = cross( N, TangentX );

    return TangentX * H.x + TangentY * H.y + N * H.z;
}


// Sum the diffuse term while iterating over all samples.
float4 
sumDiffuse(float3 diffuseSample, float NoV, float4 result)
{
    result.xyz += diffuseSample;
    result.w++;
    return result;
}
