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

#ifndef INCLUDED_RENDER_TEST_HUD
#define INCLUDED_RENDER_TEST_HUD

#include <IblRenderHud.h>
#include <IblMaterial.h>

namespace Ibl
{
class Application;
class UberLightingModel;
class MeshEntity;
class VSMModel;
class Slider;
class Static;
class CheckBox;
class ComboBox;
class Scene;
class Camera;
class Entity;
class Mesh;
class PostEffect;
class Material;
class ImageWidget;

#define LoadEnvironment 16000
#define SaveEnvironment 16001
#define ComputeEnvironment 16002
#define CancelComputeEnvironment 16003
#define LoadAsset 16004
#define BrdfViewer 16005

class ApplicationHUD : public RenderHUD
{
  public:
    ApplicationHUD (Ibl::Application*,
                    Ibl::IDevice* device,
                    Ibl::InputState* inputState,
                    Ibl::Scene* scene);

    virtual ~ApplicationHUD();

    virtual bool               create();
    virtual bool               update(double elapsedTime);
    virtual void               render(const Ibl::Camera* camera);
    virtual void               handleEvent (UINT eventId, int controlId, Ibl::Control* control);

    void                       showApplicationUI();
    void                       setupMeshUI();
    void                       cleanupMeshUI();

    static  void TW_CALL       setCB(const void *value, void * /*clientData*/);
    static  void TW_CALL       getCB(void *value, void * /*clientData*/);

    void                       addToBundle(Property* key, Property* value);

  private:
    Scene*                     _scene;

    static ApplicationHUD*     _applicationHud;
    bool                       _controlsVisible;

    Button*                    _loadEnvironmentMapButton;
    Button*                    _saveEnvironmentMapButton;
    Button*                    _computeEnvironmentMapButton;
    Button*                    _cancelComputeEnvironmentButton;
    Button*                    _loadAsset;
    ImageWidget*               _brdfViewer;

    std::map<Ibl::Property*, ETwType>         _tweakProperties;
    std::map<Property*, std::set<Property*> > _tweakBundles;
};
}

#endif