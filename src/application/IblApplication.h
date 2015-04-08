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

#ifndef INCLUDED_APPLICATION
#define INCLUDED_APPLICATION

#include <IblPlatform.h>
#include <IblTimer.h>
#include <IblLog.h>
#include <IblNode.h>
#include <IblTypedProperty.h>
#include <IblMaterial.h>

namespace Ibl
{
class Window;
class DeviceD3D11;
class Camera;
class ShaderMgr;
class VertexDeclarationMgr;
class InputManager;
class PostEffectsMgr;
class TextureMgr;
class ShaderParameterValueFactory;
class Scene;
class ColorPass;
class IBLRenderPass;
class FocusedDampenedCamera;
class RenderHUD;
class IBLProbe;
class Entity;
class Titles;

class Application : public Node
{ 
  public:
    Application(ApplicationHandle instance);
    virtual ~Application();

    bool                       parseOptions(int argc, char* argv[]);
    void                       initialize();
    void                       run();

    ApplicationHandle          instance() const;

    const Timer&               timer() const;
    Timer&                     timer();

    Window*                    window();
    const Window*              window() const;

    bool                       loadParameters();
    bool                       saveParameters() const;

    bool                       loadEnvironment(const std::string& filePathName);
    bool                       saveImages(const std::string& filePathName, bool gameOnly = false);

    void                       loadAsset(const std::string& assetFilePathName,
                                         const std::string& materialFilePathName = std::string(""));

    enum VisualizationType
    {
        LDR = 1,
        MDR = 2,
        HDR = 4
    };

    void                       pause();
    void                       cancel();
    void                       compute();


    Entity*                    visualizedEntity();
    Entity*                    sphereEntity();
    Entity*                    iblSphereEntity();

    IntProperty*               visualizationSpaceProperty();

  protected:
    void                       updateApplication();
    bool                       purgeMessages() const;
    void                       updateVisualizationType();

  private:
    // Properties:
    IntProperty*               _visualizationSpaceProperty;
    IntProperty*               _currentVisualizationSpaceProperty;

    PixelFormatProperty *    _hdrFormatProperty;
    IntProperty*               _probeResolutionProperty;

    ApplicationHandle          _instance;
    Ibl::Window*                _mainWindow;
    Ibl::DeviceD3D11*           _device;

    // Management layer.
    InputManager*              _inputMgr;
    FocusedDampenedCamera*     _cameraManager;

    RenderHUD*                 _renderHUD;

    // Render passes
    ColorPass*                 _colorPass;
    IBLRenderPass*             _iblRenderPass;
    IBLProbe*                  _probe;

    Ibl::Scene*                 _scene;

    // Application entities for convenience.
    Entity*                    _visualizedEntity;
    Entity*                    _sphereEntity;
    Entity*                    _iblSphereEntity;

    bool                       _headless;
    uint32_t                   _windowWidth;
    uint32_t                   _windowHeight;
    bool                       _windowed;

  protected:
    Ibl::Timer                  _timer;

    bool                        _runTitles;
    SpecularWorkflow            _workflow;
    std::string                 _defaultAsset;
};
}

#endif