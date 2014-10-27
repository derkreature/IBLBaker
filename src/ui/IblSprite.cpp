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

#include <IblSprite.h>
#include <IblIShader.h>
#include <IblDynamicRenderer.h>
#include <IblShaderMgr.h>
#include <IblIShader.h>
#include <IblIEffect.h>
#include <IblGpuTechnique.h>
#include <IblMaterial.h>
#include <IblDynamicRenderer.h>
#include <IblMatrix44.h>
#include <IblMaterial.h>
#include <IblLog.h>
#include <IblITexture.h>

namespace Ibl
{
inline int RectWidth( RECT &rc ) { return ( (rc).right - (rc).left ); }
inline int RectHeight( RECT &rc ) { return ( (rc).bottom - (rc).top ); }

    
Sprite::Sprite (Ibl::IDevice* device) : 
    IRenderResource (device),
    _dynamicRenderer (nullptr),
    _material (nullptr),
    _shader (nullptr),
    _technique (nullptr)
{
}

Sprite::~Sprite()
{
    safedelete (_dynamicRenderer);
    safedelete (_material);
}

bool
Sprite::initialize ()
{
    // Load the sprite shader
    if (!_deviceInterface->shaderMgr()->addShader ("IblSpriteShader.fx", _shader))
    {
        LOG ("Could not create sprite shader");
    }

    // Get the sprite technique
    if (!_shader->getTechniqueByName ("sprite", _technique))
    {
        LOG ("Could not find sprite technique 'sprite'");
    }

    // Create the dynamic renderer
    _dynamicRenderer = new Ibl::DynamicRenderer(_deviceInterface);

    // Create the core material
    _material = new Ibl::Material(_deviceInterface);
    _material->setShader (_shader);
    _material->setTechnique (_technique);

    return create();
}

bool
Sprite::create()
{
    return true;
}

bool
Sprite::cache()
{
    return true;
}

bool
Sprite::free()
{
    return true;
}

bool
Sprite::render (const Ibl::Camera* camera,
                const Ibl::ITexture* texture,
                const RECT& rcScreenIn,
                const RECT& rcTexture, 
                const float x,
                const float y,
                const float z,
                const Ibl::Vector4f& color) const
{
    // This needs fixing so the material is passed in.
    _material->setAlbedoMap (texture);
    _material->albedoColorProperty()->set(color);

    _dynamicRenderer->setRenderState(_material, camera, nullptr, nullptr);

    RECT rcScreen = rcScreenIn;
    OffsetRect( &rcScreen, (int)x, (int)y);
    OffsetRect( &rcScreen, RectWidth( rcScreen )/2, RectHeight( rcScreen)/2 );
    // Need to get the back buffer width...
    float fBBWidth = (float)_deviceInterface->backbuffer()->width();
    float fBBHeight = (float)_deviceInterface->backbuffer()->height();

    float fScaleX = (float) RectWidth( rcScreen );
    float fScaleY = (float) RectHeight( rcScreen );

    Ibl::Vector3f screenPosition ( (float)rcScreen.left, (float)rcScreen.top, 0.5f);
   _deviceInterface->enableAlphaBlending();

    float u, v, u2, v2;

    float fTexWidth = (float)texture->width();
    float fTexHeight = (float)texture->height();

    u = (float)(rcTexture.left) / fTexWidth;
    v2 = (float)(rcTexture.top) / fTexHeight;
    u2 = u + (float)(rcTexture.right - rcTexture.left) / fTexWidth;
    v = v2 + (float)(rcTexture.bottom - rcTexture.top) / fTexHeight;

    _dynamicRenderer->begin(_material, camera);

    float left = ((rcScreenIn.left / fBBWidth) - 0.5f) * 2.0f;
    float right = ((rcScreenIn.right / fBBWidth) - 0.5f) * 2.0f;
    float top = (1.0f - (rcScreenIn.top / fBBHeight) - 0.5f) * 2.0f;
    float bottom = (1.0f - (rcScreenIn.bottom/ fBBHeight) - 0.5f) * 2.0f;

    // A
    _dynamicRenderer->vertex3f(Ibl::Vector3f(left, bottom, z), Ibl::Vector2f(u, v));

    // B
    _dynamicRenderer->vertex3f(Ibl::Vector3f(right, bottom, z), Ibl::Vector2f(u2, v));

    // C
    _dynamicRenderer->vertex3f(Ibl::Vector3f(right, top, z), Ibl::Vector2f(u2, v2));


    // A
    _dynamicRenderer->vertex3f(Ibl::Vector3f(left, bottom, z), Ibl::Vector2f(u, v));

    // C
    _dynamicRenderer->vertex3f(Ibl::Vector3f(right, top, z), Ibl::Vector2f(u2, v2));

    // D
    _dynamicRenderer->vertex3f(Ibl::Vector3f(left, top, z), Ibl::Vector2f(u, v2));

    _dynamicRenderer->end();


    _dynamicRenderer->begin(_material);

    _deviceInterface->setDrawMode (Ibl::Wireframe);

    // A
    _dynamicRenderer->vertex3f(Ibl::Vector3f(left, bottom, z), Ibl::Vector2f(u, v));

    // B
    _dynamicRenderer->vertex3f(Ibl::Vector3f(right, bottom, z), Ibl::Vector2f(u2, v));

    // C
    _dynamicRenderer->vertex3f(Ibl::Vector3f(right, top, z), Ibl::Vector2f(u2, v2));

    // A
    _dynamicRenderer->vertex3f(Ibl::Vector3f(left, bottom, z), Ibl::Vector2f(u, v));

    // C
    _dynamicRenderer->vertex3f(Ibl::Vector3f(right, top, z), Ibl::Vector2f(u2, v2));

    // D
    _dynamicRenderer->vertex3f(Ibl::Vector3f(left, top, z), Ibl::Vector2f(u, v2));
    
    _deviceInterface->setDrawMode (Ibl::Filled);

    _dynamicRenderer->end();

    return true;
}

}