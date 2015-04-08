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


#include <IblRenderHUD.h>
#include <IblRenderWindow.h>
#include <IblLog.h>
#include <IblApplication.h>
#include <IblRegion.h>
#include <IblISurface.h>
#include <IblPostEffectsMgr.h>
#include <IblRenderDeviceD3D11.h>
#include <IblImageWidget.h>

namespace Ibl
{
RenderHUD* _renderHUD = 0;
#define Logo          1024

RenderHUD::RenderHUD (Ibl::Application* application,
                      Ibl::IDevice* device,
                      Ibl::InputState* inputState,
                      const std::string& logoPath) :
_drawFps (false),
_inputState (inputState),
_logoPath (logoPath),
_application (application),
_deviceInterface(device),
_logo(nullptr)
{
    _scriptControlsVisible = false;
    _renderHUD = this;
    _uiVisible = false;
    _renderWindow = dynamic_cast<Ibl::RenderWindow*>(application->window());
    const Ibl::ISurface* backbuffer = _deviceInterface->backbuffer();
    
    float aspect = 1.0f;
    Ibl::Vector2f min;
    Ibl::Vector2f max;
    if (backbuffer->width() > backbuffer->height())
    {
        float xaspect = ((float)backbuffer->width() / (float)backbuffer->height());
        float yaspect = ((float)backbuffer->height() / (float)backbuffer->width());
        float yoffset = (0.45f*xaspect);
        float whiteSpaceYOffset = yoffset * 0.25f;
        min = Vector2f((0.985f - (0.45f)), -1.05f - whiteSpaceYOffset);
        max = Vector2f(0.985f, (-1.05f + yoffset) - whiteSpaceYOffset);
    }
    else
    {
        aspect = ((float)backbuffer->height() / (float)backbuffer->width());
        min = Vector2f(0.37f, -1.1f);
        max = Vector2f(1.00f, -1.1f + (0.21f * aspect));
    }

    _logo = new Ibl::ImageWidget(_deviceInterface, _logoPath, Region2f(min, max));
    _logoVisible = true;
}

RenderHUD::~RenderHUD()
{
    _renderHUD = 0;

    // TODO: Move to Imgui.
    if (_logo)
        safedelete(_logo);

}

Ibl::ImageWidget*
RenderHUD::logo()
{
    // You touched my logo!
    return _logo;
}

bool
RenderHUD::logoVisible() const
{
    return _logoVisible;
}

void
RenderHUD::setLogoVisible(bool state)
{
    _logo->setVisible(state);
}

void
RenderHUD::setUIVisible(bool value) 
{
    _uiVisible = value;
}

bool
RenderHUD::uiVisible(void) const
{
    return _uiVisible;
}

bool
RenderHUD::update(double elapsedTime)
{
    _elapsedTime = float(elapsedTime);
    if (_inputState->getKeyState(DIK_F7))
    {
        static int index = 0;
        std::string filePathName = "";
        index++;
    }
    if (_inputState->getKeyState(DIK_F4))
    {
        _drawFps = false;
    }
    if (_inputState->getKeyState(DIK_F3))
    {
        _drawFps = true;
    }
    
    return true;
}

bool
RenderHUD::create()
{
    return true;
}

void
RenderHUD::render(const Ibl::Camera* camera)
{
    Ibl::DrawMode drawMode = _deviceInterface->getDrawMode ();
    _deviceInterface->setDrawMode (Ibl::Filled);

    if (_logo->visible())
    {
        _logo->render(_elapsedTime);
    }

    _deviceInterface->setDrawMode (drawMode);
    _deviceInterface->setAlphaSrcFunction (Ibl::SourceAlpha);
    _deviceInterface->setAlphaDestFunction (Ibl::InverseSourceAlpha);

}

void
RenderHUD::toggleScriptControlVisibility()
{
    _scriptControlsVisible = !_scriptControlsVisible;
}

}