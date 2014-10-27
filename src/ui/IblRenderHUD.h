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


#ifndef INCLUDED_BB_RENDERHUD
#define INCLUDED_BB_RENDERHUD

#include <IblPlatform.h>
#include <IblControl.h>
#include <IblIRenderResource.h>
//#include <IblFrameCounter.h>
#include <IblInputState.h>
#include <AntTweakBar.h>

namespace Ibl
{

class Widget;
class Dialog;
class EditBox;
class ListBox;
class ImageWidget;
class Button;

class Application;
class RenderWindow;

class ITexture;
class Font;
class IDevice;
class InputState;
class Camera;

class RenderHUD 
{
  public:
    RenderHUD(Ibl::Application* application,
              Ibl::IDevice* device,
              Ibl::InputState* inputState,
              const std::string& logoPath = std::string("data/Textures/BakerLogo.dds"));
    
    virtual ~RenderHUD();

    virtual bool               create();
    virtual bool               update(double elapsedTime);
    virtual void               render(const Ibl::Camera* camera);
    virtual void               showApplicationUI()=0;
    virtual void               handleEvent(UINT eventId, int controlId, Ibl::Control* control);
    void                       toggleScriptControlVisibility();

    bool                       logoVisible() const;
    void                       setLogoVisible(bool state);

    void                       setUIVisible(bool);
    bool                       uiVisible() const;

    TwBar*                     addTweakBar(const std::string& name, const Ibl::Region2i& bounds);


    Ibl::ImageWidget*           logo();

  protected:
    Ibl::Dialog*                dialog();
    Ibl::Dialog*                _dialog;

    Ibl::Font*                  _fpsFont;
    Ibl::RenderWindow*          _renderWindow;
    bool                       _scriptControlsVisible;
    bool                       _logoVisible;
    bool                       _uiVisible;

    Ibl::Application*           _application;
    Ibl::IDevice*               _deviceInterface;
    Ibl::ImageWidget *          _logo;
    Ibl::InputState*            _inputState;

    std::vector<TwBar*>        _tweakBars;

    bool                       _drawFps;
    std::string                _logoPath;
};
}
#endif