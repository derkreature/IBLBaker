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
#include <IblIBLRenderPass.h>
#include <IblBrdf.h>
#include <IblShaderMgr.h>
#include <IblEntity.h>
#include <IblCamera.h>
#include <IblIBLProbe.h>
#include <IblScene.h>
#include <IblMaterial.h>
#include <IblMesh.h>
#include <IblIShader.h>
#include <IblGpuVariable.h>
#include <IblPostEffectsMgr.h>
#include <IblLog.h>
#include <IblEntity.h>
#include <IblIndexedMesh.h>
#include <IblShaderMgr.h>
#include <IblIEffect.h>
#include <IblMatrixAlgo.h>

namespace Ibl
{
IBLRenderPass::IBLRenderPass(Ibl::IDevice* device) :
    Ibl::RenderPass (device),
    _convolve (nullptr),
    _cached (false),
    _material(nullptr),
    _colorConversionShader(nullptr),
    _colorConversionTechnique(nullptr),
    _colorConversionMipLevelVariable(nullptr),
    _colorConversionIsMDRVariable(nullptr),
    _colorConversionGammaVariable(nullptr),
    _colorConversionLDRExposureVariable(nullptr),
    _colorConversionMDRScaleVariable(nullptr),
    _colorConversionGamma(2.2f),
    _colorConversionLDRExposure(1.0f),
    _colorConversionMDRScale(6)
{
    _passName = "ibl";

    _paraboloidTransformCache.reset(new CameraTransformCache());
    _environmentTransformCache.reset(new CameraTransformCache());

    // Load the MDR conversion shader.
    // Load the importance sampling shader and variables.
    if (!_deviceInterface->shaderMgr()->addShader("IblColorConvertEnvironment.fx", _colorConversionShader, true))
    {
        LOG("ERROR: Could not add the environment color conversion shader.");
        throw (std::exception("No color conversion shader available for probes"));
    }
    else
    {
        _colorConversionShader->getTechniqueByName(std::string("Default"), _colorConversionTechnique);
        _colorConversionShader->getParameterByName("CurrentMipLevel", _colorConversionMipLevelVariable);
        _colorConversionShader->getParameterByName("IsMDR", _colorConversionIsMDRVariable);
        _colorConversionShader->getParameterByName("Gamma", _colorConversionGammaVariable);
        _colorConversionShader->getParameterByName("LDRExposure", _colorConversionLDRExposureVariable);
        _colorConversionShader->getParameterByName("MDRScale", _colorConversionMDRScaleVariable);
    }



    cache();
}

IBLRenderPass::~IBLRenderPass()
{
    safedelete(_sphereEntity);
    safedelete(_material);
}

bool
IBLRenderPass::free ()
{
    return true;
}

bool
IBLRenderPass::cache()
{
    if (!_cached)
    {
        // TODO: Load sphere projection resource.
        if (!loadMesh())
        {
            LOG ("Failed to load sphere projection mesh for IBLRenderPass");
            return false;
        }
    }
    return true;
}

bool
IBLRenderPass::loadMesh()
{
    // Static scene load does not resolve materials.
    // This sphere is for reprojection for dual projection to cube map.
    if (_sphereEntity = Scene::load(_deviceInterface, "data/meshes/Sphere/sphere.obj"))
    {
        _sphereMesh = _sphereEntity->meshes()[0];
        _sphereMesh->scaleProperty()->set(Ibl::Vector3f(5,5,5));
        _material = new Material(_deviceInterface);
        _material->textureGammaProperty()->set(1.0f);
        _sphereMesh->setMaterial(_material);
    }
    else
    {
        LOG ("Could not load sphere");
    }

    return true;
}

void
IBLRenderPass::refineDiffuse(Ibl::Scene* scene,
                             const Ibl::IBLProbe* probe)
{
    // TODO: Optimize.
	Ibl::Camera* camera = scene->camera();

	float projNear = camera->zNear();
	float projFar = camera->zFar();
    Ibl::Matrix44f proj;
    Ibl::projectionPerspectiveMatrixLH (Ibl::BB_PI * 0.5f,
                                       1.0, 
                                       projNear, 
                                       projFar,
                                       &proj);

	Ibl::Vector3f origin(0, 0, 0);
	// Setup view matrix
	_environmentTransformCache->set(probe->basis(), proj, probe->basis(), origin, projNear, projFar, -1);
	camera->setCameraTransformCache(_environmentTransformCache);

    // Setup our source.
    size_t mipLevels = probe->diffuseCubeMap()->resource()->mipLevels();
    const Ibl::ITexture* sourceTexture = probe->environmentCubeMap();

    float roughness = 0;
    float roughnessDelta = 1.0f / (float)(mipLevels);
    float samplesOffset = (float)(probe->sampleOffset());
    float samplesPerFrame = (float)(probe->samplesPerFrame());
    float sampleCount = (float)(probe->sampleCount());

    roughness = 1.0;

    const Ibl::Brdf* brdf = scene->activeBrdf();
    const Ibl::IShader* importanceSamplingShaderDiffuse = brdf->diffuseImportanceSamplingShader();
    // Convolve for diffuse.
    {
        float currentMip = 0;

        const Ibl::ISurface* targetSurface = probe->diffuseCubeMap()->surface();
        Ibl::FrameBuffer framebuffer(targetSurface, nullptr);

        Ibl::Viewport mipViewport (0,0, probe->diffuseResolution(), probe->diffuseResolution(), 0, 1);
        _deviceInterface->bindFrameBuffer(framebuffer);
        _deviceInterface->setViewport(&mipViewport);
        _deviceInterface->clearSurfaces (0, Ibl::CLEAR_TARGET, 0, 0, 0, 1);

        const Ibl::GpuTechnique*     importanceSamplingDiffuseTechnique = nullptr;
        const Ibl::GpuVariable*      convolutionSrcDiffuseVariable = nullptr;
        const Ibl::GpuVariable*      convolutionMipDiffuseVariable = nullptr;
        const Ibl::GpuVariable*      convolutionRoughnessDiffuseVariable = nullptr;
        const Ibl::GpuVariable*      convolutionSamplesOffsetDiffuseVariable = nullptr;
        const Ibl::GpuVariable*      convolutionViewsDiffuseVariable = nullptr;
        const Ibl::GpuVariable*      convolutionSampleCountDiffuseVariable = nullptr;
        const Ibl::GpuVariable*      convolutionMaxSamplesDiffuseVariable = nullptr;
        const Ibl::GpuVariable*      convolutionSrcLastResultDiffuseVariable = nullptr;

        importanceSamplingShaderDiffuse->getTechniqueByName(std::string("basic"), importanceSamplingDiffuseTechnique);
        importanceSamplingShaderDiffuse->getParameterByName("ConvolutionSrc", convolutionSrcDiffuseVariable);
        importanceSamplingShaderDiffuse->getParameterByName("LastResult", convolutionSrcLastResultDiffuseVariable);
        importanceSamplingShaderDiffuse->getParameterByName("ConvolutionMip", convolutionMipDiffuseVariable);
        importanceSamplingShaderDiffuse->getParameterByName("ConvolutionRoughness", convolutionRoughnessDiffuseVariable);
        importanceSamplingShaderDiffuse->getParameterByName("ConvolutionSamplesOffset", convolutionSamplesOffsetDiffuseVariable);
        importanceSamplingShaderDiffuse->getParameterByName("ConvolutionSampleCount", convolutionSampleCountDiffuseVariable);
        importanceSamplingShaderDiffuse->getParameterByName("ConvolutionMaxSamples", convolutionMaxSamplesDiffuseVariable);

        // Set parameters
        convolutionSrcDiffuseVariable->setTexture(sourceTexture);
        convolutionSrcLastResultDiffuseVariable->setTexture(probe->lastDiffuseCubeMap());
        
        convolutionMipDiffuseVariable->set ((const float*)&currentMip, sizeof (float));
        convolutionRoughnessDiffuseVariable->set((const float*)&roughness, sizeof (float));
        convolutionSamplesOffsetDiffuseVariable->set((const float*)&samplesOffset, sizeof (float));        
        convolutionSampleCountDiffuseVariable->set(&samplesPerFrame , sizeof(float));
        convolutionMaxSamplesDiffuseVariable->set(&sampleCount, sizeof(float));

        importanceSamplingShaderDiffuse->renderMesh (Ibl::RenderRequest(importanceSamplingDiffuseTechnique, scene, camera, _sphereMesh));
    }
}

void
IBLRenderPass::refineSpecular(Ibl::Scene* scene,
                              const Ibl::IBLProbe* probe)
{
	Ibl::Camera* camera = scene->camera();

    float projNear = camera->zNear();
    float projFar = camera->zFar();
    Ibl::Matrix44f proj;
    Ibl::projectionPerspectiveMatrixLH (Ibl::BB_PI * 0.5f,
                                                        1.0, 
                                                        projNear, 
                                                        projFar,
                                                        &proj);

	Ibl::Vector3f origin(0, 0, 0);
    // Setup view matrix
	_environmentTransformCache->set(probe->basis(), proj, probe->basis(), origin, projNear, projFar, -1);
    camera->setCameraTransformCache(_environmentTransformCache);

    // Setup our source.
    size_t mipLevels = probe->specularCubeMap()->resource()->mipLevels() - probe->mipDrop();
    const Ibl::ITexture* sourceTexture = probe->environmentCubeMap();

    float roughness = 0;
    float roughnessDelta = 1.0f / (float)(mipLevels-1);
    float samplesOffset = (float)(probe->sampleOffset());
    float samplesPerFrame = (float)(probe->samplesPerFrame());
    float sampleCount = (float)(probe->sampleCount());

    const Ibl::Brdf* brdf = scene->activeBrdf();
    const Ibl::IShader* importanceSamplingShaderSpecular = brdf->specularImportanceSamplingShader();

    // Convolve specular.
    uint32_t mipSize = probe->specularCubeMap()->resource()->width();

    for (uint32_t mipId = 0; mipId < mipLevels; mipId++)
    {
        float currentMip = (float)(mipId);

        const Ibl::ISurface* targetSurface = probe->specularCubeMap()->surface(-1, mipId);
        Ibl::FrameBuffer framebuffer(targetSurface, nullptr);

        Ibl::Viewport mipViewport (0.0f,0.0f, (float)(mipSize), (float)(mipSize), 0.0f, 1.0f);
        _deviceInterface->bindFrameBuffer(framebuffer);
        _deviceInterface->setViewport(&mipViewport);


        const Ibl::GpuTechnique*     importanceSamplingSpecularTechnique = nullptr;
        const Ibl::GpuVariable*      convolutionSrcSpecularVariable = nullptr;
        const Ibl::GpuVariable*      convolutionMipSpecularVariable = nullptr;
        const Ibl::GpuVariable*      convolutionRoughnessSpecularVariable = nullptr;
        const Ibl::GpuVariable*      convolutionSamplesOffsetSpecularVariable = nullptr;
        const Ibl::GpuVariable*      convolutionSampleCountSpecularVariable = nullptr;
        const Ibl::GpuVariable*      convolutionMaxSamplesSpecularVariable = nullptr;
        const Ibl::GpuVariable*      convolutionSrcLastResultSpecularVariable = nullptr;

        importanceSamplingShaderSpecular->getTechniqueByName(std::string("basic"), importanceSamplingSpecularTechnique);
        importanceSamplingShaderSpecular->getParameterByName("ConvolutionSrc",     convolutionSrcSpecularVariable);
        importanceSamplingShaderSpecular->getParameterByName("LastResult",         convolutionSrcLastResultSpecularVariable);
        importanceSamplingShaderSpecular->getParameterByName("ConvolutionMip",     convolutionMipSpecularVariable);
        importanceSamplingShaderSpecular->getParameterByName("ConvolutionRoughness", convolutionRoughnessSpecularVariable);
        importanceSamplingShaderSpecular->getParameterByName("ConvolutionSamplesOffset", convolutionSamplesOffsetSpecularVariable);
        importanceSamplingShaderSpecular->getParameterByName("ConvolutionSampleCount", convolutionSampleCountSpecularVariable);
        importanceSamplingShaderSpecular->getParameterByName("ConvolutionMaxSamples", convolutionMaxSamplesSpecularVariable);

        // Set parameters
        convolutionSrcSpecularVariable->setTexture(sourceTexture);
        convolutionSrcLastResultSpecularVariable->setTexture(probe->lastSpecularCubeMap());
        convolutionMipSpecularVariable->set ((const float*)&currentMip, sizeof (float));
        convolutionRoughnessSpecularVariable->set((const float*)&roughness, sizeof (float));
        convolutionSamplesOffsetSpecularVariable->set((const float*)&samplesOffset, sizeof (float));
        convolutionSampleCountSpecularVariable->set(&samplesPerFrame , sizeof(float));
        convolutionMaxSamplesSpecularVariable->set(&sampleCount, sizeof(float));

        // Render the paraboloid out.
        importanceSamplingShaderSpecular->renderMesh (Ibl::RenderRequest(importanceSamplingSpecularTechnique, scene, camera, _sphereMesh));
        roughness += roughnessDelta;

        mipSize = mipSize >> 1;
    }
}

void
IBLRenderPass::render (Ibl::Scene* scene)
{
    Ibl::Camera* camera           = scene->camera();

    _deviceInterface->enableDepthWrite();
    _deviceInterface->enableZTest();

    // For each ibl set on the scene, render and process the dynamic ibl.

    _deviceInterface->disableAlphaBlending ();
    _deviceInterface->setCullMode (Ibl::CullNone);
    
    Ibl::Viewport oldview;
    _deviceInterface->getViewport(&oldview);
    
    // Render all meshes that have an environment marker.
    const std::vector<Ibl::IBLProbe*>& probes = scene->probes();
    
    const Ibl::Brdf* brdf = scene->activeBrdf();
    if (!brdf)
    {
        THROW("Cannot find brdf to create IBL " << __FILE__ << " " << __LINE__);
    }

    // Detect uncache condition based on importance sampling shaders.
    bool forceUncache = false;
    if (_specularHash != brdf->specularImportanceSamplingShader()->hash() ||
        _diffuseHash != brdf->diffuseImportanceSamplingShader()->hash())
    {
        _specularHash = brdf->specularImportanceSamplingShader()->hash();
        _diffuseHash = brdf->diffuseImportanceSamplingShader()->hash();
        forceUncache = true;
    }

    Ibl::CameraTransformCachePtr cachedTransforms = scene->camera()->cameraTransformCache();
    for (auto it = probes.begin(); it != probes.end(); it++)
    {
        // Todo, cull probe by location and range.
        // if (probe->hasInfluence(scene->activeCamera()))
        IBLProbe * probe = *it;
        if (forceUncache)
            probe->uncache();

        bool cached = probe->isCached();
        if (cached)
        {
            continue;
        }
        else if (probe->sampleOffset() == 0)
        {
            // If sample offset is 0, we need to create the environment
            // map and perform a first set of samples.
            _deviceInterface->disableZTest();
            _deviceInterface->disableDepthWrite();
            _deviceInterface->disableStencilTest();
            _deviceInterface->setCullMode (Ibl::CullNone);

             {
				// The ibl probe could also have a znear and zfar.
				// In this example it is more expedient just to use the camera znear - zfar.
				float projNear = camera->zNear();
				float projFar = camera->zFar();
                Ibl::Matrix44f proj;
                Ibl::projectionPerspectiveMatrixLH (Ibl::BB_PI * 0.5f,
                                                    1.0, 
                                                    projNear, 
                                                    projFar,
                                                    &proj);
    
                // Setup view matrix for the environment source render.
                _environmentTransformCache->set(probe->basis(), proj, probe->basis(), probe->center(), projNear, projFar, -1);
    
                // Setup camera cache.
                camera->setCameraTransformCache(_environmentTransformCache);
    
                // Set framebuffer to cubemap.
                // Render to environment top level mip (highest resolution).
                size_t mipLevels = probe->environmentCubeMap()->resource()->mipLevels();

                Ibl::Vector2f mipSize = Ibl::Vector2f(float(probe->environmentCubeMap()->resource()->width()), 
                                                    float(probe->environmentCubeMap()->resource()->height()));

                for (size_t mipId = 0; mipId < mipLevels; mipId++)
                {
                    Ibl::Viewport mipViewport (0.0f, 0.0f, (float)(mipSize.x), (float)(mipSize.y), 0.0f, 1.0f);

                    // Render to top level mip for both cubemaps. A better strategy would be to blit after the first render...
                    Ibl::FrameBuffer framebuffer(probe->environmentCubeMap()->surface(-1, (int32_t)(mipId)), nullptr);
                    _deviceInterface->bindFrameBuffer(framebuffer);
                    _deviceInterface->setViewport(&mipViewport);
                    _deviceInterface->clearSurfaces (0, Ibl::CLEAR_TARGET, 0, 0, 0, 1);
    
                    // Render the scene to cubemap (single pass).
                    //renderMeshes (_passName, scene);
                    const std::vector<Ibl::Mesh*>& meshes = scene->meshesForPass(_passName);
                    for (auto it = meshes.begin(); it != meshes.end(); it++)
                    {
                        const Ibl::Mesh* mesh = (*it);
                        const Ibl::Material* material = mesh->material();
                        const Ibl::IShader* shader = material->shader();
                        const Ibl::GpuTechnique* technique = material->technique();
    
                        RenderRequest renderRequest (technique, scene, scene->camera(), mesh);
                        shader->renderMesh(renderRequest);
                    }

                    mipSize.x /= 2.0f;
                    mipSize.y /= 2.0f;
                }

                // Generate mip maps post rendering.
                probe->environmentCubeMap()->generateMipMaps();    
                refineSpecular(scene, probe);
                refineDiffuse(scene, probe);

                colorConvert(scene, probe);
                // Update the sample count
                probe->updateSamples();
            }
            _deviceInterface->enableZTest();
            _deviceInterface->enableDepthWrite();
            _deviceInterface->setCullMode (Ibl::CullNone);
        }
        else
        {
            // Refine samples.
            // If sample offset is 0, we need to create the environment
            // map and perform a first set of samples.
            _deviceInterface->disableZTest();
            _deviceInterface->disableDepthWrite();
            _deviceInterface->disableStencilTest();
            _deviceInterface->setCullMode (Ibl::CullNone);
    
            float projNear = camera->zNear();
            float projFar = camera->zFar();
            Ibl::Matrix44f proj;
            Ibl::projectionPerspectiveMatrixLH (Ibl::BB_PI * 0.5f,
                                                1.0, 
                                                projNear, 
                                                projFar,
                                                &proj);
    
    
            // Setup view matrix
            _environmentTransformCache->set(probe->basis(), proj, probe->basis(), probe->center(), projNear, projFar, -1);
    
            // Setup camera cache.
            scene->camera()->setCameraTransformCache(_environmentTransformCache);
    
            refineSpecular(scene, probe);
            refineDiffuse(scene, probe);

            // Update the sample count
            probe->updateSamples();
            colorConvert(scene, probe);

            _deviceInterface->enableZTest();
            _deviceInterface->enableDepthWrite();
            _deviceInterface->setCullMode (Ibl::CullNone);
        }
    }
    
    // Restore original camera transforms.
    scene->camera()->setCameraTransformCache(cachedTransforms);
    
    _deviceInterface->disableAlphaBlending ();
    _deviceInterface->bindFrameBuffer(_deviceInterface->postEffectsMgr()->sceneFrameBuffer());
    _deviceInterface->setViewport(&oldview);

    _deviceInterface->setCullMode (Ibl::CCW);
}

void
IBLRenderPass::colorConvert(bool applyMDR,
                            bool useMips,
                            Ibl::Scene* scene,
                            const Ibl::ITexture* dst,
                            const Ibl::ITexture* src,
                            const Ibl::IBLProbe* probe)
{
    // TODO: Optimize.
    float projNear = 0.5;
    float projFar = 1000.0f;
    Ibl::Matrix44f proj;
    Ibl::projectionPerspectiveMatrixLH(Ibl::BB_PI * 0.5f,
        1.0,
        projNear,
        projFar,
        &proj);

    // Setup view matrix
    _environmentTransformCache->set(probe->basis(), proj, probe->basis(), probe->center(), projNear, projFar, -1);

    // Setup camera cache.
    scene->camera()->setCameraTransformCache(_environmentTransformCache);

    // Setup our source.
    size_t mipLevels = Ibl::minValue(src->resource()->mipLevels(), dst->resource()->mipLevels());
    const Ibl::ITexture* sourceTexture = src;

    float isMDR = applyMDR ? 1.0f : 0.0f;
    //                                             bool useMips,

    _sphereMesh->material()->setAlbedoMap(src);
    Ibl::Vector2i mipSize = Ibl::Vector2i(dst->width(), dst->height());

    // Convolve specular.
    for (size_t mipId = 0; mipId < mipLevels; mipId++)
    {
        float currentMip = (float)(mipId);
        // Render to target mip.
        const Ibl::ISurface* targetSurface = dst->surface(-1, (int32_t)(mipId));
        Ibl::FrameBuffer framebuffer(targetSurface, nullptr);

        Ibl::Viewport mipViewport(0.0f, 0.0f, (float)(mipSize.x), (float)(mipSize.y), 0.0f, 1.0f);
        _deviceInterface->bindFrameBuffer(framebuffer);
        _deviceInterface->setViewport(&mipViewport);

        _colorConversionMipLevelVariable->set((const float*)&currentMip, sizeof(float));
        _colorConversionIsMDRVariable->set((const float*)&isMDR, sizeof(float));
        _colorConversionMDRScaleVariable->set((const float*)&_colorConversionMDRScale, sizeof(float));
        _colorConversionGammaVariable->set((const float*)&_colorConversionGamma, sizeof(float));
        _colorConversionLDRExposureVariable->set((const float*)&_colorConversionLDRExposure, sizeof(float));
        _colorConversionMDRScaleVariable->set((const float*)&_colorConversionMDRScale, sizeof(float));

        // Render the paraboloid out.
        _colorConversionShader->renderMesh(Ibl::RenderRequest(_colorConversionTechnique, scene, scene->camera(), _sphereMesh));
        mipSize.x /= 2;
        mipSize.y /= 2;
    }
}

void
IBLRenderPass::colorConvert(Ibl::Scene* scene, Ibl::IBLProbe* probe)
{
    {
        // Convert specular src to MDR, save. (small memory optimization).
        colorConvert(true, true, scene, probe->environmentCubeMapMDR(), probe->environmentCubeMap(), probe);

        // Convert specular to MDR
        colorConvert(true, true, scene, probe->specularCubeMapMDR(), probe->specularCubeMap(), probe);

        // Convert diffuse to MDR
        colorConvert(true, false, scene, probe->diffuseCubeMapMDR(), probe->diffuseCubeMap(), probe);
    }
}

}