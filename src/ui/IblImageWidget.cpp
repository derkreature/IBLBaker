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

#include <IblImageWidget.h>
#include <IblScreenOrientedQuad.h> 
#include <IblTextureMgr.h>
#include <IblITexture.h>
#include <IblShaderMgr.h>
#include <IblLog.h>
#include <IblIShader.h>
#include <IblMaterial.h>
#include <IblGpuTechnique.h>

namespace Ibl
{
ImageWidget::ImageWidget(Ibl::IDevice* device, 
                         const std::string& texturePath,
                         const Region2f& bounds)
{
    _image = nullptr;
    _shader = 0;
    _material = 0; 
    _image = 0;
    _quad = 0;
    _device = device;
    _texturePath = texturePath;
    _bounds = bounds;
    _blendingIn = false;
    _blendInTime = 0;
    _totalBlendInTime = 0;

    init();
}

ImageWidget::ImageWidget(Ibl::IDevice* device, 
                         const Ibl::ITexture* image,
                         const Region2f& bounds)
{
    _shader = 0;
    _material = 0;
    _device = device;
    _image = image;
    _quad = 0;
    _bounds = bounds;
    _blendingIn = false;
    _blendInTime = 0;
    _totalBlendInTime = 0;

    init();
}

ImageWidget::~ImageWidget()
{
    safedelete(_quad);
    safedelete(_material);
    safedelete(_quad);
}

bool 
ImageWidget::cache()
{
    if (_quad && _shader)
    {
        if (_shader->getTechniqueByName ("postprocess", _technique))
        {
            _material->setShader (_shader);
            _material->setTechnique (_technique);
        } 
    }
    return true;
}

HRESULT 
ImageWidget::init() 
{    
    // Load the texture that this image widget displays
    if (_texturePath.size() > 0 && _image == nullptr)
    {
        _image = dynamic_cast <const Ibl::ITexture*>
            (_device->textureMgr()->loadTexture (_texturePath));
    }


    _material = new Material(_device);
    _material->setAlbedoMap(_image);
    _material->albedoColorProperty()->set(Ibl::Vector4f(1, 1, 1, 1));

    {
        // Load Shader
        if (Ibl::ShaderMgr* shaderMgr = _device->shaderMgr())
        {
            if (!shaderMgr->addShader ("IblImageBlit.fx", _shader, true))
            {
                 return E_FAIL;
            }
        }
    }                        

    // create the quad used to render this image to the screen
    if (_quad = new Ibl::ScreenOrientedQuad(_device))
    {
        if (_quad->initialize (_bounds))
        {
            _quad->setMaterial (_material);
        }
    }

    return cache();
}

void
ImageWidget::setBlendIn(float blendInTime)
{
    _blendingIn = true;
    _blendInTime = blendInTime;
    _totalBlendInTime = 0;
}

void
ImageWidget::setImage(const Ibl::ITexture* texture)
{
    _material->setAlbedoMap(texture);
}

void
ImageWidget::render (float elapsed)
{
    if (_blendingIn)
    {
        float alpha  = (_totalBlendInTime / _blendInTime);
        if (alpha  > 1)
        {
            _blendingIn = false;
            alpha = 1.0;
        }
        _material->albedoColorProperty()->set(Ibl::Vector4f(1, 1, 1, alpha));
    }

    if (_visible && _image && _quad)
    {
        _device->enableAlphaBlending();
        _device->setAlphaSrcFunction(Ibl::SourceAlpha);
        _device->setAlphaDestFunction(Ibl::InverseSourceAlpha);
        _device->setSrcFunction(Ibl::SourceAlpha);
        _device->setDestFunction(Ibl::InverseSourceAlpha);

        _shader->renderMesh (Ibl::RenderRequest(_technique, nullptr, nullptr, _quad));
    }
    if (_blendingIn)
    {
        _totalBlendInTime += elapsed;
    }
    
}

void
ImageWidget::setVisible(bool visible)
{
    _visible = visible;
}

bool
ImageWidget::visible() const
{
    return _visible;
}

}

