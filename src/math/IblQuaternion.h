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

#ifndef INCLUDED_BB_QUATERNION_INCLUDED
#define INCLUDED_BB_QUATERNION_INCLUDED

#include <IblPlatform.h>
#include <IblVector3.h>

namespace Ibl
{
template <typename T>
class Quaternion
{
  public:
    T x, y, z, w;
    Quaternion <T>() : w(1.0), x(0.0), y(0.0), z(0.0)
        {}

    Quaternion <T>( T _x, T _y, T _z, T _w ) :
        w(_w), x(_x), y(_y), z(_z)
        {}

    Quaternion<T> (T rx, T ry, T rz)
    {
        T cos_z_2 = (T)(cosf(0.5f*(float)(rz)));
        T cos_y_2 = (T)(cosf(0.5f*(float)(ry)));
        T cos_x_2 = (T)(cosf(0.5f*(float)(rx)));

        T sin_z_2 = (T)(sinf(0.5f*(float)(rz)));
        T sin_y_2 = (T)(sinf(0.5f*(float)(ry)));
        T sin_x_2 = (T)(sinf(0.5f*(float)(rx)));

        w = cos_z_2*cos_y_2*cos_x_2 + sin_z_2*sin_y_2*sin_x_2;
        x = cos_z_2*cos_y_2*sin_x_2 - sin_z_2*sin_y_2*cos_x_2;
        y = cos_z_2*sin_y_2*cos_x_2 + sin_z_2*cos_y_2*sin_x_2;
        z = sin_z_2*cos_y_2*cos_x_2 - cos_z_2*sin_y_2*sin_x_2;
    }
    
    Quaternion<T>(const Quaternion<T>& other) :
        x (other.x), y (other.y), z (other.z), w (other.w)
        {}

    Quaternion<T>& operator=(const Quaternion<T>& other)
    {
        if (this != &other)
        {
            x = other.x; 
            y = other.y; 
            z = other.z; 
            w = other.w;
        }
        return *this;
    }

    inline void setIdentity()
    { x = y = z = 0.0; w = 1.0;}

    Quaternion<T> operator-() const
    { return Quaternion<T> ( -x, -y, -z, -w,);}

    inline bool                operator == (const Quaternion<T>& other) const
    {
        // Matrix44 uses memcpy for the product graph.
        // This is more applicable in a general sense.
        if (Ibl::Limits<T>::isEqual(other.x, x) &&
            Ibl::Limits<T>::isEqual(other.y, y) &&
            Ibl::Limits<T>::isEqual(other.z, z) &&
            Ibl::Limits<T>::isEqual(other.w, w))
        {
            return true;
        }
        return false;
    }

    Quaternion<T>& operator *=(const Quaternion<T>& q)
    {
        float qx, qy, qz, qw;
        qx = x;
        qy = y;
        qz = z;
        qw = w;

        x = qw * q.x + qx * q.w + qy * q.z - qz * q.y;
        y = qw * q.y - qx * q.z + qy * q.w + qz * q.x;
        z = qw * q.z + qx * q.y - qy * q.x + qz * q.w;
        w = qw * q.w - qx * q.x - qy * q.y - qz * q.z;

        return *this;
    } 

    void operator*=(const Vector3<T>& v)
    {
        T qx = x;
        T qy = y;
        T qz = z;
        T qw = w;

        x = qw * v.x + qy * v.z - qz * v.y;
        y = qw * v.y - qx * v.z + qz * v.x;
        z = qw * v.z + qx * v.y - qy * v.x;
        w = - qx * v.x - qy * v.y - qz * v.z;
    }

    Vector3<T> toEuler(bool homogenous=true) const
    {
        T sqw = w*w;
        T sqx = x*x; 
        T sqy = y*y;
        T sqz = z*z;

        Vector3<T> euler;
        if (homogenous)
        {
            euler.x = (T)atan2f(2.f * (x*y + z*w), sqx - sqy - sqz + sqw);
            euler.y = (T)asinf(-2.f * (x*z - y*w));
            euler.z = (T)atan2f(2.f * (y*z + x*w), -sqx - sqy + sqz + sqw);
        } 
        else 
        {
            euler.x = (T)atan2f(2.f * (z*y + x*w), 1 - 2*(sqx + sqy));
            euler.y = (T)asinf(-2.f * (x*z - y*w));
            euler.z = (T)atan2f(2.f * (x*y + z*w), 1 - 2*(sqy + sqz));
        }
        return euler;
    }

    friend std::ostream& 
    operator << (std::ostream &s, const Quaternion<T>& q)
    {
        return (s << " x = " << q.x << " y = " << q.y << " z = " << q.z << " w = " << q.w);
    }

};
typedef Quaternion<typename float> Quaternionf;
}

#endif
