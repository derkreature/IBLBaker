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
#ifndef INCLUDED_BB_RENDER_PASS
#define INCLUDED_BB_RENDER_PASS

#include <IblPlatform.h>
#include <IblNode.h>
#include <IblTypedProperty.h>
#include <IblRenderEnums.h>
#include <IblIRenderResource.h>

namespace Ibl
{
class Scene;
class IDevice;

enum TransformSpace
{
    CameraViewProjSpace = (1<<0),
    LightViewProjSpace  = (1<<1),
    DualParaboloidSpace = (1<<3),
    CubeMapSpace        = (1<<4),
    UV                  = (1<<5),
    NoTransform         = (1<<6)
};


class RenderPass : public IRenderResource, public Ibl::Node
{
  public:
    RenderPass (Ibl::IDevice* device);
    virtual ~RenderPass();

    void                       setCullMode (Ibl::CullMode cullMode) {
                                   _cullMode = cullMode;
    }
    Ibl::CullMode               cullMode() const {
                                   return _cullMode;
    }

    virtual void               render (Scene* scene) = 0;
    const std::string&         passName() const;

    void                       renderMeshes(const std::string& passName, 
                                            const Ibl::Scene* scene);

  protected:

    Ibl::CullMode               _cullMode;
    bool                       _enabled;
    std::string                _passName;
};

}
#endif
