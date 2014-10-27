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

#include <IblViewport.h>

namespace Ibl
{
Viewport::Viewport () :
_x (0),
_y (0),
_width (1),
_height (1),
_minZ (0),
_maxZ (1)
{
}

Viewport::Viewport (float x, float y, float width, float height, float minz, float maxz) :
_x (x),
_y (y),
_width (width),
_height (height),
_minZ (minz),
_maxZ (maxz)
{
}


Viewport::Viewport (uint32_t x, uint32_t y, uint32_t width, uint32_t height, float minz, float maxz) :
_x (float(x)),
_y (float(y)),
_width (float(width)),
_height (float(height)),
_minZ (minz),
_maxZ (maxz)
{
}

Viewport::Viewport(int32_t x, int32_t y, int32_t width, int32_t height, int32_t minz, int32_t maxz) :
_x(float(x)),
_y(float(y)),
_width(float(width)),
_height(float(height)),
_minZ(float(minz)),
_maxZ(float(maxz))
{
}

Viewport::Viewport (const Viewport& viewport)
{
    _x = viewport._x;
    _y = viewport._y;
    _width = viewport._width;
    _height = viewport._height;
    _minZ = viewport._minZ;
    _maxZ = viewport._maxZ;
}
}