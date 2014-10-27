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

#ifndef INCLUDED_VECTOR2
#define INCLUDED_VECTOR2

#include <IblPlatform.h>

namespace Ibl
{
template <typename T>
class Vector2
{
  public:  
    T                          x;
    T                          y;

    Vector2<T> (T nx, T ny) : x(nx), y(ny) {}
    Vector2<T> (T all = 0) : x(all), y(all) {}
    Vector2<T> (const Vector2<T>& other) { memcpy(&x, &other.x, sizeof(T) * 2); }

    inline T&                   operator[] (size_t i)
        { return (&x)[i]; }

    inline T                    operator[] (size_t i) const 
        { return (&x)[i]; }

    static uint32_t            dimensions() { return 2; }
    static Vector2<T>          baseTypeMax() { return Vector2<T>(Limits<T>::maximum()); }
    static Vector2<T>          baseTypeMin() { return Vector2<T>(Limits<T>::minimum()); }

    inline const Vector2<T> &  operator -= (const Vector2<T> &v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    inline Vector2<T>          operator-(const Vector2<T> &v) const
    {
        return Vector2 (x - v.x, y - v.y);
    }

    inline Vector2<T>          operator+(const Vector2<T> &v) const
    {
        return Vector2 (x + v.x, y + v.y);
    }

    inline Vector2<T>          operator/(T v) const
    {
        return Vector2 (x / v, y / v);
    }

    inline void                operator*=(T s)
    {
        x *= s;
        y *= s;
    }

    inline Vector2<T>          operator-() const
    {
        return Vector2 (-x, -y);
    }

    Vector2<T>&                operator=(const Vector2<T>& other)
    {
        if (this == &other )
            return *this;        
        x = other.x;
        y = other.y;
        return *this;
    }

    inline void                operator+=(const Vector2<T>& v)
    {
        x += v.x;
        y += v.y;
    }

    inline void                operator*=(const Vector2<T>& v)
    {
        x *= v.x;
        y *= v.x;
    }

    inline bool                operator == (const Vector2<T>& other) const
    {
        return (Ibl::Limits<T>::isEqual (other.x, x) &&
                Ibl::Limits<T>::isEqual (other.y, y));
    }

    inline bool                operator != (const Vector2<T>& other) const
    {
        if (Ibl::Limits<T>::isEqual (other.x, x) && 
            Ibl::Limits<T>::isEqual (other.y, y))
        {
            return false;
        }
        return true;
    }

    friend inline Vector2<T>   operator*(const Vector2<T>& v, T s)
    {
        Vector2<T> result = *this;
        result *= s;
        return result;
    }

    friend inline Vector2<T>   operator*(T s, const Vector2<T>& v)
    {
        Vector2<T> result = *this;
        result *= s;
        return result;
    }

    friend inline Vector2<T>   operator*(const Vector2<T>& v1, const Vector2<T>& v2)
    {
        return Vector2<T>(v1.x*v2.x, v1.y*v2.y);
    }

    friend std::ostream& 
    operator << (std::ostream &s, const Vector2<T>& v)
    {
        return (s << "x = " << v.x << " y = " << v.y);
    }
};

typedef Vector2<int> Vector2i;
typedef Vector2<float> Vector2f;
}

#endif