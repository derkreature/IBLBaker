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

    template <class T>
    inline void
    saturate(T& x) 
    {
        clamp(x, T(0), T(1));
    }

    template <class T>
    inline T
    saturated(const T& x) 
    {
        return clamped(x, T(0), T(1));
    }


    template <class T>
    inline T abs ( T f ) { return (T)fabsf((float)(f)); }

    typedef uint16_t half;
}

#endif