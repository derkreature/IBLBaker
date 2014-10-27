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

#ifndef INCLUDED_BB_BOX
#define INCLUDED_BB_BOX

#include <IblPlatform.h>
#include <IblVector2.h>
#include <IblVector3.h>
#include <IblVector4.h>

namespace Ibl
{
template <typename T>
class Region
{
  public:
    Region()
    {
    }

    Region(const T & position)
    {
        minExtent = position;
        maxExtent = position;
    }

    Region(const T & minExtentValue, 
           const T & maxExtentValue)
    {
        minExtent = minExtentValue;
        maxExtent = maxExtentValue;
    }

    bool            operator == (const Region<T> & src) const
    {
        return (minExtent == src.minExtent && maxExtent == src.maxExtent);
    }

    bool            operator != (const Region<T> & src) const
    {
        return (minExtent != src.minExtent || maxExtent != src.maxExtent);
    }

    T               size() const
    {
        return maxExtent - minExtent;
    }

    T               center() const
    {
        return (max + min)*0.5;
    }

    T               minExtent;
    T               maxExtent;
};

typedef Region < typename Vector2i > Region2i;
typedef Region < typename Vector2f > Region2f;
typedef Region < typename Vector3i > Region3i;
typedef Region < typename Vector3f > Region3f;
}

#endif

