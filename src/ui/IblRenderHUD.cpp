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
#include <IblWidgetDialog.h>
#include <IblRenderWindow.h>
#include <IblControl.h>
#include <IblEditBox.h>
#include <IblListBox.h>
#include <IblImageWidget.h>
#include <IblLog.h>
#include <IblApplication.h>
#include <IblRegion.h>
#include <IblFontMgr.h>
#include <IblFont.h>
#include <IblButton.h>
#include <IblISurface.h>
#include <IblPostEffectsMgr.h>
#include <AntTweakBar.h>
#include <IblRenderDeviceD3D11.h>
#include <IblDialogResourceManager.h>

namespace Ibl
{
namespace
{
RenderHUD* _renderHUD = 0;
#define Logo          1024
void CALLBACK OnGUIEvent (UINT nEvent, 
                          int nControlID, 
                          Ibl::Control*);
}

RenderHUD::RenderHUD (Ibl::Application* application,
                      Ibl::IDevice* device,
                      Ibl::InputState* inputState,
                      const std::string& logoPath) :
_dialog (0),
_fpsFont (0),
_drawFps (false),
_inputState (inputState),
_logoPath (logoPath),
_application (application),
_deviceInterface(device)
{
    _scriptControlsVisible = false;
    _renderHUD = this;
    _uiVisible = false;
    _renderWindow = dynamic_cast<Ibl::RenderWindow*>(application->window());

    if (Ibl::FontMgr * fontMgr = _deviceInterface->fontMgr())
    {
        if (_fpsFont = dynamic_cast <Ibl::Font*>
            (fontMgr->createFont ("Arial", 14)))
        {
            LOG ("Hud font created");
        }
        else
        {
            LOG ("Hud font failed...\n");
        }
    }

    const Ibl::ISurface* backbuffer = _deviceInterface->backbuffer();

    if (_dialog = new Ibl::Dialog (_renderWindow,_deviceInterface ))
    {        
        _dialog->SetCallback (OnGUIEvent);
        _dialog->setLocation (0, 0);
        _dialog->SetFont(0, "Arial", 14, FW_SEMIBOLD );

        
        float aspect = 1.0f;
        Ibl::Vector2f min; 
        Ibl::Vector2f max; 
        if (backbuffer->width() > backbuffer->height())
        {
           float xaspect  = ((float)backbuffer->width() / (float)backbuffer->height());
           float yaspect  = ((float)backbuffer->height() / (float)backbuffer->width());
           float yoffset = (0.45f*xaspect);
           float whiteSpaceYOffset = yoffset * 0.25f;
           min = Vector2f ((0.985f-(0.45f)), -1.05f -  whiteSpaceYOffset);
           max = Vector2f (0.985f, (-1.05f+yoffset)- whiteSpaceYOffset);
        }
        else
        {
          aspect  = ((float)backbuffer->height() / (float)backbuffer->width());            
          min = Vector2f (0.37f, -1.1f);
          max = Vector2f (1.00f, -1.1f + (0.21f * aspect));
        }

        _dialog->addImageWidget (Logo, _logoPath, Region2f (min, max));
        _logo = dynamic_cast<Ibl::ImageWidget *>
            (_dialog->getControl(Logo));
    }
    

    if (Ibl::DeviceD3D11* device11 = dynamic_cast<Ibl::DeviceD3D11*>(_deviceInterface))
    {
        if (!TwInit(TW_DIRECT3D11, *device11))
        {
            LOG ("Failed to init device\n");
        }
    }
    // Else GL, etc...


    _logoVisible = true;
}

RenderHUD::~RenderHUD()
{
    _renderHUD = 0;

    for (size_t i = 0; i <_tweakBars.size(); i++)
    {
        TwDeleteBar(_tweakBars[i]);
    }
    TwTerminate();

    safedelete (_dialog);
    DestoryDialogResourceManager();
}


TwBar*
RenderHUD::addTweakBar(const std::string& name, const Ibl::Region2i& bounds)
{
    TwBar *bar = TwNewBar(name.c_str());
    TwDefine(" GLOBAL help='This is an example of the default kreature options.' "); // Message added to the help bar.
    int barSize[2] = {bounds.size().x, bounds.size().y};
    int barLoc[2] = {bounds.minExtent.x, bounds.minExtent.y};
    TwSetParam(bar, nullptr, "position", TW_PARAM_INT32, 2, barLoc);
    TwSetParam(bar, nullptr, "size", TW_PARAM_INT32, 2, barSize);

    _tweakBars.push_back(bar);
    return bar;
}

Ibl::ImageWidget*
RenderHUD::logo()
{
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
    _inputState->setHasGUIFocus (_dialog->dialogHasFocus() || !_renderWindow->rendererHasFocus());

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

    if (_dialog)
    {
        _dialog->resetZ();
        _dialog->render ((float)_application->timer().elapsedTime(), camera);
    }

    if (_fpsFont && _drawFps)
    {
        std::ostringstream stream;
        static const std::string msg = "Framerate: ";
        static const std::string unit = " fps";

        stream << msg << _application->timer().frameRate() << unit;
        Ibl::Vector2f location (0, 0);

        _fpsFont->render (camera, stream.str().c_str(),
                          location,
                          Ibl::Vector4f (1.0f, 1.0f, 1.0f, 1.0f));
    
        Ibl::Vector2f cameraLocation (0, 24);
        std::ostringstream  cameraStream;
        cameraStream << "Camera t = " << camera->translation().x << " " << camera->translation().y << " " << camera->translation().z  << " : rotation = " << camera->rotation().x << " " << camera->rotation().y << " " << camera->rotation().z;
        _fpsFont->render (camera, cameraStream.str().c_str(),
                          cameraLocation,
                          Ibl::Vector4f (1.0f, 1.0f, 1.0f, 1.0f));
    }


    if (_uiVisible)
    {
        TwDraw();
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

void
RenderHUD::handleEvent (UINT nEvent, int nControlID, Ibl::Control* pControl)
{
}

namespace
{
void CALLBACK 
OnGUIEvent (UINT eventId, int controlId, Ibl::Control* control)
{
    _renderHUD->handleEvent (eventId, controlId, control);
}
}
}