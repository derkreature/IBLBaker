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

#include <IblApplicationHUD.h>
#include <IblApplication.h>
#include <IblWidgetDialog.h>
#include <IblSlider.h>
#include <IblStatic.h>
#include <IblCheckBox.h>
#include <IblComboBox.h>
#include <IblCamera.h>
#include <IblScene.h>
#include <IblEntity.h>
#include <IblMaterial.h>
#include <IblMesh.h>
#include <IblPostEffect.h>
#include <IblPostEffectsMgr.h>
#include <IblEntity.h>
#include <IblMaterial.h>
#include <IblIBLProbe.h>
#include <IblBrdf.h>
#include <IblImageWidget.h>
#include <Iblimgui.h>
#include <CommDlg.h>

namespace Ibl
{
#define MAX_FILE_PATH_NAME 1024

ApplicationHUD* ApplicationHUD::_applicationHud = 0;


template <typename T>
void
removeTweakProperty(TwBar* bar, T* propertyT, 
                    std::map<Ibl::Property*, ETwType>& trackedProperties, 
                    std::map<Property*, std::set<Property*> >& bundles)
{
    TwRemoveVar(bar, propertyT->name().c_str());
     auto bundleIt = bundles.find(propertyT);
     if (bundleIt != bundles.end())
     {
         bundles.erase(bundleIt);
     }
     auto propertyIt = trackedProperties.find(propertyT);
     if (propertyIt != trackedProperties.end())
     {
         trackedProperties.erase(propertyIt);
     }
}

template <typename T>
bool
addTweakProperty(TwBar* bar, 
                 T* propertyT, 
                 std::map<Ibl::Property*, ETwType>& trackedProperties)
{
    ETwType type = (ETwType)(0);
    std::ostringstream settings;
    Ibl::Property* property = propertyT;

    if (typeid(T) == typeid(Ibl::VectorProperty))
    {
        type = TW_TYPE_DIR3F;
    }
    else if (typeid(T) == typeid(Ibl::FloatProperty))
    {
        type = TW_TYPE_FLOAT;
    }
    else if (typeid(T) == typeid(Ibl::BoolProperty))
    {
        type = TW_TYPE_BOOLCPP;
    }
    else if (typeid(T) == typeid(Ibl::IntProperty) ||
             typeid(T) == typeid(Ibl::PixelFormatProperty))
    {
        type = TW_TYPE_INT32;
        if (property->tweakFlags())
        {
            if (property->tweakFlags()->enumType)
            {
                type = property->tweakFlags()->enumType->type();
            }
        }
    }
    else
    {
        LOG("Unknown T : " << typeid(T).name() << " property " << propertyT->name())
        return false;
    }

    if (const Ibl::TweakFlags* tweakFlags = property->tweakFlags())
    {
        if (type != TW_TYPE_BOOLCPP && type != TW_TYPE_DIR3F  && type <= TW_TYPE_DIR3D)
        {
            settings << " min=" << tweakFlags->minValue << " max=" << tweakFlags->maxValue << 
                        " step=" << tweakFlags->step;
        }
        settings << " group=" << tweakFlags->category;
    }

    LOG("Creating attribute " << propertyT->name() << " with settings: " << settings.str());

    TwAddVarCB(bar, propertyT->name().c_str(), type, &ApplicationHUD::setCB, &ApplicationHUD::getCB, property, settings.str().c_str());
    trackedProperties.insert(std::make_pair(property, type));

    return true;
}

void
getProperty(void*& twPtr, void* propertyPtr, 
            std::map<Ibl::Property*, ETwType>& trackedProperties)
{
    auto it = trackedProperties.find((Ibl::Property*)(propertyPtr));
    if (it != trackedProperties.end())
    {
        if (it->second == TW_TYPE_DIR3F)
        {
            if (Ibl::VectorProperty* property = dynamic_cast<Ibl::VectorProperty*>(it->first))
            {
                Ibl::Vector3f& value = property->get();
                for (uint32_t channel = 0; channel < 3; channel++)
                    ((float*)(twPtr))[channel] = value[channel];
            }
        }
        else if (it->second == TW_TYPE_FLOAT)
        {
            if (Ibl::FloatProperty* property = dynamic_cast<Ibl::FloatProperty*>(it->first))
            {
                float value = property->get();
                *(float*)(twPtr) = value;
            }
        }
        else if (it->second == TW_TYPE_BOOLCPP)
        {
            if (Ibl::BoolProperty* property = dynamic_cast<Ibl::BoolProperty*>(it->first))
            {
                *(bool*)(twPtr) = property->get();
            }
        }
        else if (it->second == TW_TYPE_INT32 ||
                 it->second > TW_TYPE_DIR3D)
        {
            if (Ibl::IntProperty* property = dynamic_cast<Ibl::IntProperty*>(it->first))
            {
                *(int32_t*)(twPtr) = property->get();
            }
            else if (Ibl::PixelFormatProperty* property = dynamic_cast<Ibl::PixelFormatProperty*>(it->first))
            {
                *(int32_t*)(twPtr) = property->get();
            }
        }
    }
}

inline void
setProperty(const void*& twPtr, TwType type, Property* p)
{
    if (type == TW_TYPE_DIR3F)
    {
        if (Ibl::VectorProperty* property = dynamic_cast<Ibl::VectorProperty*>(p))
        {
            Ibl::Vector3f value;
            for (uint32_t channel = 0; channel < 3; channel++)
                value[channel] = ((const float*)(twPtr))[channel];
            property->set(value);
        }
    }
    else if (type == TW_TYPE_FLOAT)
    {
        if (Ibl::FloatProperty* property = dynamic_cast<Ibl::FloatProperty*>(p))
        {
            property->set(*(const float*)(twPtr));
        }
    }
    else if (type == TW_TYPE_BOOLCPP)
    {
        if (Ibl::BoolProperty* property = dynamic_cast<Ibl::BoolProperty*>(p))
        {
            property->set(*(const bool*)(twPtr));
        }
    }
    else if (type == TW_TYPE_INT32 ||
             type > TW_TYPE_DIR3D)
    {
        if (Ibl::IntProperty* property = dynamic_cast<Ibl::IntProperty*>(p))
        {
            property->set(*(const int32_t*)(twPtr));
        }
        else if (Ibl::PixelFormatProperty* property = dynamic_cast<Ibl::PixelFormatProperty*>(p))
        {
            property->set((Ibl::PixelFormat)*(int32_t*)(twPtr));
        }
    }
}

inline void
setProperty(const void*& twPtr, void* propertyPtr, 
            std::map<Ibl::Property*, ETwType>& trackedProperties, 
            const std::map<Property*, std::set<Property*> >& bundles)
{
    {
        auto it = trackedProperties.find((Ibl::Property*)(propertyPtr));
        if (it != trackedProperties.end())
        {
            auto bundleIt = bundles.find(it->first);
            if (bundleIt != bundles.end())
            {
                for (auto propertyIt = bundleIt->second.begin(); propertyIt != bundleIt->second.end(); propertyIt++)
                {
                    setProperty(twPtr, it->second, *propertyIt);
                }
            }

            setProperty(twPtr, it->second, it->first);
        }

    }
}

ApplicationHUD::ApplicationHUD (Ibl::Application* application,
                                Ibl::IDevice* device,
                                Ibl::InputState* inputState,
                                Ibl::Scene* scene) : 
RenderHUD (application, device, inputState),
_scene (scene),
_controlsVisible (false)
{
    _applicationHud = this;
    Ibl::Camera* camera = _scene->camera();

    TwBar * bar = addTweakBar("IBLBaker", Ibl::Region2i(Ibl::Vector2i(5, 170), Ibl::Vector2i(250, 600)));
    TwDefine("IBLBaker color='90 90 90' text=light "); // Change TweakBar color and use dark text
    addTweakProperty(bar, _scene->camera()->exposureProperty(), _tweakProperties);
    addTweakProperty(bar, _scene->camera()->gammaProperty(), _tweakProperties);

    addToBundle(application->iblSphereEntity()->mesh(0)->material()->textureGammaProperty(), 
                application->sphereEntity()->mesh(0)->material()->textureGammaProperty());

    addTweakProperty(bar, application->iblSphereEntity()->mesh(0)->material()->textureGammaProperty(), _tweakProperties);

    addTweakProperty(bar, _scene->activeBrdfProperty(), _tweakProperties);


    addTweakProperty(bar, _scene->probes()[0]->hdrPixelFormatProperty(), _tweakProperties);
    addTweakProperty(bar, _scene->probes()[0]->sourceResolutionProperty(), _tweakProperties);

    addTweakProperty(bar, _scene->probes()[0]->diffuseResolutionProperty(), _tweakProperties);
    addTweakProperty(bar, _scene->probes()[0]->specularResolutionProperty(), _tweakProperties);
    addTweakProperty(bar, _scene->probes()[0]->sampleCountProperty(), _tweakProperties);
    addTweakProperty(bar, _scene->probes()[0]->samplesPerFrameProperty(), _tweakProperties);
    addTweakProperty(bar, _scene->probes()[0]->mipDropProperty(), _tweakProperties);
    addTweakProperty(bar, _scene->probes()[0]->environmentScaleProperty(), _tweakProperties);
    addTweakProperty(bar, _scene->probes()[0]->iblSaturationProperty(), _tweakProperties);
    addTweakProperty(bar, _scene->probes()[0]->iblContrastProperty(), _tweakProperties);
    addTweakProperty(bar, _scene->probes()[0]->iblHueProperty(), _tweakProperties);
    addTweakProperty(bar, _scene->probes()[0]->maxPixelRProperty(), _tweakProperties);
    addTweakProperty(bar, _scene->probes()[0]->maxPixelGProperty(), _tweakProperties);
    addTweakProperty(bar, _scene->probes()[0]->maxPixelBProperty(), _tweakProperties);

    setupMeshUI();

    _dialog->addButton (LoadEnvironment, L"Load Environment", 5, 
                        5, 125, 25, 0U, false, &_loadEnvironmentMapButton);
    _dialog->addButton (SaveEnvironment, L"Save Environment", 5, 
                        35, 125, 25, 0U, false, &_saveEnvironmentMapButton);    
    _dialog->addButton (ComputeEnvironment, L"Compute", 5, 
                        65, 125, 25, 0U, false, &_computeEnvironmentMapButton);    
    _dialog->addButton (CancelComputeEnvironment, L"Cancel", 5, 
                        95, 125, 25, 0U, false, &_cancelComputeEnvironmentButton);
    _dialog->addButton(LoadAsset, L"Load Asset", 5,
                        125, 125, 25, 0U, false, &_loadAsset);

    _dialog->addImageWidget(BrdfViewer, _scene->activeBrdf()->brdfLut(), Region2f(Vector2f(0.80f, 0.8f), Vector2f(0.98f, 0.98f)), &_brdfViewer);

    _loadEnvironmentMapButton->setEnabled(true);
    _loadEnvironmentMapButton->setVisible(false);
    _computeEnvironmentMapButton->setVisible(false);
    _cancelComputeEnvironmentButton->setVisible(false);
    _saveEnvironmentMapButton->setEnabled(true);
    _saveEnvironmentMapButton->setVisible(false);
    _brdfViewer->setVisible(false);

    _loadAsset->setEnabled(true);
    _loadAsset->setVisible(false);

    _saveEnvironmentMapButton->setVisible(false);

    TwDefine(" IBLBaker visible=false ");


    _controlsVisible = true;
    setUIVisible(true);
}

void
ApplicationHUD::setupMeshUI()
{
    if (_application->visualizedEntity()->meshes().size() > 0)
    {
        Material* baseMaterial = _application->visualizedEntity()->mesh(0)->material();
        Mesh* baseMesh = _application->visualizedEntity()->mesh(0);

        const std::vector<Mesh*>& meshes = _application->visualizedEntity()->meshes();
        for (auto meshIt = ++meshes.begin(); meshIt != meshes.end(); meshIt++)
        {
            Material* currentMaterial = (*meshIt)->material();
            Mesh* currentMesh = (*meshIt);
            addToBundle(baseMaterial->debugTermProperty(), currentMaterial->debugTermProperty());
            addToBundle(baseMaterial->specularWorkflowProperty(), currentMaterial->specularWorkflowProperty());
            addToBundle(baseMaterial->specularIntensityProperty(), currentMaterial->specularIntensityProperty());
            addToBundle(baseMaterial->roughnessScaleProperty(), currentMaterial->roughnessScaleProperty());
        }

        addTweakProperty(_tweakBars[0], baseMaterial->debugTermProperty(), _tweakProperties);
        addTweakProperty(_tweakBars[0], baseMaterial->specularWorkflowProperty(), _tweakProperties);
        addTweakProperty(_tweakBars[0], baseMaterial->specularIntensityProperty(), _tweakProperties);
        addTweakProperty(_tweakBars[0], baseMaterial->roughnessScaleProperty(), _tweakProperties);
    }
}

void
ApplicationHUD::cleanupMeshUI()
{
    if (_application->visualizedEntity()->meshes().size() > 0)
    {
        Material* baseMaterial = _application->visualizedEntity()->mesh(0)->material();
        Mesh* baseMesh = _application->visualizedEntity()->mesh(0);

        removeTweakProperty(_tweakBars[0],
            baseMaterial->debugTermProperty(),
            _tweakProperties, _tweakBundles);
        removeTweakProperty(_tweakBars[0],
            baseMesh->rotationProperty(),
            _tweakProperties, _tweakBundles);
        removeTweakProperty(_tweakBars[0],
            baseMaterial->specularWorkflowProperty(),
            _tweakProperties, _tweakBundles);
        removeTweakProperty(_tweakBars[0],
            baseMaterial->specularIntensityProperty(),
            _tweakProperties, _tweakBundles);
        removeTweakProperty(_tweakBars[0],
            baseMaterial->roughnessScaleProperty(),
            _tweakProperties, _tweakBundles);
    }
}

void
ApplicationHUD::showApplicationUI()
{
    TwDefine(" IBLBaker visible=true");

    _loadEnvironmentMapButton->setVisible(true);
    _computeEnvironmentMapButton->setVisible(true);
    _cancelComputeEnvironmentButton->setVisible(true);
    _saveEnvironmentMapButton->setVisible(true);
    _loadAsset->setVisible(true);
    _brdfViewer->setVisible(true);
}

void
ApplicationHUD::addToBundle(Property* key, Property* value)
{
    auto it = _tweakBundles.find(key);
    if (it == _tweakBundles.end())
    {
        _tweakBundles.insert(std::make_pair(key, std::set<Property*>()));
        it = _tweakBundles.find(key);
        assert(it != _tweakBundles.end());
    }
    it->second.insert(value);
}

void 
ApplicationHUD::setCB(const void *value, void * property)
{
    setProperty(value, property, _applicationHud->_tweakProperties, _applicationHud->_tweakBundles);
}

void 
ApplicationHUD::getCB(void *value, void * /*clientData*/ property)
{
    getProperty(value, property, _applicationHud->_tweakProperties);
}

ApplicationHUD::~ApplicationHUD()
{
}

bool
ApplicationHUD::create()
{
    return RenderHUD::create();
}

bool
ApplicationHUD::update(double elapsedTime)
{

    if (const Ibl::ITexture * texture = _scene->activeBrdf()->brdfLut())
    {
        _brdfViewer->setImage(texture);
    }

    if (_inputState->getKeyState(DIK_F1))
    {
        _controlsVisible = true;
        setUIVisible (true);
    }
    if (_inputState->getKeyState(DIK_F2))
    {
        _controlsVisible = false;
        setUIVisible (false);
    }

    if (RenderHUD::update(elapsedTime))
    {
        return true;
    }
    return false;
}

void
ApplicationHUD::render(const Ibl::Camera* camera)
{
    static int32_t scrollArea = 0;
    int32_t width = _deviceInterface->backbuffer()->width();
    int32_t height = _deviceInterface->backbuffer()->height();
#define IMGUI_IMPL 0
#if IMGUI_IMPL
// Bgfx rip off code for testing :).
    LOG("x = " << _inputState->_x << " y = " << _inputState->_y);

    imguiBeginFrame(_inputState, _inputState->_cursorPositionX, _inputState->_cursorPositionY, _inputState->leftMouseDown() ? IMGUI_MBUT_LEFT : 0 | _inputState->rightMouseDown() ? IMGUI_MBUT_RIGHT : 0, 0, width, height);

    float speed, middleGray, white, threshold;
    speed = middleGray = white = threshold = 0;

    imguiBeginScrollArea("Settings", 5, 10, width / 5, height / 3, &scrollArea);
    imguiSeparatorLine();

    imguiSlider("Speed", speed, 0.0f, 1.0f, 0.01f);
    imguiSeparator();

    imguiSlider("Middle gray", middleGray, 0.1f, 1.0f, 0.01f);
    imguiSlider("White point", white, 0.1f, 2.0f, 0.01f);
    imguiSlider("Threshold", threshold, 0.1f, 2.0f, 0.01f);
    static float blah[3] = {0.0, 1.0, 1.0};
    static bool activated = true;
    imguiColorWheel("Diffuse color:", blah, activated);
    imguiEndScrollArea();


    imguiEndFrame();
#endif

    RenderHUD::render(camera);
}

BOOL selectFilenameLoad(HWND windowHandle, 
                        LPWSTR filename, 
                        LPWSTR filter)
{
   OPENFILENAME ofn;       // common dialog box structure
   WCHAR dirName[MAX_FILE_PATH_NAME];

   BOOL fileOpenStatus;

   // Initialize OPENFILENAME structure
   memset(&ofn, 0, sizeof(OPENFILENAME));
   ofn.lStructSize = sizeof(OPENFILENAME);
   ofn.hwndOwner = windowHandle;
   ofn.lpstrFile = filename;
   ofn.nMaxFile = MAX_FILE_PATH_NAME;
   ofn.lpstrFilter = filter; 
   ofn.nFilterIndex = 1;
   ofn.lpstrFileTitle = nullptr;
   ofn.nMaxFileTitle = 0;
   ofn.lpstrInitialDir = nullptr;
   ofn.lpstrFileTitle = L"Open File";
   ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

   _wgetcwd(dirName, MAX_FILE_PATH_NAME);
   fileOpenStatus = ::GetOpenFileName(&ofn);
   _wchdir(dirName);

   return fileOpenStatus;
}

BOOL selectFilenameSave(WindowHandle windowHandle, 
                        LPWSTR filename, 
                        LPWSTR filter)
{
   OPENFILENAME ofn = { sizeof(OPENFILENAME), windowHandle, NULL,
      filter, NULL, 0, 1,
      filename, MAX_FILE_PATH_NAME, NULL, 0,
      NULL, L"Save As", NULL,
      0, 0, NULL,
      0, NULL, NULL };
   BOOL fileSaveStatus;
   WCHAR dirName[MAX_FILE_PATH_NAME];

   _wgetcwd(dirName, MAX_FILE_PATH_NAME);
   fileSaveStatus = ::GetSaveFileName(&ofn);
   _wchdir(dirName);

   return fileSaveStatus;
}

void
ApplicationHUD::handleEvent (UINT eventId, int controlId, Ibl::Control* control)
{
    InitCommonControls();
    switch (controlId)
    {
        case LoadEnvironment:
        {
            LOG ("Loading an environment");
            WCHAR selectedFilePathName[MAX_FILE_PATH_NAME];
            memset(selectedFilePathName, 0, MAX_FILE_PATH_NAME * sizeof(WCHAR));
            WCHAR * filter = L"All\0*.*\0Text\0*.TXT\0";

            if(selectFilenameLoad (nullptr, selectedFilePathName, filter))
            {
                std::wstring inputString(selectedFilePathName);
                std::string  filePathName(inputString.begin(), inputString.end());

                _application->loadEnvironment(std::string(filePathName.c_str()));
            }
            break;
        }
        case SaveEnvironment:
        {
            WCHAR selectedFilePathName[MAX_FILE_PATH_NAME];
            memset(selectedFilePathName, 0, MAX_FILE_PATH_NAME * sizeof(WCHAR));
            WCHAR * filter = L"DirectDraw Surfaces(*.dds)\0*.dds\0\0";
            if (Ibl::selectFilenameSave(_application->window()->windowHandle(),
                                    selectedFilePathName, filter))
            {
                std::wstring inputString(selectedFilePathName);
                std::string  filePathName(inputString.begin(), inputString.end());

                _application->saveImages(filePathName.c_str());
            }
            break;
        }
        case ComputeEnvironment:
        {
            _application->compute();
            break;
        }
        case CancelComputeEnvironment:
        {
            _application->cancel();
            break;
        }
        case LoadAsset:
        {
            LOG("Loading an asset");
            WCHAR selectedFilePathName[MAX_FILE_PATH_NAME];
            memset(selectedFilePathName, 0, MAX_FILE_PATH_NAME * sizeof(WCHAR));
            WCHAR * filter = L"All\0*.*\0FBX\0*.fbx\0";
            
            if (selectFilenameLoad(nullptr, selectedFilePathName, filter))
            {
                std::wstring inputString(selectedFilePathName);
                std::string  filePathName(inputString.begin(), inputString.end());
                
                cleanupMeshUI();

                _application->loadAsset(std::string(filePathName.c_str()));

                _application->visualizedEntity()->mesh(0)->rotationProperty()->set(Ibl::Vector3f(0,0,360));

                setupMeshUI();

            }
            break;
        }
    }
    RenderHUD::handleEvent (eventId, controlId, control);
}

}
