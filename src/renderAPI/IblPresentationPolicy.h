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
#ifndef INCLUDED_BB_PRESENTATION_POLICY
#define INCLUDED_BB_PRESENTATION_POLICY

#include <IblPlatform.h>
#include <IblPostEffect.h>
#include <IblFrameBuffer.h>
#include <IblITexture.h>

namespace Ibl
{
class IDevice;
class Camera;

class PresentationPolicy : public PostEffect
{
  public:
    PresentationPolicy(Ibl::IDevice* device);
    virtual ~PresentationPolicy();

    virtual bool               prepareForBackBuffer (const Camera*);

    virtual bool               syncToBackBuffer (const Camera*, 
                                                 const ISurface* surface = nullptr);

    virtual const ITexture*    renderTarget() const = 0;
    virtual const ITexture*    displayTarget() const = 0;

    virtual const ISurface*    renderTargetSurface() const = 0; 
    virtual const ISurface*    displayTargetSurface() const = 0;

    virtual const ISurface*    fullSceneBackBufferSurface() const { return nullptr; }
    virtual const ISurface*    opaqueBackBufferSurface() const { return nullptr;}

    virtual const FrameBuffer& sceneFrameBuffer() const = 0;
    virtual bool               recreateOnResize() { return true; }

  protected:
    mutable FrameBuffer        _sceneFrameBuffer;
};
}

#endif