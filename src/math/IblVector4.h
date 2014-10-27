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

#ifndef INCLUDED_VECTOR_4
#define INCLUDED_VECTOR_4

#include <IblPlatform.h>
#include <IblMath.h>

namespace Ibl
{
template <typename T>
class Vector4
{
  public:
    T                          x;
    T                          y;
    T                          z;
    T                          w;

    Vector4<T> () : x(0), y(0), z(0) {  };
    Vector4<T> (T nx, T ny, T nz, T nw ) : x (nx), y (ny), z (nz), w (nw) {}
    Vector4<T> (T init) : x (init), y (init), z (init), w (init) {}
    Vector4<T> (const Vector4<T>& other) { x = other.x; y = other.y; z = other.z; w = other.w; }
    Vector4<T>& operator = (const Vector4<T>& other)
    {
        if (this == &other )
            return *this;
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }

    static uint32_t             dimensions() {return 4;}

    inline bool                operator == (const Vector4<T>& other) const
    {
        // Matrix44 uses memcpy for the product graph.
        // This is more applicable in a general sense.
        if (Ibl::Limits<T>::isEqual(other.x, x) &&
            Ibl::Limits<T>::isEqual(other.y, y) &&
            Ibl::Limits<T>::isEqual(other.z, z) &&
            Ibl::Limits<T>::isEqual(other.w, w) )
        {
            return true;
        }
        return false;
    }

    inline T&                   operator[] (size_t i) { return (&x)[i]; }
    inline T                    operator[] (size_t i) const  { return (&x)[i]; }

    Vector4<T>                  operator + ( const Vector4<T>& other) const
    {
        return Vector4<T>( x + other.x, y + other.y, z + other.z, w + other.w );
    }

    friend Vector4<T>&          operator += (Vector4<T>& a, 
                                             const Vector4<T>& b)
    {
        a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
        return a;
    }

    Vector4<T>                  operator - (const Vector4<T>& b) const
    { 
        return Vector4( x - b.x, y - b.y, z - b.z, w - b.w); 
    }

    friend Vector4<T>&          operator -= (Vector4<T>& a, 
                                             const Vector4<T>& b)
    { 
        a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w; return a;
    }

    Vector4<T>                  operator - () const
    { 
        return Vector4(-x, -y, -z, w); 
    }

    Vector4<T>                  operator *  (T scalar)
    { 
        return Vector4<T> (scalar*x, scalar*y, scalar*z, scalar*w); 
    }

    friend Vector4<T>           operator *  (T scalar, const Vector4<T>& a)
    { 
        return Vector4<T> (scalar*a.x, scalar*a.y, scalar*a.z, scalar * a.w ); 
    } 

    Vector4<T>&                 operator *= (T scalar)
    { 
        x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; 
    }

    Vector4<T>                  operator /  (T scalar)
    { 
        return Vector4<T>(x/scalar, y/scalar, z/scalar, w/scalar); 
    }

    Vector4<T>&                 operator /= (T scalar)
    { 
        x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this; 
    }

    T                           dot (const Vector4<T>& b) const
    {
        return (x * b.x) + (y * b.y) + (z * b.z) + (w * b.w);
    }

    inline T                    distance (const Vector4<T>& b) const
    {
        return sqrt( distanceSquared (b) );
    }

    inline T                    distanceSquared (const Vector4<T>& b) const
    {
        T nx = x - b.x;
        T ny = y - b.y;
        T nz = z - b.z;
        T nw = w - b.w;
        return nx*nx + ny*ny + nz*nz + nw*nw;
    }

    static Vector4<T> lerp (const Vector4<T>& from, const Vector4<T>& to, float slerp)
    {
        Vector4<T> result = to-from;
        result *= slerp;
        result += from;
        return result;
    };

    friend std::ostream& 
    operator << (std::ostream &s, const Vector4<T>& v)
    {
        return (s << "x = " << v.x << " y = " << v.y << " z = " << v.z " w = " << v.);
    }
};

typedef Ibl::Vector4<float> Vector4f;
}

#endif