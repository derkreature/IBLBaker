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
#include <IblPostEffectsMgr.h>
#include <IblPostEffect.h>
#include <IblITexture.h>
#include <IblCamera.h>
#include <IblLog.h>
#include <IblViewportProperty.h>
#include <IblISurface.h>
#include <IblHDRPresentationPolicy.h>
#include <strstream>

namespace Ibl
{

PostEffectsMgr::PostEffectsMgr (const Ibl::Application* application,
                                Ibl::IDevice* device) : 
    _deviceInterface(device)
{
    _usingPostEffects = false;
    _presentationPolicies.insert (std::make_pair (HDR, new HDRPresentationPolicy(_deviceInterface)));
    setActivePolicy (HDR);
}

const FrameBuffer&
PostEffectsMgr::sceneFrameBuffer() const
{
    return _activePolicy->sceneFrameBuffer();
}

PresentationPolicy*
PostEffectsMgr::activePolicy ()
{
    return _activePolicy;
}

bool
PostEffectsMgr::setActivePolicy  (PostEffectPresentationPolicy policyType)
{
    auto it = _presentationPolicies.find (policyType);
    if (it != _presentationPolicies.end())
    {
        _activePolicy = it->second;
        return true;
    }
    return false;
}

PostEffectsMgr::~PostEffectsMgr()
{
    clearPathway();
}

bool 
PostEffectsMgr::free()
{
    return true;
}

bool 
PostEffectsMgr::create()
{    
    return true;
}

bool 
PostEffectsMgr::findEffect (const PostEffect*& target, const std::string& name) const
{
    bool result = false;
    for (uint32_t i = 0; i < _postEffects.size(); i++)
    {
        if (_postEffects[i]->name() == name)
        {
            target = _postEffects[i];
            return true;
        }
    }
    return result;
}

bool 
PostEffectsMgr::findEffect (PostEffect*& target, const std::string& name)
{
    bool result = false;
    for (uint32_t i = 0; i < _postEffects.size(); i++)
    {
        if (_postEffects[i]->name() == name)
        {
            target = _postEffects[i];
            return true;
        }
    }
    return result;
}

uint32_t 
PostEffectsMgr::postEffectCount() const
{
    return static_cast<uint32_t>(_postEffects.size());
}

const PostEffect* 
PostEffectsMgr::postEffect (const uint32_t& i) const
{
    return _postEffects[i];
}

bool 
PostEffectsMgr::removePostEffect(const uint32_t& index )
{
    uint32_t currentIndex = 0;
    for (auto it = _postEffects.begin(); it != _postEffects.end(); it++)
    {
        if (index == currentIndex)
        {
            safedelete((*it));
            _postEffects.erase(it);
            return true;
        }
        currentIndex++;
    }

    return false;
}

//-----------------------------------
// Manufactures a complex post graph
//-----------------------------------
PostEffect* 
PostEffectsMgr::manufacture (const std::string& name)
{
    return 0;
}

void
PostEffectsMgr::addPostEffect(PostEffect* postEffect)
{
    _postEffects.push_back(postEffect);
}

bool 
PostEffectsMgr::clearPathway()
{
    _usingPostEffects = false;
    for(uint32_t i = 0; i < _postEffects.size(); i++)
    {
        safedelete (_postEffects[i]);
    }

    _postEffects.erase(_postEffects.begin(), _postEffects.end());

    for (auto it = _presentationPolicies.begin(); it != _presentationPolicies.end(); it++)
    {
        safedelete(it->second);
    }
    return true;
}

bool 
PostEffectsMgr::update (float delta)
{
    for(uint32_t i = 0; i < _postEffects.size(); i++)
    {
        _postEffects[i]->update (delta);                
    }

    return true;
}

void 
PostEffectsMgr::removePostEffect (const PostEffect* rt)
{
    for (auto rtit = _postEffects.begin(); rtit != _postEffects.end(); rtit++)
    {
        if(*rtit == rt)
        {
            PostEffect* target = *rtit;
            safedelete (target);
            _postEffects.erase(rtit);
            break;
        }
    }

    if (_postEffects.size() == 0)
        _usingPostEffects = false;
}

const ITexture*
PostEffectsMgr::presentationBackBuffer()
{
    return _activePolicy->renderTarget();
}

void 
PostEffectsMgr::render(const Camera* camera) const
{
    Ibl::DrawMode drawMode = _deviceInterface->getDrawMode ();
    _deviceInterface->setDrawMode (Ibl::Filled);
    uint32_t i = 0;
    _activePolicy->prepareForBackBuffer(camera);
    
    //if (_usingPostEffects)
    {
        _deviceInterface->disableDepthWrite();
        //
        // try doing a surface copy straight to the render chain at this stage
        const ITexture* swapChain = _activePolicy->displayTarget();

        //render out the effects
        for (i = 0; i < _postEffects.size(); i++)
        {
            if (_postEffects[i]->render(swapChain, camera))
            {
                swapChain = _postEffects[i]->texture();
            }
        }
        _deviceInterface->enableDepthWrite();
    }

    const ISurface* syncSurface = (i > 0) ? _postEffects[i-1]->texture()->surface() : nullptr;
    _activePolicy->syncToBackBuffer (camera, syncSurface);
    _deviceInterface->backbuffer()->bind(false);
    _deviceInterface->setDrawMode (drawMode);
}

}