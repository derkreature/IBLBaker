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

#ifndef BB_LIMITS_INCLUDED
#define BB_LIMITS_INCLUDED

#include <IblPlatform.h>

namespace Ibl
{
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

//---------------------------------------------
// A bare bones template Limits implementation.
//---------------------------------------------
template <class T>
class Limits
{
  public:
    static bool                isEqual ( T a,  T b) { return isZero (a - b); }
    static bool                isZero ( T a) { return (a == 0); }
    static bool                isNaN ( T a) { return  false; }
    static bool                isInf ( T) { return !_finite (a); }

    static size_t              maxVal (const T& a, const T& b) { return (a > b) ? a : b; }
    static size_t              minVal (const T& a, const T& b) { return (a < b) ? a : b; }
    static T                   maximum();
    static T                   minimum();
    static T                   one() { return 1; }
};

template<> class Limits <char>
{
  public:
    static char                 maximum() { return CHAR_MAX; }
    static char                 minimum() { return CHAR_MIN; }
    static bool                 isEqual (char a,  char b) { return a == b; }
};

template<> class Limits <uint8_t>
{
public:
    static uint8_t             maximum() { return UCHAR_MAX; }
    static uint8_t             minimum() { return 0; }
};

template<> class Limits <int16_t>
{
  public:
    static int16_t             maximum() { return SHRT_MAX; }
    static int16_t             minimum() { return SHRT_MIN; }   
};

template<> class Limits <uint16_t>
{
  public:
    static uint16_t             maximum() { return USHRT_MAX; }
    static uint16_t             minimum() { return 0; }
};

template<> class Limits <int32_t>
{
  public:
    static int32_t             maximum() { return INT_MAX; }
    static int32_t             minimum() { return INT_MIN; }
    static int32_t             maxVal (const int32_t& a, const int32_t& b) { return (a > b) ? a : b; }
    static int32_t             minVal (const int32_t& a, const int32_t& b) { return (a < b) ? a : b;}
    static bool                isZero (int32_t a) { return (a == 0); }
    static bool                isEqual (int32_t a,  int32_t b) { return a == b; }
};

template<> class Limits <int64_t>
{
  public:
    static int64_t             maximum() { return _I64_MAX; }
    static int64_t             minimum() { return _I64_MIN; }    
    static int64_t             maxVal (const int64_t& a, const int64_t& b) { return (a > b) ? a : b; }
    static int64_t             minVal (const int64_t& a, const int64_t& b) { return (a < b) ? a : b; }
    static bool                isZero (int64_t a) { return (a == 0); }
    static bool                isEqual (int64_t a,  int64_t b) { return a == b; }
};

template<> class Limits <uint64_t>
{
  public:
    static uint64_t            maximum() { return _UI64_MAX; }
    static uint64_t            minimum() { return 0; }
    static uint64_t            maxVal (const uint64_t& a, const uint64_t& b) { return (a > b) ? a : b; }
    static uint64_t            minVal (const uint64_t& a, const uint64_t& b) { return (a < b) ? a : b; }
    static bool                isZero (uint64_t a) { return (a == 0); }
    static bool                isEqual (uint64_t a,  uint64_t b) { return a == b; }
};

template<> class Limits <uint32_t>
{
  public:
    static uint32_t            maximum() { return UINT_MAX; }
    static uint32_t            minimum() { return 0; }    
    static bool                isZero (uint32_t a) { return (a == 0); }
    static bool                isEqual (uint32_t a,  uint32_t b) { return a == b; }
};

template<> class Limits <long>
{
  public:
    static long                maximum() { return LONG_MAX; }
    static long                minimum() { return LONG_MIN; }    
    static bool                isZero (long a) { return (a == 0); }
    static long                one() { return 1; }
    static bool                isEqual (long a,  long b) { return a == b; }
};

template<> class Limits <unsigned long>
{
  public:
    static unsigned long       maximum() { return ULONG_MAX; }
    static unsigned long       minimum() { return 0; }
    static bool                isZero (unsigned long a) { return (a == 0); }
    static bool                isEqual (unsigned long a, unsigned long b) { return a == b; }
};

template<> class Limits <float>
{
  public:
    static float               maximum() { return FLT_MAX; }
    static float               minimum() { return FLT_MIN; }
    static float               epsilon() { return 1e-6f; }
    static bool                isZero (float a) { return (fabsf (a) < epsilon()); }
    static bool                isEqual (float a,  float b) { return Limits<float>::isZero (a - b); }
    static float               one() { return 1.0f; }
};

template<> class Limits <double>
{
  public:
    static double              maximum() { return DBL_MAX; }
    static double              minimum() { return DBL_MIN; }
    static double              epsilon() { return DBL_EPSILON; }
    static bool                isZero (double a) { return (abs (a) < epsilon()); }
    static bool                isEqual (double a, double b) { return isZero (a - b); }
    static double              one() { return 1.0; }
};
}

#endif
