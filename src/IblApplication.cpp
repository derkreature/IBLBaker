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
#include <CtrAssetManager.h>
#include <CtrRenderDeviceD3D11.h>
#include <CtrColorPass.h>
#include <CtrTextureMgr.h>
#include <CtrShaderMgr.h>
#include <CtrRenderHUD.h>
#include <CtrCamera.h>
#include <CtrScene.h>
#include <CtrEntity.h>
#include <CtrMaterial.h>
#include <CtrInputManager.h>
#include <CtrCameraManager.h>
#include <CtrIBLRenderPass.h>
#include <CtrPostEffectsMgr.h>
#include <CtrIBLProbe.h>
#include <CtrTitles.h>
#include <CtrBrdf.h>
#include <CtrImageWidget.h>
#include <Ctrimgui.h>
#include <strstream>
#include <Ctrimgui.h>

namespace Ctr
{
namespace
{

ImguiEnumVal IblEnum[] =
{
    { IBLApplication::HDR, "HDR" },
    { IBLApplication::MDR, "MDR" },
    { IBLApplication::LDR, "LDR" }
};

static const EnumTweakType IBLModeType(&IblEnum[0], 3, "IBLMode");


ImguiEnumVal ModelEnum[] =
{
    { IBLApplication::UserModel, "User" },
    { IBLApplication::ShaderBallModel, "Shader Ball" },
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
    { Ctr::NoDebugTerm, "No Debug Term" },
    { Ctr::NormalTerm, "Normals" },
    { Ctr::AmbientOcclusionTerm, "Ambient Occlusion" },
    { Ctr::AlbedoTerm, "Albedo" },
    { Ctr::IBLDiffuseTerm, "IBL Diffuse Radiance" },
    { Ctr::IBLSpecularTerm, "IBL Specular" },
    { Ctr::MetalTerm, "Metalness" },
    { Ctr::RoughnessTerm, "Roughness" },
    { Ctr::BrdfTerm, "Brdf" }
};
static const EnumTweakType DebugAOVType(&DebugAOVEnum[0], 9, "debugAOV");

}

IBLApplication::IBLApplication(ApplicationHandle instance) : 
    Ctr::Application(instance),
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
    _visualizationSpaceProperty->set(Ctr::IBLApplication::HDR);
    _currentVisualizationSpaceProperty->set(-1);
    _specularIntensityProperty->set(1.0f);
    _roughnessScaleProperty->set(1.0f);


#ifdef _DEBUG
    assert (_instance);
#endif
}

IBLApplication::~IBLApplication()
{
    safedelete(_cameraManager);
    safedelete(_colorPass);
    safedelete(_iblRenderPass);
    safedelete(_scene);
    safedelete(_renderHUD);
    safedelete(_inputMgr);
}

bool
IBLApplication::parseOptions(int argc, char* argv[])
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
IBLApplication::instance() const
{
    return _instance;
}

const Timer&
IBLApplication::timer() const
{
    return _timer;
}

Timer&
IBLApplication::timer()
{
    return _timer;
}

IntProperty*
IBLApplication::modelVisualizationProperty()
{
    return _modelVisualizationProperty;
}

FloatProperty*
IBLApplication::constantRoughnessProperty()
{
    return _constantRoughnessProperty;
}

FloatProperty*
IBLApplication::constantMetalnessProperty()
{
    return _constantMetalnessProperty;
}

IntProperty*
IBLApplication::specularWorkflowProperty()
{
    return _specularWorkflowProperty;
}

IntProperty*
IBLApplication::debugTermProperty()
{
    return _debugTermProperty;
}

FloatProperty*
IBLApplication::specularIntensityProperty()
{
    return _specularIntensityProperty;
}

FloatProperty*
IBLApplication::roughnessScaleProperty()
{
    return _roughnessScaleProperty;
}

const Window*
IBLApplication::window() const
{
    return _mainWindow;
}

Window*
IBLApplication::window()
{
    return _mainWindow;
}

void
IBLApplication::initialize()
{
    DeviceD3D11* device = new DeviceD3D11();
    Ctr::ApplicationRenderParameters deviceParams(this, "IBLBaker", Ctr::Vector2i(_windowWidth, _windowHeight), _windowed, false);

    if (device->initialize(deviceParams))
    {
        _device = device;
        imguiCreate(_device);

        _mainWindow = _device->renderWindow();

        _scene = new Ctr::Scene(_device);
        _inputMgr = new InputManager (this);
        _cameraManager = new Ctr::FocusedDampenedCamera(_inputMgr->inputState());
        _cameraManager->create(_scene);
        _cameraManager->setTranslation(Ctr::Vector3f(0, -200, 0));
        _cameraManager->setRotation(Ctr::Vector3f(10, -15, -220));

        loadAsset(_visualizedEntity, _defaultAsset, "", true);

        loadAsset(_shaderBallEntity, "data\\meshes\\shaderBall\\shaderBall.fbx", "", false);

        _sphereEntity = _scene->load("data\\meshes\\sphere\\sphere.obj", 
                                     "data\\meshes\\sphere\\sphere.material");
        _sphereEntity->mesh(0)->scaleProperty()->set(Ctr::Vector3f(10,10,10));

        _iblSphereEntity = _scene->load("data\\meshes\\sphere\\sphere.obj", 
                                        "data\\meshes\\sphere\\iblsphere.material");

        _iblSphereEntity->mesh(0)->scaleProperty()->set(Ctr::Vector3f(10,10,10));

        // Initialize render passes
        _colorPass = new Ctr::ColorPass(_device);
        _iblRenderPass = new Ctr::IBLRenderPass(_device);

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
        _renderHUD = new IBLApplicationHUD(this, _device, _inputMgr->inputState(), _scene);
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
IBLApplication::syncVisualization()
{
    setupModelVisibility(_visualizedEntity, _modelVisualizationProperty->get() == Ctr::IBLApplication::UserModel);
    setupModelVisibility(_shaderBallEntity, _modelVisualizationProperty->get() != Ctr::IBLApplication::UserModel);
}

void
IBLApplication::setupModelVisibility(Ctr::Entity* entity, bool visibility)
{
    const std::vector<Mesh*>& meshes = entity->meshes();
    for (auto meshIt = meshes.begin(); meshIt != meshes.end(); meshIt++)
    {
        (*meshIt)->setVisible(visibility);
    }
}

void
IBLApplication::run()
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
IBLApplication::purgeMessages() const
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
IBLApplication::loadParameters()
{
    if (std::unique_ptr<pugi::xml_document> doc = 
        std::unique_ptr<pugi::xml_document>(Ctr::AssetManager::assetManager()->openXmlDocument("data/iblBakerConfig.xml")))
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
                    workflow = Ctr::RoughnessMetal;
                }
                else if (workflowValue == std::string("GlossMetal"))
                {
                    workflow = Ctr::GlossMetal;
                }
                else if (workflowValue == std::string("RoughnessInverseMetal"))
                {
                    workflow = Ctr::RoughnessInverseMetal;
                }
                else if (workflowValue == std::string("GlossInverseMetal"))
                {
                    workflow = Ctr::GlossInverseMetal;
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
IBLApplication::saveParameters() const
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
            case Ctr::RoughnessMetal:
                workflow = "RoughnessMetal";
                break;
            case Ctr::GlossMetal:
                workflow = "GlossMetal";
                break;
            case Ctr::RoughnessInverseMetal:
                workflow = "RoughnessInverseMetal";
                break;
            case Ctr::GlossInverseMetal:
                workflow = "GlossInverseMetal";
                break;
        }
        configNode.append_attribute("SpecularWorkflow").set_value(workflow.c_str());

        doc->save_file("data/iblBakerConfig.xml");

    }

    return true;
}

void
IBLApplication::updateApplication()
{
    float elapsedTime = (float)(_timer.elapsedTime());
    _cameraManager->update(elapsedTime, true, true);
    _device->update();
    _scene->update();
    _renderHUD->update(elapsedTime);

    Ctr::InputState* inputState = _inputMgr->input().inputState();
    if (inputState->leftMouseDown() && !inputState->hasGUIFocus() && 
        inputState->getKeyState(DIK_LCONTROL))
    {
        Vector3f rotation;
        Ctr::Entity* entity = visualizedEntity();

        if (modelVisualizationProperty()->get() == Ctr::IBLApplication::ShaderBallModel)
            entity = _shaderBallEntity;
        else
            entity = _visualizedEntity;

        rotation = entity->mesh(0)->rotation();

        rotation += Ctr::Vector3f(((float)inputState->_y),((float)inputState->_x), 0);
        std::vector<Mesh*> meshes = entity->meshes();
        for (auto it = meshes.begin(); it != meshes.end(); it++)
            (*it)->rotationProperty()->set(rotation);
    }

    if (_visualizationSpaceProperty->get() != _currentVisualizationSpaceProperty->get())
    {
        updateVisualizationType();
    }

    Ctr::Camera* camera = _scene->camera();
    camera->updateViewProjection();
    camera->cacheCameraTransforms();
    _device->beginRender();
    // Perform application work.
    const Ctr::Vector4f clearColor = Ctr::Vector4f(0, 1,0,1);
    _device->bindFrameBuffer (_device->postEffectsMgr()->sceneFrameBuffer());
    _device->clearSurfaces (0, Ctr::CLEAR_TARGET | Ctr::CLEAR_ZBUFFER|Ctr::CLEAR_STENCIL, 
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
IBLApplication::loadEnvironment(const std::string& filePathName)
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


            Ctr::PixelFormat format = texture->format();
            float inputGamma = 1.0;
            // Setup default gamma. 1.0 for HDR, 2.2 for LDR
            if (format == Ctr::PF_FLOAT32_RGBA ||
                format == Ctr::PF_FLOAT16_RGBA ||
                format == Ctr::PF_FLOAT32_RGB ||
                format == Ctr::PF_FLOAT16_GR ||
                format == Ctr::PF_FLOAT32_GR ||
                format == Ctr::PF_FLOAT32_R ||
                format == Ctr::PF_FLOAT16_R)
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
IBLApplication::saveImages(const std::string& filePathName, bool gameOnly)
{
    if (Ctr::IBLProbe* probe = _scene->probes()[0])
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
IBLApplication::compute()
{
    if (Ctr::IBLProbe* probe = _scene->probes()[0])
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
IBLApplication::pause()
{
    throw std::runtime_error("Not implemented!");
}

void
IBLApplication::cancel()
{
    if (Ctr::IBLProbe* probe = _scene->probes()[0])
    {
        probe->markComputed(true);
    }
}

IntProperty*
IBLApplication::visualizationSpaceProperty()
{
    return _visualizationSpaceProperty;
}

void
IBLApplication::updateVisualizationType()
{
    _currentVisualizationSpaceProperty->set(_visualizationSpaceProperty->get());

    switch (_currentVisualizationSpaceProperty->get())
    {
        case IBLApplication::HDR:
            _visualizedEntity->mesh(0)->material()->setTechniqueName("Default");
            break;
    }
    _device->shaderMgr()->resolveShaders(_visualizedEntity);

}

Entity*
IBLApplication::visualizedEntity()
{
    return _visualizedEntity;
}

Entity*
IBLApplication::shaderBallEntity()
{
    return _shaderBallEntity;
}

Entity*
IBLApplication::sphereEntity()
{
    return _sphereEntity;
}

Entity*
IBLApplication::iblSphereEntity()
{
    return _iblSphereEntity;
}

void
IBLApplication::loadAsset(Entity*& targetEntity,
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