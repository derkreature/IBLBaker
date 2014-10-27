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
#include <IblDepthResolve.h>
#include <IblITexture.h>
#include <IblIShader.h>
#include <IblIEffect.h>
#include <IblGpuVariable.h>

namespace Ibl
{
DepthResolve::DepthResolve(Ibl::IDevice* device) : PostEffect (device, Ibl::PF_UNKNOWN)
{
}

DepthResolve::~DepthResolve()
{
}

bool DepthResolve::free()
{
    return PostEffect::free();
}

bool
DepthResolve::cache()
{
    _effect = _shader->effect();

    _shader->getTechniqueByName(std::string ("postProcess"), _depthResolve);
    _shader->getTechniqueByName(std::string ("postProcess2"), _depthResolve2);
    _shader->getTechniqueByName(std::string ("postProcess4"), _depthResolve4);
    _shader->getTechniqueByName(std::string ("postProcess8"), _depthResolve8);
        
    //we have one offscrean texture already created.
    _shader->getParameterByName (std::string ("sourceWidth"), _sourceWidthVariable);
    _shader->getParameterByName (std::string ("sourceHeight"), _sourceHeightVariable);

    _shader->getParameterByName (std::string ("depth"), _depthSource);
    _shader->getParameterByName (std::string ("depth2"), _depthSource2);
    _shader->getParameterByName (std::string ("depth4"), _depthSource4);
    _shader->getParameterByName (std::string ("depth8"), _depthSource8);

    return PostEffect::cache();
}

bool
DepthResolve::render (const Ibl::IDepthSurface* dest, 
                      const Ibl::IDepthSurface* source,
                      const Ibl::Camera* camera)
{
    // Set Alpha blend enable to false
    {
        const Ibl::GpuTechnique* depthResolveTechnique = 0;
        Ibl::FrameBuffer framebuffer (nullptr, dest);
        int sampleCount = source->multiSampleCount();
        _device->bindFrameBuffer(framebuffer);

        float width = float(source->width());
        float height = float(source->height());

        _sourceWidthVariable->set(&width, sizeof(float));
        _sourceHeightVariable->set(&height, sizeof(float));

        switch (sampleCount)
        {
            case 1:
            {    
                depthResolveTechnique = _depthResolve;
                _depthSource->setDepthTexture(source);
                break;
            }
            case 2:
            {
                depthResolveTechnique = _depthResolve2;
                _depthSource2->setDepthTexture(source);
                break;
            }
            case 4:
            {
                depthResolveTechnique = _depthResolve4;
                _depthSource4->setDepthTexture(source);
                break;
            }
            case 8:
            {
                depthResolveTechnique = _depthResolve8;
                _depthSource8->setDepthTexture(source);
                break;
            }
            default:
            {
                LOG ("Unknown sample count for depth resolve: " << sampleCount);
                return false;
            }
        }

        _device->enableDepthWrite();
        _device->enableZTest();
        _device->disableStencilTest();

        _shader->renderMesh (Ibl::RenderRequest(depthResolveTechnique, nullptr, nullptr, this));
    }
    return true;
}

}