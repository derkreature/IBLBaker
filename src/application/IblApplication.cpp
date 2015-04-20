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

#include <IblApplication.h>
#include <IblApplicationHUD.h>
#include <IblAssetManager.h>
#include <IblRenderDeviceD3D11.h>
#include <IblColorPass.h>
#include <IblTextureMgr.h>
#include <IblShaderMgr.h>
#include <IblRenderHUD.h>
#include <IblCamera.h>
#include <IblScene.h>
#include <IblEntity.h>
#include <IblMaterial.h>
#include <IblInputManager.h>
#include <IblCameraManager.h>
#include <IblIBLRenderPass.h>
#include <IblPostEffectsMgr.h>
#include <IblIBLProbe.h>
#include <IblTitles.h>
#include <IblBrdf.h>
#include <IblImageWidget.h>
#include <Iblimgui.h>
#include <strstream>
#include <Iblimgui.h>

namespace Ibl
{
namespace
{

ImguiEnumVal IblEnum[] =
{
    { Application::HDR, "HDR" },
    { Application::MDR, "MDR" },
    { Application::LDR, "LDR" }
};

static const EnumTweakType IBLModeType(&IblEnum[0], 3, "IBLMode");


ImguiEnumVal ModelEnum[] =
{
    { Application::UserModel, "User" },
    { Application::ShaderBallModel, "Shader Ball" },
};
static const EnumTweakType ModelEnumType(&ModelEnum[0], 2, "Model");

// Half arsed for demo.
ImguiEnumVal AppSpecularWorkflowEnum[] =
{
    { RoughnessMetal, "Roughness/Metal" },
    { GlossMetal, "Gloss/Metal" },
    { RoughnessInverseMetal, "Roughness/Inverse Metal" },
    { GlossInverseMetal, "Gloss/Inverse Metal" },
};

static const EnumTweakType WorkflowEnumType(&AppSpecularWorkflowEnum[0], 4, "Workflow");

ImguiEnumVal DebugAOVEnum[] =
{
    { Ibl::NoDebugTerm, "No Debug Term" },
    { Ibl::NormalTerm, "Normals" },
    { Ibl::AmbientOcclusionTerm, "Ambient Occlusion" },
    { Ibl::AlbedoTerm, "Albedo" },
    { Ibl::IBLDiffuseTerm, "IBL Diffuse Radiance" },
    { Ibl::IBLSpecularTerm, "IBL Specular" },
    { Ibl::MetalTerm, "Metalness" },
    { Ibl::RoughnessTerm, "Roughness" },
    { Ibl::BrdfTerm, "Brdf" }
};
static const EnumTweakType DebugAOVType(&DebugAOVEnum[0], 9, "debugAOV");

}

Application::Application(ApplicationHandle instance) : 
    Node(),
    _instance (instance),
    _mainWindow(nullptr),
    _device(nullptr),
    _colorPass(nullptr),
    _iblRenderPass(nullptr),
    _cameraManager(nullptr),
    _inputMgr(nullptr),
    _renderHUD(nullptr),
    _visualizedEntity (nullptr),
    _sphereEntity(nullptr),
    _iblSphereEntity(nullptr),
    _scene(nullptr),
    _headless(false),
    _windowWidth (1280),
    _windowHeight(720),
    _windowed (true),
    // TODO: Fix, not sure what th fuck happened here.
    _visualizationSpaceProperty(new IntProperty(this, "Visualization Type")),
    _currentVisualizationSpaceProperty(new IntProperty(this, "Visualization Space")),
    _hdrFormatProperty(new PixelFormatProperty(this, "HDR Format")),
    _probeResolutionProperty(new IntProperty(this, "Visualization Type")),
    _modelVisualizationProperty(new IntProperty(this, "Model", new TweakFlags(&ModelEnumType, "Model"))),
    _constantRoughnessProperty(new FloatProperty(this, "Constant Roughness")),
    _constantMetalnessProperty(new FloatProperty(this, "Constant Metalness")),
    _specularWorkflowProperty(new IntProperty(this, "Specular Workflow", new TweakFlags(&WorkflowEnumType, "Workflow"))),
    _specularIntensityProperty(new FloatProperty(this, "Specular Intensity")),
    _roughnessScaleProperty(new FloatProperty(this, "Roughness Scale")),
    _debugTermProperty(new IntProperty(this, "Debug Visualization", new TweakFlags(&DebugAOVType, "Material"))),
    _defaultAsset("data\\meshes\\pistol\\pistol.obj"),
    _runTitles(false)
{
    _modelVisualizationProperty->set(0);
    _visualizationSpaceProperty->set(Ibl::Application::HDR);
    _currentVisualizationSpaceProperty->set(-1);
    _specularIntensityProperty->set(1.0f);
    _roughnessScaleProperty->set(1.0f);


#ifdef _DEBUG
    assert (_instance);
#endif
}

Application::~Application()
{
    _instance = nullptr;
    safedelete(_cameraManager);
    safedelete(_colorPass);
    safedelete(_iblRenderPass);
    safedelete(_scene);
    safedelete(_renderHUD);
    safedelete(_inputMgr);
    safedelete(_device);


}

bool
Application::parseOptions(int argc, char* argv[])
{
    // Scan for --help
    for (int32_t argId = 0; argId < argc; argId++)
    {
        if (std::string("--help") == argv[argId])
        {
            LOG ("IBLBaker: Specular and Irradiance cubemap baking tool")

            return false;
        }
    }

    return true;
}

ApplicationHandle  
Application::instance() const
{
    return _instance;
}

const Timer&
Application::timer() const
{
    return _timer;
}

Timer&
Application::timer()
{
    return _timer;
}

IntProperty*
Application::modelVisualizationProperty()
{
    return _modelVisualizationProperty;
}

FloatProperty*
Application::constantRoughnessProperty()
{
    return _constantRoughnessProperty;
}

FloatProperty*
Application::constantMetalnessProperty()
{
    return _constantMetalnessProperty;
}

IntProperty*
Application::specularWorkflowProperty()
{
    return _specularWorkflowProperty;
}

IntProperty*
Application::debugTermProperty()
{
    return _debugTermProperty;
}

FloatProperty*
Application::specularIntensityProperty()
{
    return _specularIntensityProperty;
}

FloatProperty*
Application::roughnessScaleProperty()
{
    return _roughnessScaleProperty;
}

const Window*
Application::window() const
{
    return _mainWindow;
}

Window*
Application::window()
{
    return _mainWindow;
}

void
Application::initialize()
{
    DeviceD3D11* device = new DeviceD3D11();
    Ibl::ApplicationRenderParameters deviceParams(this, "IBLBaker", Ibl::Vector2i(_windowWidth, _windowHeight), _windowed);

    if (device->initialize(deviceParams))
    {
        _device = device;
        imguiCreate(_device);

        _mainWindow = _device->renderWindow();

        _scene = new Ibl::Scene(_device);
        _inputMgr = new InputManager (this);
        _cameraManager = new Ibl::FocusedDampenedCamera(_inputMgr->inputState());
        _cameraManager->create(_scene);
        _cameraManager->setTranslation(Ibl::Vector3f(0, -200, 0));
        _cameraManager->setRotation(Ibl::Vector3f(10, -15, -220));

        loadAsset(_visualizedEntity, _defaultAsset, "", true);

        loadAsset(_shaderBallEntity, "data\\meshes\\shaderBall\\shaderBall.fbx", "", false);

        _sphereEntity = _scene->load("data\\meshes\\sphere\\sphere.obj", 
                                     "data\\meshes\\sphere\\sphere.material");
        _sphereEntity->mesh(0)->scaleProperty()->set(Ibl::Vector3f(10,10,10));

        _iblSphereEntity = _scene->load("data\\meshes\\sphere\\sphere.obj", 
                                        "data\\meshes\\sphere\\iblsphere.material");

        _iblSphereEntity->mesh(0)->scaleProperty()->set(Ibl::Vector3f(10,10,10));

        // Initialize render passes
        _colorPass = new Ibl::ColorPass(_device);
        _iblRenderPass = new Ibl::IBLRenderPass(_device);

        // Add a probe.
        _probe = _scene->addProbe();
        // Default samples
        _probe->sampleCountProperty()->set(128);
        _probe->samplesPerFrameProperty()->set(128);

        _probe->hdrPixelFormatProperty()->set(_hdrFormatProperty->get()),
        _probe->sourceResolutionProperty()->set(_probeResolutionProperty->get());

        Vector4f maxPixelValue = _iblSphereEntity->mesh(0)->material()->albedoMap()->maxValue();
        _probe->maxPixelRProperty()->set(maxPixelValue.x);
        _probe->maxPixelGProperty()->set(maxPixelValue.y);
        _probe->maxPixelBProperty()->set(maxPixelValue.z);

        // Good to go.
        _renderHUD = new Ibl::ApplicationHUD(this, _device, _inputMgr->inputState(), _scene);
        _renderHUD->create();

        {
            _renderHUD->setLogoVisible(true);
            _renderHUD->logo()->setBlendIn(6.0f);
            _renderHUD->showApplicationUI();
        }
        syncVisualization();
    }
    else
    {
        delete device;
        THROW ("Failed to create render device");
    }
}

void
Application::syncVisualization()
{
    setupModelVisibility(_visualizedEntity, _modelVisualizationProperty->get() == Ibl::Application::UserModel);
    setupModelVisibility(_shaderBallEntity, _modelVisualizationProperty->get() != Ibl::Application::UserModel);
}

void
Application::setupModelVisibility(Ibl::Entity* entity, bool visibility)
{
    const std::vector<Mesh*>& meshes = entity->meshes();
    for (auto meshIt = meshes.begin(); meshIt != meshes.end(); meshIt++)
    {
        (*meshIt)->setVisible(visibility);
    }
}

void
Application::run()
{
    _timer.startTimer();

    // [MattD][Thermal] Turn on to save your gpu some needless processing.
    #define DO_NOT_FRY_GPU 1
    #if DO_NOT_FRY_GPU
        _timer.setLockFrameCounter(60);
    #endif

    do
    {
        _timer.update();
        _inputMgr->update();
        updateApplication();

    }
    while (!purgeMessages());
}

bool 
Application::purgeMessages() const
{
    MSG msg;
    while(PeekMessage (&msg, 0, 0, 0, PM_REMOVE))
    {
        if(msg.message == WM_QUIT)
            return true;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return false;
}

bool
Application::loadParameters()
{
    if (std::unique_ptr<pugi::xml_document> doc = 
        std::unique_ptr<pugi::xml_document>(Ibl::AssetManager::assetManager()->openXmlDocument("data/iblBakerConfig.xml")))
    {
        pugi::xpath_node configNode = doc->select_single_node("/Config");
        if (configNode)
        {
            if (const char* xpathValue = configNode.node().attribute("DefaultAsset").value())
            {
                _defaultAsset = xpathValue;
            }

            if (const char* xpathValue = configNode.node().attribute("WindowWidth").value())
            {
                _windowWidth = atoi(xpathValue);
            }
            if (const char* xpathValue = configNode.node().attribute("WindowHeight").value())
            {
                _windowHeight = atoi(xpathValue);
            }

            if (const char* xpathValue = configNode.node().attribute("Windowed").value())
            {
                _windowed = atoi(xpathValue) == 1 ? true : false;
            }

            if (const char* xpathValue = configNode.node().attribute("Titles").value())
            {
                _runTitles = atoi(xpathValue) == 1 ? true : false;
            }
            

            if (const char* xpathValue = configNode.node().attribute("IBLFormat").value())
            {
                _hdrFormatProperty->set(atoi(xpathValue) == 16 ? PF_FLOAT16_RGBA : PF_FLOAT32_RGBA);
            }

            if (const char* xpathValue = configNode.node().attribute("SourceEnvironmentResolution").value())
            {
                _probeResolutionProperty->set(atoi(xpathValue));
            }

            if (const char* xpathValue = configNode.node().attribute("SpecularWorkflow").value())
            {
                std::string workflowValue(xpathValue);

                SpecularWorkflow workflow;
                if (workflowValue == std::string("RoughnessMetal"))
                {
                    workflow = Ibl::RoughnessMetal;
                }
                else if (workflowValue == std::string("GlossMetal"))
                {
                    workflow = Ibl::GlossMetal;
                }
                else if (workflowValue == std::string("RoughnessInverseMetal"))
                {
                    workflow = Ibl::RoughnessInverseMetal;
                }
                else if (workflowValue == std::string("GlossInverseMetal"))
                {
                    workflow = Ibl::GlossInverseMetal;
                }
                else
                {
                    LOG("Could not find a valid preference for specular workflow, using RoughnessMetal.")
                }

                _specularWorkflowProperty->set(workflow);
            }
            else
            {
                LOG("Could not locate a preference for specular workflow, using RoughnessMetal.")
            }

            if (_windowWidth <= 0 || _windowHeight <= 0)
            {
                _windowWidth = 1280;
                _windowHeight = 720;
            }

            return true;
        }
    }
    return false;
}

bool
Application::saveParameters() const
{
    std::unique_ptr<pugi::xml_document> doc;
    doc.reset(new pugi::xml_document());
    if (doc)
    {

        // Quick sanitize for close while minimized.
        int32_t width = window()->width();
        int32_t height = window()->height();

        if (width <= 0 || height <= 0)
        {
            width = 1280;
            height = 720;
        }

        pugi::xml_node configNode = doc->append_child(pugi::node_element);
        configNode.set_name("Config");

        configNode.append_attribute("DefaultAsset").set_value(_defaultAsset.c_str());

        char buffer[512];
        memset(buffer, 0, sizeof(char) * 512);
        _itoa(window()->width(), buffer, 10);
        configNode.append_attribute("WindowWidth").set_value(buffer);

        memset(buffer, 0, sizeof(char) * 512);
        _itoa(window()->height(), buffer, 10);
        configNode.append_attribute("WindowHeight").set_value(buffer);

        memset(buffer, 0, sizeof(char) * 512);
        _itoa(window()->windowed() ? 1 : 0, buffer, 10);
        configNode.append_attribute("Windowed").set_value(buffer);

        memset(buffer, 0, sizeof(char) * 512);
        _itoa(_runTitles ? 1 : 0, buffer, 10);
        configNode.append_attribute("Titles").set_value(buffer);

        int format = _scene->probes()[0]->hdrPixelFormat() == PF_FLOAT16_RGBA ? 16 : 32;
        memset(buffer, 0, sizeof(char) * 512);
        _itoa(format, buffer, 10);
        configNode.append_attribute("IBLFormat").set_value(buffer);

        memset(buffer, 0, sizeof(char) * 512);
        _itoa(_scene->probes()[0]->sourceResolutionProperty()->get(), buffer, 10);
        configNode.append_attribute("SourceEnvironmentResolution").set_value(buffer);


        std::string workflow("RoughnessMetal");
        switch (_specularWorkflowProperty->get())
        {
            case Ibl::RoughnessMetal:
                workflow = "RoughnessMetal";
                break;
            case Ibl::GlossMetal:
                workflow = "GlossMetal";
                break;
            case Ibl::RoughnessInverseMetal:
                workflow = "RoughnessInverseMetal";
                break;
            case Ibl::GlossInverseMetal:
                workflow = "GlossInverseMetal";
                break;
        }
        configNode.append_attribute("SpecularWorkflow").set_value(workflow.c_str());

        doc->save_file("data/iblBakerConfig.xml");

    }

    return true;
}

void
Application::updateApplication()
{
    float elapsedTime = (float)(_timer.elapsedTime());
    _cameraManager->update(elapsedTime, true, true);
    _device->update();
    _scene->update();
    _renderHUD->update(elapsedTime);

    Ibl::InputState* inputState = _inputMgr->input().inputState();
    if (inputState->leftMouseDown() && !inputState->hasGUIFocus() && 
        inputState->getKeyState(DIK_LCONTROL))
    {
        Vector3f rotation;
        Ibl::Entity* entity = visualizedEntity();

        if (modelVisualizationProperty()->get() == Ibl::Application::ShaderBallModel)
            entity = _shaderBallEntity;
        else
            entity = _visualizedEntity;

        rotation = entity->mesh(0)->rotation();

        rotation += Ibl::Vector3f(((float)inputState->_y),((float)inputState->_x), 0);
        std::vector<Mesh*> meshes = entity->meshes();
        for (auto it = meshes.begin(); it != meshes.end(); it++)
            (*it)->rotationProperty()->set(rotation);
    }

    if (_visualizationSpaceProperty->get() != _currentVisualizationSpaceProperty->get())
    {
        updateVisualizationType();
    }

    Ibl::Camera* camera = _scene->camera();
    camera->updateViewProjection();
    camera->cacheCameraTransforms();
    _device->beginRender();
    // Perform application work.
    const Ibl::Vector4f clearColor = Ibl::Vector4f(0, 1,0,1);
    _device->bindFrameBuffer (_device->postEffectsMgr()->sceneFrameBuffer());
    _device->clearSurfaces (0, Ibl::CLEAR_TARGET | Ibl::CLEAR_ZBUFFER|Ibl::CLEAR_STENCIL, 
                                clearColor.x, clearColor.y, clearColor.z, clearColor.w);
    if (!_inputMgr->inputState()->leftMouseDown())
        _iblRenderPass->render(_scene);


     // Camera input.
    _device->bindFrameBuffer(_device->postEffectsMgr()->sceneFrameBuffer());
    
    _colorPass->render(_scene);

	 // Finalize post effects.
    _device->postEffectsMgr()->render(camera);

    _device->bindFrameBuffer(_device->deviceFrameBuffer());
    _renderHUD->render(camera);

	// Present to back buffre
    _device->present();
}

bool
Application::loadEnvironment(const std::string& filePathName)
{
    bool result = false; 
    if (AssetManager::fileExists(filePathName))
    {
        // TODO: Reference counting.
        _device->textureMgr()->recycle(_sphereEntity->mesh(0)->material()->albedoMap());
        _sphereEntity->mesh(0)->material()->setAlbedoMap(filePathName);
        _iblSphereEntity->mesh(0)->material()->setAlbedoMap(filePathName);

        _scene->probes()[0]->uncache();

        // Is the environment a cubemap, if not, load up spherical versions of shaders.
        if (const ITexture* texture = _sphereEntity->mesh(0)->material()->albedoMap())
        {
            //texture->textureCount
            if (texture->isCubeMap())
            {
                // Setup cubemap shaders.
                _sphereEntity->mesh(0)->material()->setShaderName("EnvironmentSphere");
                _iblSphereEntity->mesh(0)->material()->setShaderName("SinglePassEnvironment");
            }
            else
            {
                // Setup spherical map shaders.
                _sphereEntity->mesh(0)->material()->setShaderName("EnvironmentSphereSpherical");
                _iblSphereEntity->mesh(0)->material()->setShaderName("SinglePassSphericalEnvironment");
            }


            Ibl::PixelFormat format = texture->format();
            float inputGamma = 1.0;
            // Setup default gamma. 1.0 for HDR, 2.2 for LDR
            if (format == Ibl::PF_FLOAT32_RGBA ||
                format == Ibl::PF_FLOAT16_RGBA ||
                format == Ibl::PF_FLOAT32_RGB ||
                format == Ibl::PF_FLOAT16_GR ||
                format == Ibl::PF_FLOAT32_GR ||
                format == Ibl::PF_FLOAT32_R ||
                format == Ibl::PF_FLOAT16_R)
            {
                 inputGamma = 1.0f;
            }
            else
            {
                 inputGamma = 2.2f;
            }

            _iblSphereEntity->mesh(0)->material()->textureGammaProperty()->set(1.0f);
            _sphereEntity->mesh(0)->material()->textureGammaProperty()->set(inputGamma);
        }

        _device->shaderMgr()->resolveShaders(_sphereEntity);
        _device->shaderMgr()->resolveShaders(_iblSphereEntity);


        Vector4f maxPixelValue = _iblSphereEntity->mesh(0)->material()->albedoMap()->maxValue();
        _probe->maxPixelRProperty()->set(maxPixelValue.x);
        _probe->maxPixelGProperty()->set(maxPixelValue.y);
        _probe->maxPixelBProperty()->set(maxPixelValue.z);

    }
    else
    {
        LOG ("Could not open file " << filePathName);
    }

    return result;
}

bool
Application::saveImages(const std::string& filePathName, bool gameOnly)
{
    if (Ibl::IBLProbe* probe = _scene->probes()[0])
    {
        bool trimmed = true;

        std::vector <std::string> pmtConversionQueue;

        size_t pathEnd = filePathName.rfind("/");
        if (pathEnd == std::string::npos)
        {
            pathEnd = filePathName.rfind("\\");
            if (pathEnd == std::string::npos)
            {
                LOG ("Failed to find path end in " << filePathName);
                return false;
            }
        }
        size_t extension = filePathName.rfind(".");
        if (extension == std::string::npos)
        {
            LOG ("Failed to find file extension in " << filePathName);
            return false;
        }

        std::string pathName = filePathName.substr(0, pathEnd+1);
        std::string fileNameBase = filePathName.substr(pathEnd+1, extension-(pathEnd+1));

        LOG ("PathName " << pathName);
        LOG ("FileName base " << fileNameBase);

        if (trimmed)
        {
            std::string specularHDRPath = pathName + fileNameBase + "SpecularHDR.dds";
            std::string diffuseHDRPath = pathName + fileNameBase + "DiffuseHDR.dds";
            std::string envHDRPath = pathName + fileNameBase + "EnvHDR.dds";

            std::string specularMDRPath = pathName + fileNameBase + "SpecularMDR.dds";
            std::string diffuseMDRPath = pathName + fileNameBase + "DiffuseMDR.dds";
            std::string envMDRPath = pathName + fileNameBase + "EnvMDR.dds";

            std::string brdfLUTPath = pathName + fileNameBase + "Brdf.dds";

            LOG("Saving RGBM MDR diffuse to " << diffuseMDRPath);
            probe->diffuseCubeMapMDR()->save(diffuseMDRPath, true /* fix seams */, false /* split to RGB MMM */);
            LOG("Saving RGBM MDR specular to " << specularMDRPath);
            probe->specularCubeMapMDR()->save(specularMDRPath, true /* fix seams */, false /* split to RGB MMM */);
            LOG("Saving RGBM MDR environment to " << envMDRPath);
            probe->environmentCubeMapMDR()->save(envMDRPath, true /* fix seams */, false /* split to RGB MMM */);


            // Save the brdf too.
            _scene->activeBrdf()->brdfLut()->save(brdfLUTPath, false, false);


// This operation on a 2k floating point cubemap with a full mip chain blows
// through remaining addressable memory on 32bit.
#if _64BIT
            probe->environmentCubeMap()->save(envHDRPath, true, false);
#endif
            LOG ("Saving HDR diffuse to " << diffuseHDRPath);
            probe->diffuseCubeMap()->save(diffuseHDRPath, true, false);

            LOG ("Saving HDR specular to " << specularHDRPath);
            probe->specularCubeMap()->save(specularHDRPath, true, false);

            return true;
        }
    }

    return false;
}

void
Application::compute()
{
    if (Ibl::IBLProbe* probe = _scene->probes()[0])
    {
        if (probe->computed())
        {
            probe->uncache();
        }
        else
        {
            probe->markComputed(false);
        }
    }
}

void
Application::pause()
{
    throw std::runtime_error("Not implemented!");
}

void
Application::cancel()
{
    if (Ibl::IBLProbe* probe = _scene->probes()[0])
    {
        probe->markComputed(true);
    }
}

IntProperty*
Application::visualizationSpaceProperty()
{
    return _visualizationSpaceProperty;
}

void
Application::updateVisualizationType()
{
    _currentVisualizationSpaceProperty->set(_visualizationSpaceProperty->get());

    switch (_currentVisualizationSpaceProperty->get())
    {
        case Application::HDR:
            _visualizedEntity->mesh(0)->material()->setTechniqueName("Default");
            break;
    }
    _device->shaderMgr()->resolveShaders(_visualizedEntity);

}

Entity*
Application::visualizedEntity()
{
    return _visualizedEntity;
}

Entity*
Application::shaderBallEntity()
{
    return _shaderBallEntity;
}

Entity*
Application::sphereEntity()
{
    return _sphereEntity;
}

Entity*
Application::iblSphereEntity()
{
    return _iblSphereEntity;
}

void
Application::loadAsset(Entity*& targetEntity,
                       const std::string& assetPathName, 
                       const std::string& materialPathName,
                       bool userAsset)
{

    if (targetEntity && userAsset)
    {
        _scene->destroy(targetEntity);
    }

    targetEntity = _scene->load(assetPathName, materialPathName);
    //if (userAsset)
    { 
        const std::vector<Mesh*>& meshes = targetEntity->meshes();
        for (auto meshIt = meshes.begin(); meshIt != meshes.end(); meshIt++)
        {
            // Copy over the last settings.
            (*meshIt)->material()->specularWorkflowProperty()->set(_specularWorkflowProperty->get());
            (*meshIt)->material()->specularIntensityProperty()->set(_specularIntensityProperty->get());
            (*meshIt)->material()->roughnessScaleProperty()->set(_roughnessScaleProperty->get());
        }
    }
}
}