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

#include <CtrPlatform.h>
#include <CtrTimer.h>
#include <CtrLog.h>
#include <CtrNode.h>
#include <CtrTypedProperty.h>
#include <CtrMaterial.h>
#include <CtrApplication.h>

namespace Ctr
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

class IBLApplication : public Ctr::Application
{ 
  public:
    IBLApplication(ApplicationHandle instance);
    virtual ~IBLApplication();

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

    void                       loadAsset(Entity*& targetEntity,
                                         const std::string& assetPathName,
                                         const std::string& materialPathName,
                                         bool  userAsset);

    IntProperty*               modelVisualizationProperty();
    FloatProperty*             constantRoughnessProperty();
    FloatProperty*             constantMetalnessProperty();
    IntProperty*               specularWorkflowProperty();
    IntProperty*               debugTermProperty();
    FloatProperty*             specularIntensityProperty();
    FloatProperty*             roughnessScaleProperty();


    enum VisualizationType
    {
        LDR = 1,
        MDR = 2,
        HDR = 4
    };

    enum ModelVisualizationType
    {
        UserModel = 0,
        ShaderBallModel = 1
    };

    enum SourceInputMode
    {
        EquirectangularInput,
        CubemapInput,
        CubeFaceListInput
    };
        

    void                       pause();
    void                       cancel();
    void                       compute();


    Entity*                    visualizedEntity();
    Entity*                    shaderBallEntity();
    Entity*                    sphereEntity();
    Entity*                    iblSphereEntity();

    IntProperty*               visualizationSpaceProperty();

    void                       setupModelVisibility(Ctr::Entity* entity, bool visibility);
    void                       syncVisualization();

    //_inputMode;

  protected:
    void                       updateApplication();
    bool                       purgeMessages() const;
    void                       updateVisualizationType();

  private:
    // Properties:
    IntProperty*               _visualizationSpaceProperty;
    IntProperty*               _currentVisualizationSpaceProperty;

    PixelFormatProperty *      _hdrFormatProperty;
    IntProperty*               _probeResolutionProperty;

    IntProperty*               _modelVisualizationProperty;

    // Management layer.
    InputManager*              _inputMgr;
    FocusedDampenedCamera*     _cameraManager;

    RenderHUD*                 _renderHUD;

    // Render passes
    ColorPass*                 _colorPass;
    IBLRenderPass*             _iblRenderPass;
    IBLProbe*                  _probe;

    Ctr::Scene*                 _scene;

    // Application entities for convenience.
    Entity*                    _visualizedEntity;
    Entity*                    _shaderBallEntity;
    Entity*                    _sphereEntity;
    Entity*                    _iblSphereEntity;

    bool                       _headless;
    uint32_t                   _windowWidth;
    uint32_t                   _windowHeight;
    bool                       _windowed;

    FloatProperty*             _constantRoughnessProperty;
    FloatProperty*             _constantMetalnessProperty;

  protected:
    Ctr::Timer                  _timer;
    bool                        _runTitles;
    std::string                 _defaultAsset;

    IntProperty*                _specularWorkflowProperty;
    FloatProperty*              _specularIntensityProperty;
    FloatProperty*              _roughnessScaleProperty;
    IntProperty*                _debugTermProperty;

    SourceInputMode             _inputMode;
};
}

#endif