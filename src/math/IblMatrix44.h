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

#ifndef INCLUDED_BB_MATRIX44_INCLUDED
#define INCLUDED_BB_MATRIX44_INCLUDED

#include <IblPlatform.h>
#include <IblMath.h>
#include <IblQuaternion.h>
#include <IblVector3.h>
#include <IblVector4.h>

namespace Ibl
{
template <typename  T>
class Matrix44
{
public:
    union
    {
        T _mat [16];
        T _m [4][4];
        struct {
            T        _11, _12, _13, _14;
            T        _21, _22, _23, _24;
            T        _31, _32, _33, _34;
            T        _41, _42, _43, _44;
        };
    };

    Matrix44<T> () {setIdentity();}

    Matrix44<T> (const Quaternion<T>& Quaternion)
    {
        rotation (Quaternion);
    }

    Matrix44 (const Matrix44<T>& other)
    {
        memcpy(&_mat[0], &other._mat[0], sizeof(T) * 16);
    }

    inline T *                 operator [] (int i) { return _m[i]; }
    inline const T *           operator [] (int i) const { return _m[i]; }

    Matrix44<T>&               operator=(const Matrix44<T>& other)
    {
        if ( this == &other )
            return *this;
        memcpy(&_mat[0], &other._mat[0], sizeof(T) * 16);
        return *this;
    } 

    inline void                setIdentity()
    {
        memset(&_mat[0], 0, sizeof(T) * 16);
        _mat[0] = Ibl::Limits<typename T>::one();
        _mat[5] = Ibl::Limits<typename T>::one();
        _mat[10] = Ibl::Limits<typename T>::one();
        _mat[15] = Ibl::Limits<typename T>::one();
    } 

    Matrix44<T>& 
    transpose()
    {
        T src [4][4];
        memcpy (&src[0][0], &_m[0][0], 16 * sizeof(T));

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                _m[i][j] = src[j][i];
            }
        }
        return *this;
    }

    Matrix44<T>&               setTranslation (const Vector3<T>& xlate)
    {
        setIdentity();
        _mat[12] = xlate.x; _mat[13] = xlate.y; _mat[14] = xlate.z;
        return *this;
    } 

    inline Vector3<T>          translation() const
    {
        return Vector3<T>(_mat[12], _mat[13], _mat[14]);
    }

    Matrix44<T>&               rotation (const Quaternion<T>& rotate)
    {
        T xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

        xs = rotate.x+rotate.x; ys = rotate.y+rotate.y; zs = rotate.z+rotate.z;
        wx = rotate.w*xs;       wy = rotate.w*ys;       wz =  rotate.w*zs;
        xx = rotate.x*xs;       xy = rotate.x*ys;       xz = rotate.x*zs;
        yy = rotate.y*ys;       yz = rotate.y*zs;       zz = rotate.z*zs;

        _mat[0] = 1.0f - (yy + zz); _mat[4] = xy - wz;          _mat[8] = xz + wy;           _mat[12] = 0.0f;
        _mat[1] = xy + wz;          _mat[5] = 1.0f - (xx + zz); _mat[9] = yz - wx;           _mat[13] = 0.0f;
        _mat[2] = xz - wy;          _mat[6] = yz + wx;          _mat[10] = 1.0f - (xx + yy); _mat[14] = 0.0f;
        _mat[3] = 0.0f;             _mat[7] = 0.0f;             _mat[11] = 0.0f;             _mat[15] = 1.0f;

        return *this;
    } 

    Matrix44<T>& 
    scaling (const Vector3<T>& scaleFactors)
    {
        setIdentity();

        _m[0][0] = scaleFactors.x;
        _m[1][1] = scaleFactors.y;
        _m[2][2] = scaleFactors.z;

        return *this;
    } 

    Matrix44<T>& 
    setRotationX( T angle )
    {
        T sx = (T)sinf((float)(angle));
        T cx = (T)cosf((float)(angle));

        setIdentity();

        _mat[5] = cx;
        _mat[6] = sx;
        _mat[9] = -sx;
        _mat[10] = cx;

        return *this;
    } 

    Matrix44<T>& 
    setRotationY( T angle )
    {
        T sy = (T)sinf((float)(angle));
        T cy = (T)cosf((float)(angle));

        setIdentity();

        _mat[0] = cy;
        _mat[2] = -sy;
        _mat[8] = sy;
        _mat[10] = cy;
        return *this;
    }

    Matrix44<T>& 
    setRotationZ( T angle )
    {
        T sz = (T)sinf((float)(angle));
        T cz = (T)cosf((float)(angle));

        setIdentity();

        _mat[0] = cz;
        _mat[1] = sz;
        _mat[4] = -sz;
        _mat[5] = cz;

        return *this;
    } 

    inline bool                operator == (const Matrix44<T>& other) const
    {
        return memcmp(_mat, other._mat, sizeof(float) * 16) == 0;
    }

    Matrix44<T>                operator*( const Matrix44<T>& other ) const
    {
        Matrix44<T> result;
        for (uint32_t i=0; i<4; i++)
        {
            for (uint32_t j=0; j<4; j++)
            {
                result._m[i][j] = _m[i][0] * other._m[0][j] + 
                                  _m[i][1] * other._m[1][j] + 
                                  _m[i][2] * other._m[2][j] + 
                                  _m[i][3] * other._m[3][j];
            }
        }
                        
        return result;
    }


    Vector4<T>
    transform( const Vector4<T>& other ) const
    {
        Vector4<T> result;
        result.x = _mat[0]*other.x + _mat[4]*other.y + _mat[8]*other.z + _mat[12]*other.w;
        result.y = _mat[1]*other.x + _mat[5]*other.y + _mat[9]*other.z + _mat[13]*other.w;
        result.z = _mat[2]*other.x + _mat[6]*other.y + _mat[10]*other.z + _mat[14]*other.w;
        result.w = _mat[3]*other.x + _mat[7]*other.y + _mat[11]*other.z + _mat[15]*other.w;
        return result;
    }

    Vector3<T>                 transform( const Vector3<T>& other ) const
    {
        Vector3<T> result;
        result.x = _mat[0]*other.x + _mat[4]*other.y + _mat[8]*other.z + _mat[12];
        result.y = _mat[1]*other.x + _mat[5]*other.y + _mat[9]*other.z + _mat[13];
        result.z = _mat[2]*other.x + _mat[6]*other.y + _mat[10]*other.z + _mat[14]; 
        return result;
    }

    void                       set ( const Vector3<T>& row1, const Vector3<T>& row2, const Vector3<T>& row3 )
    {
        setIdentity();

        _mat[0] = row1.x; _mat[4] = row1.y; _mat[8] = row1.z;
        _mat[1] = row2.x; _mat[5] = row2.y; _mat[9] = row2.z;
        _mat[2] = row3.x; _mat[6] = row3.y; _mat[10] = row3.z;
    }
    
    inline void                makeOrthoLH(T w, T h, T zn, T zf)
    {
        memset(&_m[0][0], 0, sizeof(T) * 16);
        float q = 1.0f / (zf - zn);

        _m[0][0] = 2.0f  /w;
        _m[1][1] =  2.0f / h;
        _m[2][2] = q;
        _m[2][3] = -zn / (zf - zn); // -zn?
        _m[3][3] = 1;
    }

    inline void                makeOrthoOffCenterLH(T l, T r, T b, T t, T zn, T zf)
    {
        setIdentity();
        _m[0][0] = 2.0f / (r - l);
        _m[1][1] = 2.0f / (t - b);
        _m[2][2] = 1.0f / (zf -zn);
        _m[3][0] = -1.0f -2.0f *l / (r - l);
        _m[3][1] = 1.0f + 2.0f * t / (b - t);
        _m[3][2] = zn / (zn -zf);
    }

    inline void                makeOrthoOffCenterRH(float l, float r, float b, float t, float zn, float zf)
    {
        setIdentity();
        _m[0][0] = 2.0f / (r - l);
        _m[1][1] = 2.0f / (t - b);
        _m[2][2] = 1.0f / (zn -zf);
        _m[3][0] = -1.0f -2.0f *l / (r - l);
        _m[3][1] = 1.0f + 2.0f * t / (b - t);
        _m[3][2] = zn / (zn -zf);
    }

    inline void                makeOrthoRH(T w, T h, T zn, T zf)
    {
        setIdentity();
        _m[0][0] = 2.0f / w;
        _m[1][1] = 2.0f / h;
        _m[2][2] = 1.0f / (zn - zf);
        _m[3][2] = zn / (zn - zf);
    }
};

typedef Matrix44<float> Matrix44f;
}

#endif