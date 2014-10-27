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
#ifndef INCLUDED_BB_FRAMEBUFFER
#define INCLUDED_BB_FRAMEBUFFER

#include <IblPlatform.h>

namespace Ibl
{
class ISurface;
class IDepthSurface;
class IGpuBuffer;
class Surface;
class IDepthSurface;
class IRenderResource;
//-----------------------------------------------
// Caches frame buffer attacments for direct3d.
// Allows device to track invalid attachements
// and correct / warn against invalid selections.
//-----------------------------------------------
class FrameBuffer 
{
  public:
    FrameBuffer();
    FrameBuffer(const FrameBuffer&);
    FrameBuffer(const Ibl::ISurface* colorSurface,
                const Ibl::IDepthSurface* depthSurface);
    ~FrameBuffer();

    void                       setColorSurface (size_t index, 
                                                const Ibl::ISurface* surface);
    void                       setDepthSurface(const Ibl::IDepthSurface* surface);
    void                       setUnorderedSurface (size_t index, 
                                                    const Ibl::IRenderResource* unorderedSurface);

    const Ibl::IDepthSurface*   depthSurface () const;
    const Ibl::ISurface *       colorSurface (size_t index) const;
    const Ibl::IRenderResource* unorderedSurface(size_t index) const;

  protected:
    const Ibl::ISurface*            _colorSurfaces[4];
    const Ibl::IDepthSurface*       _depthSurface;
    const Ibl::IRenderResource *    _unorderedSurfaces[4];
};
}

#endif