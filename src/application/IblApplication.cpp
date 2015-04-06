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
#include <IblFontMgr.h>
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
#include <IblImageWidget.h>
#include <IblBrdf.h>
#include <Iblimgui.h>
#include <strstream>

namespace Ibl
{
namespace
{

TwEnumVal IblEnum[] =
{
    { Application::HDR, "HDR" },
    { Application::MDR, "MDR" },
    { Application::LDR, "LDR" }
};

static const EnumTweakType IBLModeType(&IblEnum[0], 3, "IBLMode");

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
    _visualizationSpaceProperty(new IntProperty(this, "Visualization Type", new TweakFlags(&IBLModeType, "Renderer"))),
    _currentVisualizationSpaceProperty(new IntProperty(this, "Visualization Type", new TweakFlags(&IBLModeType, "Renderer"))),
    _hdrFormatProperty(new PixelFormatProperty(this, "Visualization Type", new TweakFlags(&IBLModeType, "Renderer"))),
    _probeResolutionProperty(new IntProperty(this, "Visualization Type", new TweakFlags(&IBLModeType, "Renderer"))),
    _workflow(RoughnessMetal),
    _titles(nullptr),
    _defaultAsset("data\\meshes\\pistol\\pistol.obj"),
    _runTitles(false)
{
    _visualizationSpaceProperty->set(Ibl::Application::HDR);
    _currentVisualizationSpaceProperty->set(-1);
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

        loadAsset(_defaultAsset);

        _sphereEntity = _scene->load("data/meshes/sphere/sphere.obj", 
                                     "data/meshes/sphere/sphere.material");
        _sphereEntity->mesh(0)->scaleProperty()->set(Ibl::Vector3f(10,10,10));

        _iblSphereEntity = _scene->load("data/meshes/sphere/sphere.obj", 
                                        "data/meshes/sphere/iblsphere.material");

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

        // Setup titles

        // Credits are just a set of UI billboards that are written to the screen with a background.
        // Start time, end time, blendIn start, blendIn end, blendOutStart, blendIn end.
        // A transition post effect is enabled and is removed when the credits are completed.
        if (_runTitles)
        {
            // Only run titles once for a user.
            _runTitles = false;
            _titles = new Ibl::Titles(_device);
            _titles->initialize("IblTitles.fx");
            _titles->setTitleLength(5.0f);

            _device->postEffectsMgr()->addPostEffect(_titles);

            Ibl::Title * background = new Ibl::Title(_device);
            background->create(std::string("data/textures/BbTitles/Background.dds"),
                Ibl::Region2f(Ibl::Vector2f(-1, -1), Ibl::Vector2f(1, 1)),
                Ibl::Vector4f(0, 0.5f, 2.5f, 3.0f));
            _titles->addTitle(background);

             Ibl::Title * kreature = new Ibl::Title(_device);
            kreature->create(std::string("data/textures/BbTitles/Kreature.dds"),
                Ibl::Region2f(Ibl::Vector2f(-1, -1), Ibl::Vector2f(1, 1)),
                Ibl::Vector4f(0, 0.5f, 2.5f, 2.8f));
            _titles->addTitle(kreature);
 
             Ibl::Title * munky = new Ibl::Title(_device);
            munky->create(std::string("data/textures/BbTitles/Title.dds"),
                Ibl::Region2f(Ibl::Vector2f(-1, -1), Ibl::Vector2f(1, 1)),
                Ibl::Vector4f(2.8f, 3.0f, 4.8f, 5.0f));
            _titles->addTitle(munky);
            _renderHUD->setLogoVisible(false);
        }
        else
        {
            _renderHUD->setLogoVisible(true);
            _renderHUD->logo()->setBlendIn(6.0f);
            _renderHUD->showApplicationUI();
        }
    }
    else
    {
        delete device;
        THROW ("Failed to create render device");
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
                std::string workflow(xpathValue);

                if (workflow == std::string("RoughnessMetal"))
                {
                    _workflow = Ibl::RoughnessMetal;
                }
                else if (workflow == std::string("GlossMetal"))
                {
                    _workflow = Ibl::GlossMetal;
                }
                else if (workflow == std::string("RoughnessInverseMetal"))
                {
                    _workflow = Ibl::RoughnessInverseMetal;
                }
                else if (workflow == std::string("GlossInverseMetal"))
                {
                    _workflow = Ibl::GlossInverseMetal;
                }
                else
                {
                    LOG("Could not find a valid preference for specular workflow, using RoughnessMetal.")
                }
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
        switch (_workflow)
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
        Vector3f rotation = _visualizedEntity->mesh(0)->rotation();

        // ((float)-inputState->_y)
        rotation += Ibl::Vector3f(((float)inputState->_y),((float)inputState->_x), 0);
        std::vector<Mesh*> meshes = _visualizedEntity->meshes();
        for (auto it = meshes.begin(); it != meshes.end(); it++)
            (*it)->rotationProperty()->set(rotation);
    }

    // Title management thingy.
    if (_titles)
    {
        _titles->update(elapsedTime);
        if (_titles->isFinished())
        {
            _device->postEffectsMgr()->removePostEffect(_titles);
            _titles = nullptr;
            _renderHUD->setLogoVisible(true);
            _renderHUD->logo()->setBlendIn(6.0f);
            _renderHUD->showApplicationUI();
        }
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


    if (_titles)
        _titles->render();
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
Application::loadAsset(const std::string& assetPathName, 
                       const std::string& materialPathName)
{
    SpecularWorkflow workflow = _workflow;
    float specularIntensity = 1.0f;
    float roughnessScale = 1.0f;

    if (_visualizedEntity)
    {
        if (_visualizedEntity->meshes().size() > 0)
        {
            workflow = (SpecularWorkflow)_visualizedEntity->meshes()[0]->material()->specularWorkflow();
            specularIntensity = _visualizedEntity->meshes()[0]->material()->specularIntensity();
            roughnessScale = _visualizedEntity->meshes()[0]->material()->roughnessScale();
        }
        _scene->destroy(_visualizedEntity);
    }

    _visualizedEntity = _scene->load(assetPathName, materialPathName);
    const std::vector<Mesh*>& meshes = _visualizedEntity->meshes();
    for (auto meshIt = meshes.begin(); meshIt != meshes.end(); meshIt++)
    {
        // Copy over the last settings.
        (*meshIt)->material()->specularWorkflowProperty()->set(workflow);
        (*meshIt)->material()->specularIntensityProperty()->set(specularIntensity);
        (*meshIt)->material()->roughnessScaleProperty()->set(roughnessScale);
    }
}
}