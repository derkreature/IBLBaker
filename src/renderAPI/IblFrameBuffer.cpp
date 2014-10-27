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
#include <IblFrameBuffer.h>
#include <IblIGpuBuffer.h>

namespace Ibl
{
FrameBuffer::FrameBuffer()
{
    for (int i = 0; i < 4; i++)
    {
        _colorSurfaces [i] = nullptr;
        _unorderedSurfaces[i] = nullptr;
    }
    _depthSurface = nullptr;
}

FrameBuffer::FrameBuffer(const FrameBuffer& other)
{
    for (int i = 0; i < 4; i++)
    {
        _colorSurfaces [i] = other.colorSurface(i);
        _unorderedSurfaces[i] = other.unorderedSurface(i);
    }
    _depthSurface = other.depthSurface();
}

FrameBuffer::~FrameBuffer()
{
}

FrameBuffer::FrameBuffer(const Ibl::ISurface* colorSurface,
                         const Ibl::IDepthSurface* depthSurface)
{    
    for (int i = 0; i < 4; i++)
    {
        _colorSurfaces [i] = nullptr;
        _unorderedSurfaces[i] = nullptr;
    }

    _colorSurfaces[0] = colorSurface;
    _depthSurface = depthSurface;
}

void 
FrameBuffer::setColorSurface (size_t index, 
                              const Ibl::ISurface* surface)
{
    assert (index < 4);
    _colorSurfaces[index] = surface;
}

void 
FrameBuffer::setUnorderedSurface (size_t index, 
                                  const Ibl::IRenderResource* surface)
{
    assert (index < 4);
    _unorderedSurfaces[index] = surface;
}

void
FrameBuffer::setDepthSurface(const Ibl::IDepthSurface* surface)
{
    _depthSurface = surface;
}

const Ibl::IDepthSurface*
FrameBuffer::depthSurface () const
{
    return _depthSurface;
}

const Ibl::ISurface *
FrameBuffer::colorSurface (size_t index) const
{
    return _colorSurfaces[index];
}

const Ibl::IRenderResource* 
FrameBuffer::unorderedSurface(size_t index) const
{
    return _unorderedSurfaces[index];
}
}