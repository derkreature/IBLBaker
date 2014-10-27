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

#ifndef INCLUDED_BB_MATRIX_ALGO
#define INCLUDED_BB_MATRIX_ALGO

#include <IblPlatform.h>
#include <IblMath.h>

namespace Ibl
{
inline void
viewMatrixLH (const Ibl::Vector3f& eye, const Ibl::Vector3f& lookAt, const Ibl::Vector3f& up, Ibl::Matrix44f* m)
{
    // compute view vectors
    Vector3f view = lookAt - eye;
    view.normalize();

    Vector3f viewRight;
    Vector3f viewUp;
    
    viewUp = up - up.dot (view) * view;
    viewUp.normalize();

    viewRight = view.cross (viewUp);

    Matrix44f& matrix = *m;

    matrix.set(-viewRight, viewUp, view);

    Vector3f eyeInv = -(matrix.transform(eye));

    matrix[3][0] = eyeInv.x;
    matrix[3][1] = eyeInv.y;
    matrix[3][2] = eyeInv.z;

    *m = matrix;
}

inline void
projectionPerspectiveMatrixLH (float fovy, float aspect, float zn, float zf, Matrix44f* m)
{
    Matrix44f& perspective = *m;
    perspective.setIdentity();
    perspective[0][0] = 1.0f / (aspect * tan(fovy/2.0f));
    perspective[1][1] = 1.0f / tan(fovy/2.0f);
    perspective[2][2] = zf / (zf - zn);
    perspective[3][2] = -((zf * zn) / (zf - zn));
    perspective[2][3] = 1;
    perspective[3][3] = 0.0f;
}

// Todo, offcenter.
inline void
projectionPerspectiveMatrixLH (float fovy, float fovx, float aspect, float zn, float zf, Matrix44f* m)
{
    Matrix44f& perspective = *m;
    perspective.setIdentity();

    float w, h, Q;

    w = (float)1.0f/tan(fovx*0.5f);  // 1/tan(x) == cot(x)
    h = (float)1.0f/tan(fovy*0.5f);   // 1/tan(x) == cot(x)
    Q = zf/(zf- zn);

    perspective[0][0] = w;
    perspective[1][1] = h;
    perspective[2][2] = Q;
    perspective[3][2] = -Q*zn;
    perspective[2][3] = 1;
    perspective[3][3] = 0.0f;
}


template <typename T>
inline
void vecTransform(Vector3<T>& v, const Quaternion<T>& q)
{
    Quaternion<T> temp = Quaternion<T>(-q.x, -q.y, -q.z, q.w);
    temp *= v;
    temp *= q;

    v.x = temp.x;
    v.y = temp.y;
    v.z = temp.z;
}

template <typename T>
inline
Ibl::Vector3<T> vecQuatTransform(const Vector3<T>& in, const Quaternion<T>& q)
{
    Ibl::Vector3<T> out = in;
    Quaternion<T> temp = Quaternion<T>(q.x, q.y, q.z, q.w);

    temp *= out;
    temp *= Quaternion<T>(-q.x, -q.y, -q.z, q.w);

    out.x = temp.x;
    out.y = temp.y;
    out.z = temp.z;
    return out;
}

template <typename T>
inline
Ibl::Vector3<T> vecQuatTransformInverse(const Vector3<T>& in, const Quaternion<T>& q)
{
    Ibl::Vector3<T> out = in;
    Quaternion<T> temp = Quaternion<T>(-q.x, -q.y, -q.z, q.w);
    temp *= out;
    temp *= q;

    out.x = temp.x;
    out.y = temp.y;
    out.z = temp.z;
    return out;
}

}



#endif