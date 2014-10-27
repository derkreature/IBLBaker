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

#ifndef INCLUDED_BB_VECTOR3_INCLUDED
#define INCLUDED_BB_VECTOR3_INCLUDED

#include <IblPlatform.h>
#include <IblMath.h>
#include <IblVector4.h>
#include <IblLimits.h>

namespace Ibl
{

template <typename T>
class Vector3
{
  public:
    T                          x;
    T                          y;
    T                          z; 

    Vector3<T> (T all = 0) : x(all), y(all), z(all) {}
    Vector3<T> (T nx, T ny, T nz ) : x(nx), y(ny), z(nz){}
    Vector3<T> (T nx, T ny) : x(nx), y(ny), z(0){}
    Vector3<T> (const Vector3<T>& other) { memcpy (&x, &other.x, sizeof(T) * 3); }

    Vector3<T>& operator = (const Vector3<T>& other)
    {
        if (this != &other)
        {
            memcpy (&x, &other.x, sizeof(T) * 3);
        }
        return *this;
    }

    static Vector3<T>          baseTypeMax() { return Vector3<T>(Limits<T>::maximum()); }
    
    static Vector3<T>          baseTypeMin() { return Vector3<T>(Limits<T>::minimum()); }

    inline T&                  operator[] (unsigned int i) { return (&x)[i]; }
    inline T                   operator[] (unsigned int i) const { return (&x)[i]; }
    inline T                   length() const { return Ibl::sqrt( x*x + y*y + z*z); }
    inline T                   lengthSquared() const { return (x*x + y*y + z*z); }
    inline T                   distance (const Vector3<T>& b) const { return Ibl::sqrt( distanceSquared (b) ); }

    inline T                   distanceSquared (const Vector3<T>& b) const
    {
        T nx = x - b.x;
        T ny = y - b.y;
        T nz = z - b.z;
        return nx*nx + ny*ny + nz*nz;
    }

    inline bool                operator == (const Vector3<T>& other) const
    { 
        if (Ibl::Limits<T>::isEqual (other.x, x) && 
            Ibl::Limits<T>::isEqual (other.y, y) && 
            Ibl::Limits<T>::isEqual (other.z, z))
        {
            return true;
        }
        return false;
    }

    bool                       operator < (const Vector3<T>& other) const
    {
        if (x < other.x && y < other.y && z < other.z)
        {
            return true;
        }
        return false;
    }

    bool                       operator != (const Vector3<T>& other) const
    {
        if (Ibl::Limits<T>::isEqual (other.x, x)
            && Ibl::Limits<T>::isEqual (other.y, y)
            && Ibl::Limits<T>::isEqual (other.z, z))
        {
            return false;
        }
        return true;
    }

    bool                       isZero() const
        { return Limits<T>::isZero(x*x + y*y + z*z);}

    bool                       isUnit() const
        { return Limits<T>::isZero(1.0f - x*x - y*y - z*z);}

    inline void                set (T nx, T ny, T nz)
        { x = nx; y = ny; z = nz; }

    void                       normalize()   
    {   
        T lengthsq = lengthSquared();
        if (Ibl::Limits<T>::isEqual (lengthsq, (T)(0)))
        {
            x = 0;
            y = 0;
            z = 0;
        }
        else
        {
            T factor = (T)(1.0) / (T)sqrtf(lengthsq);
            x *= factor;
            y *= factor;
            z *= factor;
        }
    }

    Vector3<T>                 normalized() const
    {
        Vector3<T> out = *this;
        out.normalize();
        return out;
    }

    static const uint32_t      dimensions() { return 3; }

    inline bool                isZeroLength() const
    {
        T sqlen = (x * x) + (y * y) + (z * z);
        return (sqlen < (pow(1e-06, 2)));
    }

    Vector3<T>                 operator + ( const Vector3<T>& other) const
    {
        return Vector3<T>( x + other.x, y + other.y, z + other.z );
    }

    friend Vector3<T>&         operator += (Vector3<T>& a, 
                                             const Vector3<T>& b)
    {
        a.x += b.x; a.y += b.y; a.z += b.z;
        return a;
    }

    Vector3<T>                 operator - (const Vector3<T>& b) const { return Vector3( x - b.x, y - b.y, z - b.z ); }
    friend Vector3<T>&         operator -= (Vector3<T>& a, 
                                            const Vector3<T>& b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; return a;}
    Vector3<T>                 operator - () const { return Vector3(-x, -y, -z); }

    Vector3<T>                 operator * (const Vector3<T>& scalar) const
        { return Vector3<T> (scalar.x*x, scalar.y*y, scalar.z*z); }

    Vector3<T>                  operator *  (T scalar) { return Vector3<T> (scalar*x, scalar*y, scalar*z); }

    friend Vector3<T>           operator *  (T scalar, const Vector3<T>& a) { return Vector3<T> (scalar*a.x, scalar*a.y, scalar*a.z ); } 

    friend Vector3<T>           operator *  (const Vector3<T>& a, T scalar) { return Vector3<T> (scalar*a.x, scalar*a.y, scalar*a.z ); } 

    Vector3<T>&                 operator *= (T scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }

    Vector3<T>                  operator /  (T scalar) { return Vector3<T>(x/scalar, y/scalar, z/scalar); }

    Vector3<T>&                 operator /= (T scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

    T                           dot   (const Vector3<T>& vector) const { return (x*vector.x + y*vector.y + z*vector.z); }

    friend T                    dot   (const Vector3<T>& a, 
                                       const Vector3<T>& b) { return (a.x*b.x + a.y*b.y + a.z*b.z); }

    Vector3                     cross (const Vector3<T>& b) const { return Vector3<T> (y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x); }

    friend Vector3<T>           cross (const Vector3<T>& a, 
                                       const Vector3<T>& b) {  return Vector3<T> (a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x); }

    friend std::ostream& 
    operator << (std::ostream &s, const Vector3<T>& v)
    {
        return (s << "x = " << v.x << " y = " << v.y << " z = " << v.z);
    }
};

typedef Vector3<typename int> Vector3i;
typedef Vector3<typename float> Vector3f;
}

#endif
