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
RWTexture2D<float4> BRDFResult : register(u0);

float radicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10f; // / 0x100000000
}

float2
hammersley(uint i, uint N)
{
    return float2(float(i) / float(N), radicalInverse_VdC(i));
}

float2
integrate(float roughness, float NoV)
{
    float3 N = float3(0.0f, 0.0f, 1.0f);
    float3 V = float3(sqrt(1.0f - NoV * NoV), 0.0f, NoV);
    float2 result = float2(0,0);

    const uint NumSamples = 1024;

    precise float Vis = visibilityForLut(roughness, NoV);

    for (uint i = 0; i < NumSamples; i++)
    {
        float2 Xi = hammersley(i, NumSamples);
        float3 H = importanceSampleGGX(Xi, roughness, N);
        precise float3 L = 2.0f * dot(V, H) * H - V;

        float NoL = saturate(L.z);
        float NoH = saturate(H.z);
        float VoH = saturate(dot(V, H));
        float NoV = saturate(dot(N, V));
        if (NoL > 0)
        {
            precise float G = geometryForLut(roughness, NoL);
            precise float F = fresnelForLut(VoH);
            result = sumLut(result, G, Vis, F, VoH, NoL, NoH, NoV); 
        }
    }

    result.x = (result.x / float(NumSamples)) ;
    result.y = (result.y / float(NumSamples)) ;

    return result;
}

[numthreads(16, 16, 1)]
void CSMain(uint2 id : SV_DispatchThreadID)
{
    float roughness = (float)(id.y+0.5) / 256.0;
    float NoV = (float)(id.x+0.5) / 256.0;
      
    float2 result = integrate(roughness, NoV); 
    // [TODO]: I need to fix my DDS saver so that is can save R32G32 and R16G16
    BRDFResult[int2(id.x, 255-id.y)] = float4(result.x, result.y,roughness,1);
}
