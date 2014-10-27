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
#include <IblPostEffect.h>
#include <IblIShader.h>
#include <IblShaderMgr.h>
#include <IblGpuTechnique.h>
#include <IblISurface.h>
#include <IblLog.h>
#include <IblRegion.h>
#include <IblIGpuBuffer.h>

namespace Ibl
{
PostEffect::PostEffect (Ibl::IDevice* device, 
                        PixelFormat format,
                        LensDistortionType distortionType) :
Ibl::RenderTargetQuad (device),
_technique(0),
_shader(0),
_textureIn(0),
_postEffectTexture(nullptr),
_format (format),
_verticalPass (false)
{
    _name = "";
    
    RenderTargetQuad::initialize (Region2f());
    // This still needs work, and needs to be recalced when
    // when the backbuffer size changes
    if (_format != Ibl::PF_UNKNOWN)
    {
        Ibl::TextureParameters textureData  = 
            TextureParameters ("PostEffectTexture",
                             Ibl::TwoD,
                             Ibl::RenderTarget,
                             _format,
                             Ibl::Vector3i(MIRROR_BACK_BUFFER, MIRROR_BACK_BUFFER, 1));

        _postEffectTexture = _device->createTexture(&textureData);
    }

    _postEffectBounds = Ibl::Region2i(Ibl::Vector2i(0,0), Ibl::Vector2i(_device->backbuffer()->width(), _device->backbuffer()->height()));
}

PostEffect::~PostEffect()
{
    _device->destroyResource(_postEffectTexture);
}


bool
PostEffect::verticalPass () const
{
    return _verticalPass;
}

void
PostEffect::setVerticalPass (bool verticalPassValue)
{
    _verticalPass = verticalPassValue;
}

bool PostEffect::update(float felapsed)
{
    return true;
}

//--------------------------------------------------
// Gets the name of the target
//--------------------------------------------------
const std::string&  PostEffect::name() const
{
    return _name;
}

//--------------------------------------------------
//sets the name of the target
//--------------------------------------------------
void PostEffect::name(const std::string& name)
{
    _name = name;
}

bool PostEffect::free()
{
    //reset render surface
    return RenderTargetQuad::free();
}

bool
PostEffect::create()
{
    return RenderTargetQuad::create();
}

bool
PostEffect::cache()
{
    if (_shader)
    {
        if (!_shader->getTechniqueByName ("postprocess", _technique))
        {
            LOG ("couldn't find default postprocess technique for file" << _filename);
        }
    }

    return RenderTargetQuad::cache();
}


const Ibl::ITexture*
PostEffect::currentSource() const
{
    return _textureIn;
}

const Ibl::Region2i& 
PostEffect::postEffectBounds() const
{
    return _postEffectBounds;
}

bool 
PostEffect::render (const Ibl::ITexture* textureIn, const Ibl::Camera* camera)
{
    _postEffectBounds = Ibl::Region2i(Ibl::Vector2i(0,0), Ibl::Vector2i(_device->backbuffer()->width(), _device->backbuffer()->height()));

    Ibl::FrameBuffer frameBuffer(_postEffectTexture->surface(), nullptr);
    _device->setupViewport(frameBuffer);
    _device->bindFrameBuffer(frameBuffer);

    _textureIn = textureIn;
    _shader->renderMesh (Ibl::RenderRequest(_technique, nullptr, camera, this));

    // Needless stall. Needs optimize
    return _device->backbuffer()->bind (0);
}

bool 
PostEffect::render (const Ibl::ITexture* textureIn, 
                    const Ibl::ITexture* textureOut,
                    const Ibl::Camera* camera)
{
    _postEffectBounds = Ibl::Region2i(Ibl::Vector2i(0,0), Ibl::Vector2i(textureOut->width(), textureOut->height()));

    // Todo, setup viewport for framebuffer...
    Ibl::FrameBuffer frameBuffer(textureOut->surface(), nullptr);
    _device->setupViewport(frameBuffer);
    _device->bindFrameBuffer(frameBuffer);
    _inputs.push_back(_textureIn);

    _textureIn = textureIn;

    _shader->renderMesh (Ibl::RenderRequest(_technique, nullptr, camera, this));

    _inputs.clear();

    // Needless stall. Needs optimize
    return _device->backbuffer()->bind (0);
}

bool
PostEffect::render (const Ibl::ITexture* texture, 
                    const Ibl::ISurface* surface,
                    const Ibl::Camera* camera)
{
    _postEffectBounds = Ibl::Region2i(Ibl::Vector2i(0,0), Ibl::Vector2i(surface->width(), surface->height()));

    Ibl::FrameBuffer frameBuffer(surface, nullptr);
    _device->setupViewport(frameBuffer);
    _device->bindFrameBuffer(frameBuffer);

    _textureIn = texture;
    _inputs.push_back(_textureIn);
    _shader->renderMesh (Ibl::RenderRequest(_technique, nullptr, camera, this));

    _inputs.clear();

    // Needless stall. Needs optimize
    return _device->backbuffer()->bind (0);
}

bool
PostEffect::composite (const Ibl::GpuTechnique* technique,
                       const Ibl::ISurface* surface,
                       std::vector<const Ibl::IGpuBuffer*> & uavs,
                       std::vector<const Ibl::ITexture*>  & inputs,
                       const Ibl::Camera* camera)
{
    _postEffectBounds = Ibl::Region2i(Ibl::Vector2i(0,0), Ibl::Vector2i(surface->width(), surface->height()));
    Ibl::FrameBuffer frameBuffer(surface, nullptr);

    for (size_t i = 0; i < uavs.size(); i++)
    {
        frameBuffer.setUnorderedSurface(i, uavs[i]);
    }

    _device->setupViewport(frameBuffer);
    _device->bindFrameBuffer(frameBuffer);
    _inputs = inputs;

    _shader->renderMesh (Ibl::RenderRequest(technique, nullptr, camera, this));

    inputs.clear();
    uavs.clear();

    // Needless stall. Needs optimize

    Ibl::FrameBuffer frameBuffer0(_device->backbuffer(), nullptr);
    _device->bindFrameBuffer(frameBuffer0);
    return true;
}


bool
PostEffect::composite (const Ibl::GpuTechnique* technique,
                       const Ibl::ISurface* surface,
                       std::vector<const Ibl::ITexture*>      & inputs,
                       const Ibl::Camera* camera)
{
    _postEffectBounds = Ibl::Region2i(Ibl::Vector2i(0,0), Ibl::Vector2i(surface->width(), surface->height()));
    _inputs = inputs;
    Ibl::FrameBuffer frameBuffer(surface, nullptr);
    _device->setupViewport(frameBuffer);
    _device->bindFrameBuffer(frameBuffer);


    _shader->renderMesh (Ibl::RenderRequest(technique, nullptr, camera, this));


    inputs.clear();
    // Needless stall. Needs optimize
    return _device->backbuffer()->bind (0);;
}

bool
PostEffect::composite (const Ibl::ISurface* surface,
                       std::vector<const Ibl::ITexture*> &  inputs,
                       const Ibl::Camera*   camera)
{
    _postEffectBounds = Ibl::Region2i(Ibl::Vector2i(0,0), Ibl::Vector2i(surface->width(), surface->height()));
    _inputs = inputs;
    Ibl::FrameBuffer frameBuffer(surface, nullptr);
    _device->setupViewport(frameBuffer);
    _device->bindFrameBuffer(frameBuffer);

    _shader->renderMesh (Ibl::RenderRequest(_technique, nullptr, camera, this));

    inputs.clear();

    // Needless stall. Needs optimize
    return _device->backbuffer()->bind (0);
}

const Ibl::ITexture*  
PostEffect::input (uint32_t index) const 
{
    if (index >= _inputs.size())
    {
        // LOG ("Requesting invalid input index " << index << " in post effect");
        return nullptr; 
    }
    return _inputs[index];
}

bool 
PostEffect::initialize (const std::string& filename)
{
    _filename   = filename;
    _shader        = 0;
    if (Ibl::ShaderMgr* shaderMgr = _device->shaderMgr())
    {
        if (!shaderMgr->addShader (_filename, _shader, true))
        {
            LOG ("could not create shader from file " << filename);            
            return false;
        }
    }

    create();
    cache();

    return true;
}

}
