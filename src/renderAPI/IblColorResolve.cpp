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
#include <IblColorResolve.h>
#include <IblITexture.h>
#include <IblIShader.h>
#include <IblIEffect.h>
#include <IblGpuVariable.h>

namespace Ibl
{
ColorResolve::ColorResolve(Ibl::IDevice* device) : PostEffect (device, Ibl::PF_UNKNOWN)
{
}

ColorResolve::~ColorResolve()
{
}

bool ColorResolve::free()
{
    return PostEffect::free();
}

bool
ColorResolve::cache()
{
    _effect = _shader->effect();


    _shader->getTechniqueByName(std::string ("postProcessArray"), _arrayColorResolve);
    _shader->getTechniqueByName(std::string ("postProcess"), _colorResolve);
    _shader->getTechniqueByName(std::string ("postProcess2"), _colorResolve2);
    _shader->getTechniqueByName(std::string ("postProcess4"), _colorResolve4);
    _shader->getTechniqueByName(std::string ("postProcess8"), _colorResolve8);
        
    //we have one offscrean texture already created.
    _shader->getParameterByName (std::string ("sourceWidth"), _sourceWidthVariable);
    _shader->getParameterByName (std::string ("sourceHeight"), _sourceHeightVariable);
    _shader->getParameterByName (std::string ("arrayOffset"), _arrayOffset);

    _shader->getParameterByName (std::string ("color1"), _colorSource);
    _shader->getParameterByName (std::string ("color1Array"), _colorSourceArray);

    _shader->getParameterByName (std::string ("color2"), _colorSource2);
    _shader->getParameterByName (std::string ("color4"), _colorSource4);
    _shader->getParameterByName (std::string ("color8"), _colorSource8);

    return PostEffect::cache();
}

bool
ColorResolve::render (const Ibl::ISurface* dest, 
                      const Ibl::ITexture* source,
                      const Ibl::Camera* camera,
                      size_t arrayOffset)
{
    // Set Alpha blend enable to false
    {
        const Ibl::GpuTechnique* ColorResolveTechnique = 0;
        Ibl::FrameBuffer framebuffer (dest, nullptr);
        int sampleCount = source->multiSampleCount();

        if (dest == source->surface())
            LOG ("WHAT? NO. Source cannot be == to Destination");

        Ibl::Viewport viewport;
        _device->getViewport(&viewport);

        _device->bindFrameBuffer(framebuffer);
        _device->setupViewport(framebuffer);

        float width = float(source->width());
        float height = float(source->height());

        float arrayOffsetIn = (float)(arrayOffset);
        _sourceWidthVariable->set(&width, sizeof(float));
        _sourceHeightVariable->set(&height, sizeof(float));
        _arrayOffset->set(&arrayOffsetIn, sizeof(float));

        ColorResolveTechnique = _arrayColorResolve;
        _colorSourceArray->setTexture(source);
        
        _device->disableDepthWrite();
        _device->disableZTest();
        _device->disableStencilTest();

        _shader->renderMesh (Ibl::RenderRequest(ColorResolveTechnique,nullptr,  nullptr, this));

        _device->setViewport(&viewport);

        _device->enableDepthWrite();
        _device->enableZTest();
        _device->enableStencilTest();
    }
    return true;
}

bool
ColorResolve::render (const Ibl::ISurface* dest, 
                      const Ibl::ITexture* source,
                      const Ibl::Camera* camera)
{
    // Set Alpha blend enable to false
    {
        const Ibl::GpuTechnique* ColorResolveTechnique = 0;
        Ibl::FrameBuffer framebuffer (dest, nullptr);
        int sampleCount = source->multiSampleCount();

        if (dest == source->surface())
            LOG ("WHAT? NO. Source cannot be == to Destination");

        Ibl::Viewport viewport;
        _device->getViewport(&viewport);

        _device->bindFrameBuffer(framebuffer);
        _device->setupViewport(framebuffer);

        float width = float(source->width());
        float height = float(source->height());

        _sourceWidthVariable->set(&width, sizeof(float));
        _sourceHeightVariable->set(&height, sizeof(float));

        switch (sampleCount)
        {
            case 1:
            {    
                ColorResolveTechnique = _colorResolve;
                _colorSource->setTexture(source);
                break;
            }
            case 2:
            {
                ColorResolveTechnique = _colorResolve2;
                _colorSource2->setTexture(source);
                break;
            }
            case 4:
            {
                ColorResolveTechnique = _colorResolve4;
                _colorSource4->setTexture(source);
                break;
            }
            case 8:
            {
                ColorResolveTechnique = _colorResolve8;
                _colorSource8->setTexture(source);
                break;
            }
            default:
            {
                LOG ("Unknown sample count for depth resolve: " << sampleCount);
                return false;
            }
        }

        _device->disableDepthWrite();
        _device->disableZTest();
        _device->disableStencilTest();

        _shader->renderMesh (Ibl::RenderRequest(ColorResolveTechnique,nullptr,  nullptr, this));

        _device->setViewport(&viewport);

        _device->enableDepthWrite();
        _device->enableZTest();
        _device->enableStencilTest();
    }
    return true;
}

}