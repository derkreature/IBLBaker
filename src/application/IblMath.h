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


#ifndef INCLUDED_BB_MATH
#define INCLUDED_BB_MATH

#include <IblPlatform.h>

namespace Ibl
{
    #define RANDOM_SCALAR      (((FLOAT)rand())/RAND_MAX)
    const float BB_PI = 3.14159265358979323f;

    inline float lerp(float _a, float _b, float _t)
    {
        return _a + (_b - _a) * _t;
    }

    inline bool
    isPOT(int32_t v) 
    {
        return v && !(v & (v - 1));
    }

    inline int32_t nextPOT(int32_t v) 
    { 
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        return v;
    }

    #ifndef RAD
        #define RAD 3.14159265358979323f / 180.0f
    #endif

    #ifndef DEG
        #define DEG 180.0f / 3.14159265358979323f
    #endif

    inline float random()
    {
        return (float(   (double)rand() / ((double)(RAND_MAX)+(double)(1)) ));
    }

    template <class T>
    inline T maxValue(const T& a, const T&b)
    {
        return a > b ? a : b;
    }

    template <class T>
    inline T minValue(const T& a, const T&b)
    {
        return a < b ? a : b;
    }

    #define RANDOM_NUM      (((FLOAT)rand()-(FLOAT)rand())/RAND_MAX)
    #define RANDOM_VECTOR   D3DVECTOR(RANDOM_NUM,RANDOM_NUM,RANDOM_NUM)
    template <class T>
    inline void clamp (T& x, T min, T max) 
    {
        x = (x<min  ? min : x<max ? x : max);
    }

    template <class T>
    inline T clamped (const T& x, T min, T max) 
    {
        return (x<min  ? min : x<max ? x : max);
    }


    inline float step(float _edge, float _a)
    {
        return _a < _edge ? 0.0f : 1.0f;
    }


    inline float pulse(float _a, float _start, float _end)
    {
        return step(_a, _start) - step(_a, _end);
    }

    // Lifted from bgfx.
    inline void rgbToHsv(float _hsv[3], const float _rgb[3])
    {
        const float rr = _rgb[0];
        const float gg = _rgb[1];
        const float bb = _rgb[2];

        const float s0 = step(bb, gg);

        const float px = lerp(bb, gg, s0);
        const float py = lerp(gg, bb, s0);
        const float pz = lerp(-1.0f, 0.0f, s0);
        const float pw = lerp(2.0f / 3.0f, -1.0f / 3.0f, s0);

        const float s1 = step(px, rr);

        const float qx = lerp(px, rr, s1);
        const float qy = py;
        const float qz = lerp(pw, pz, s1);
        const float qw = lerp(rr, px, s1);

        const float dd = qx - minValue(qw, qy);
        const float ee = 1.0e-10f;

        _hsv[0] = fabsf(qz + (qw - qy) / (6.0f * dd + ee));
        _hsv[1] = dd / (qx + ee);
        _hsv[2] = qx;
    }


    template <class T>
    inline float
        saturate(T x)
    {
        return clamped(x, T(0), T(1));
    }

    template <class T>
    inline T
        saturated(const T& x)
    {
        return clamped(x, T(0), T(1));
    }


    template <class T>
    inline T abs(T f) { return (T)fabsf((float)(f)); }
    inline float fract(float _a)
    {
        return _a - floorf(_a);
    }

    // Lifted from bgfx.
    inline void hsvToRgb(float _rgb[3], const float _hsv[3])
    {
        const float hh = _hsv[0];
        const float ss = _hsv[1];
        const float vv = _hsv[2];

        const float px = fabsf(fract(hh + 1.0f) * 6.0f - 3.0f);
        const float py = fabsf(fract(hh + 2.0f / 3.0f) * 6.0f - 3.0f);
        const float pz = fabsf(fract(hh + 1.0f / 3.0f) * 6.0f - 3.0f);

        _rgb[0] = vv * lerp(1.0f, saturate(px - 1.0f), ss);
        _rgb[1] = vv * lerp(1.0f, saturate(py - 1.0f), ss);
        _rgb[2] = vv * lerp(1.0f, saturate(pz - 1.0f), ss);
    }



    typedef uint16_t half;
}

#endif