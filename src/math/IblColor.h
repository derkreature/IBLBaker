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
#ifndef INCLUDED_BB_COLOUR
#define INCLUDED_BB_COLOUR

#include <IblPlatform.h>
#include <IblVector4.h>

namespace Ibl
{
//-------------------------------------
// Color wrapper for old DXUT controls.
//-------------------------------------
#define COLOR_ARGB(a,r,g,b) ((PackedColor)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define COLOR_RGBA(r,g,b,a) COLOR_ARGB(a,r,g,b)
#define COLOR_VALUE(r,g,b,a) COLOR_RGBA((uint32_t)((r)*255.f),(uint32_t)((g)*255.f),(uint32_t)((b)*255.f),(uint32_t)((a)*255.f))
typedef uint32_t               PackedColor;

inline PackedColor             colourARGB (size_t a, size_t r, size_t g, size_t b)
{
    return COLOR_ARGB (a,r,g,b);
}

inline PackedColor             colourRGBA (size_t r, size_t g, size_t b, size_t a)
{
    return COLOR_ARGB (r,g,b,a);
}

inline PackedColor             colourValue (float r, float g, float b, float a)
{
    return COLOR_VALUE (r,g,b,a);
}

inline Vector4f                fromPackedColor (PackedColor colour)
{
    const float f = 1.0f / 255.0f;
    Vector4f color;
    color.x = f * (FLOAT)(unsigned char)(colour >> 16);
    color.y = f * (FLOAT)(unsigned char)(colour >>  8);
    color.z = f * (FLOAT)(unsigned char)colour;
    color.w = f * (FLOAT)(unsigned char)(colour >> 24);
    return color;
}

inline PackedColor             packedColor(const Vector4f& color)
{
    return COLOR_VALUE (color.x, color.y, color.z, color.w);
}

}


#endif
