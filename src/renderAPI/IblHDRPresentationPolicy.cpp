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
#include <IblHDRPresentationPolicy.h>
#include <IblTextureMgr.h>
#include <IblShaderMgr.h>
#include <IblIShader.h>
#include <IblCamera.h>
#include <IblGpuVariable.h>
#include <IblGpuTechnique.h>
#include <IblRenderRequest.h>

namespace Ibl
{
HDRPresentationPolicy::HDRPresentationPolicy (Ibl::IDevice* device) :
    PresentationPolicy (device),
    _useAntiAliasing (false),
    _renderTarget (0),
    _usePostFXAA(true),
    _currentFXAAPreset(5),
    _sourceTexture(0),
    _renderTargetA(0)
{
    Ibl::ShaderMgr* shaderMgr = _device->shaderMgr();

    uint32_t sampleCount = device->multiSampleCount();
    uint32_t sampleQuality = device->multiSampleQuality();

    // In the anti aliasing case this texture becomes the swap texture
    _renderTargetA = _device->createTexture(&TextureParameters ("HDR Texture 1",
                             Ibl::TwoD,
                             Ibl::RenderTarget,
                             Ibl::PF_FLOAT16_RGBA,
                             Ibl::Vector3i(MIRROR_BACK_BUFFER, MIRROR_BACK_BUFFER, 1)));                                                                      

    Ibl::TextureParameters sourceInit = 
            Ibl::TextureParameters ("",
                                   Ibl::TwoD,
                                   Ibl::RenderTarget,
                                   Ibl::PF_A8R8G8B8,
                                   Ibl::Vector3i(MIRROR_BACK_BUFFER, MIRROR_BACK_BUFFER, 1));
    _sourceTexture = _device->createTexture(&sourceInit);
    if (!_sourceTexture)
    {
        LOG ("Could not allocate the source texture for fxaa");
    }
    
    for (int i = 0; i < 6; i++)
    {
        if (shaderMgr)
        {
            const Ibl::IShader* shader = 0;
            std::map <std::string, std::string>       defines;

            std::ostringstream stream;
            stream << i;
        
            defines.insert (std::make_pair(std::string("FXAA_PRESET"), std::string(stream.str().c_str())));

            if (!shaderMgr->addShader ("IblFXAA.fx",  shader, true, false, defines))
            {
                LOG ("Failed to load FXAA shader\n");
            }
            else
            {
                _fxaaShader.push_back(shader);
            }
        }
    }
    
     // Load the tone mapping shader
    initialize ("IblHDRPresentation.fx");
}

HDRPresentationPolicy::~HDRPresentationPolicy()
{
    _device->destroyResource(_renderTargetA);
    _device->destroyResource(_sourceTexture);
}

bool
HDRPresentationPolicy::free()
{
    return PresentationPolicy::free();
}

bool
HDRPresentationPolicy::create()
{
    PresentationPolicy::create();
    return cache();
}

bool
HDRPresentationPolicy::cache()
{
    _renderTarget = _renderTargetA;
    _sceneFrameBuffer = FrameBuffer (_renderTarget->surface(), _device->depthbuffer());
    return PresentationPolicy::cache();
}

const ITexture*    
HDRPresentationPolicy::lastRenderTarget() const
{
    return _renderTarget;
}


bool
HDRPresentationPolicy::prepareForBackBuffer(const Ibl::Camera* camera)
{

    _renderTarget = _renderTargetA;
    _sceneFrameBuffer = FrameBuffer (_renderTarget->surface(), _device->depthbuffer());

    _device->bindDepthSurface(nullptr);
    _device->bindSurface(0, 0);
    _device->disableAlphaBlending();

    _device->disableDepthWrite();
    _device->setCullMode (Ibl::CullNone);
   
    if (render (_renderTarget, camera)) /* stage that does the tone mapping */
    {
        if (_usePostFXAA && _sourceTexture)
        {
            _sourceTexture->clearSurface(0,0,0,0,0);
            _device->blitSurfaces (_sourceTexture->surface(), 
                                            _postEffectTexture->surface());
            renderFXAA(_sourceTexture, _postEffectTexture, camera);
        }
        return true;
    }

    return false;
}

const ITexture*
HDRPresentationPolicy::renderTarget() const
{
    return _renderTarget;
}

const ITexture*
HDRPresentationPolicy::displayTarget() const
{
    return _postEffectTexture;
}

const ISurface*
HDRPresentationPolicy::renderTargetSurface() const
{
    return _renderTargetA->surface();
}

const ISurface*
HDRPresentationPolicy::displayTargetSurface() const
{
    return _postEffectTexture->surface();
}

const ISurface*
HDRPresentationPolicy::fullSceneBackBufferSurface() const
{
    return nullptr;
}

const ISurface*
HDRPresentationPolicy::opaqueBackBufferSurface() const
{
    return nullptr;
}

bool
HDRPresentationPolicy::usePostFXAA() const
{
    return _usePostFXAA;
}

void
HDRPresentationPolicy::setUsePostFXAA(bool value)
{
    _usePostFXAA = value;
}

size_t
HDRPresentationPolicy::currentFXAAPreset() const
{
    return _currentFXAAPreset;
}

void
HDRPresentationPolicy::setCurrentFXAAPreset(int value)
{
    _currentFXAAPreset = value;
}

void 
HDRPresentationPolicy::renderFXAA(ITexture *src,
                                  ITexture *dst,
                                  const Camera* camera) 
{

    const GpuVariable* rcpFrameVariable = 0;
    const GpuVariable* sourceTextureVariable = 0;    

    if (_currentFXAAPreset < _fxaaShader.size())
    {
        _fxaaShader[_currentFXAAPreset]->getParameterByName("sourceTexture", sourceTextureVariable);
        _fxaaShader[_currentFXAAPreset]->getParameterByName("rcpFrame", rcpFrameVariable);

        sourceTextureVariable->setTexture(src);

        Ibl::Vector4f rcp (1.0f / src->width(), 1.0f / src->height(), 0,0);
        rcpFrameVariable->set(&rcp.x, sizeof(float)*4);

        Ibl::FrameBuffer framebuffer;
        // And here we go!
        framebuffer.setColorSurface (0, dst->surface());
        framebuffer.setDepthSurface(0);
        _device->bindFrameBuffer(framebuffer);

        // Do it!
        const Ibl::GpuTechnique* fxaa = 0;
        _fxaaShader[_currentFXAAPreset]->getTechniqueByName("postprocess", fxaa);
        _fxaaShader[_currentFXAAPreset]->renderMesh (Ibl::RenderRequest(fxaa, nullptr, camera, this));

        _device->bindDepthSurface(nullptr);
        _device->bindSurface(0, 0);
    }
}

const FrameBuffer& 
HDRPresentationPolicy::sceneFrameBuffer() const
{
    _sceneFrameBuffer = FrameBuffer(_renderTarget->surface(), _device->depthbuffer());
    return _sceneFrameBuffer;
}

}
