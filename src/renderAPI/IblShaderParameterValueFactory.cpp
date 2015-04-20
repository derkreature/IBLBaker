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


#include <IblShaderParameterValueFactory.h>
#include <IblIndexedMesh.h>
#include <IblShaderParameterValue.h>
#include <IblTextureMgr.h> 
#include <IblGpuTechnique.h>
#include <IblGpuVariable.h>
#include <IblCamera.h>
#include <IblMesh.h>
#include <IblMatrixAlgo.h>
#include <IblIEffect.h>
#include <IblPostEffect.h>
#include <IblVector2.h>
#include <IblProjectionProperty.h>
#include <IblIGpuBuffer.h>
#include <IblPostEffectsMgr.h>
#include <IblMaterial.h>
#include <IblIBLProbe.h>
#include <IblScene.h>
#include <IblBrdf.h>

namespace Ibl
{
class IShaderParameterFactory 
{
  public:
    virtual bool                  supports (GpuVariable* variable) const = 0;
    virtual ShaderParameterValue* make (GpuVariable* variable,
                                        Ibl::IEffect* effect) = 0;
};

class UnknownValue :  public ShaderParameterValue
{
  public:
    UnknownValue (const GpuVariable* variable, Ibl::IEffect* effect) : ShaderParameterValue (variable, effect) 
    {
        setParameterType (UnknownParameter);    
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
    }
};

class WorldMatrixValue :  public ShaderParameterValue
{
  public:
    WorldMatrixValue(const GpuVariable* variable, Ibl::IEffect*effect): 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (WorldMatrix);
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        Ibl::Matrix44f world = request.mesh->worldTransform();        
        _variable->setMatrix((const float*)&world);
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "WORLD")==0;
    }
};

class MeshGroupIdValue :  public ShaderParameterValue
{
  public:
    MeshGroupIdValue(const GpuVariable* variable, Ibl::IEffect*effect): 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (MeshGroupId);
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        uint32_t groupId = request.mesh->groupId();
        _variable->set((const uint32_t*)&groupId, sizeof(uint32_t));
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "MESHGROUPID")==0;
    }
};

class WorldViewProjectionValue :  public ShaderParameterValue
{
  public:
    WorldViewProjectionValue(const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (WorldViewProjection);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        Ibl::Matrix44f world = request.mesh->worldTransform();

        const Ibl::CameraTransformCachePtr ctc = request.camera->cameraTransformCache();
        const Ibl::Matrix44f& viewProj = ctc->viewProjMatrix();
        world = world * viewProj;

        _variable->setMatrix((const float*)&world);
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "WORLDVIEWPROJECTION")==0;
    }
};

class UserAlbedoValue : public ShaderParameterValue
{
public:
    UserAlbedoValue(const GpuVariable* variable, Ibl::IEffect*effect) :
        ShaderParameterValue(variable, effect)
    {
        setParameterType(UserAlbedo);
    }

    virtual void setParam(const Ibl::RenderRequest& request) const
    {
        if (const Material* material = request.material)
        {
            const Vector4f& userAlbedo = material->userAlbedo();
            _variable->setVector(&userAlbedo.x);
        }
    }

    static bool supports(GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "USERALBEDO") == 0;
    }
};

class UserRMValue : public ShaderParameterValue
{
public:
    UserRMValue(const GpuVariable* variable, Ibl::IEffect*effect) :
        ShaderParameterValue(variable, effect)
    {
        setParameterType(UserRM);
    }

    virtual void setParam(const Ibl::RenderRequest& request) const
    {
        if (const Material* material = request.material)
        {
            const Vector4f& userRM = material->userRM();
            _variable->setVector(&userRM.x);
        }
    }

    static bool supports(GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "USERRM") == 0;
    }
};


class IblOcclValue : public ShaderParameterValue
{
public:
    IblOcclValue(const GpuVariable* variable, Ibl::IEffect*effect) :
        ShaderParameterValue(variable, effect)
    {
        setParameterType(IblOccl);
    }

    virtual void setParam(const Ibl::RenderRequest& request) const
    {
        if (const Material* material = request.material)
        {
            const Vector4f& iblOccl = material->iblOccl();
            _variable->setVector(&iblOccl.x);
        }
    }

    static bool supports(GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "IBLOCCL") == 0;
    }
};

class DetailMapValue : public ShaderParameterValue
{
public:
    DetailMapValue(const GpuVariable* variable, Ibl::IEffect*effect) :
        ShaderParameterValue(variable, effect)
    {
        setParameterType(DetailMap);
    }

    virtual void setParam(const Ibl::RenderRequest& request) const
    {
        if (const Material* material = request.material)
        {
            _variable->setTexture(material->detailMap());
        }
    }

    static bool supports(GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "DETAILMAP") == 0;
    }

};

class MaterialDiffuseValue :  public ShaderParameterValue
{
  public:
    MaterialDiffuseValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (MaterialDiffuse);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        if (const Material* material = request.material)
        {           
            _variable->setVector((const float*)&material->albedoColor());
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "MATERIALDIFFUSE")==0;
    }
};

class SpecularIntensityValue : public ShaderParameterValue
{
  public:
    SpecularIntensityValue(const GpuVariable* variable, Ibl::IEffect*effect) :
        ShaderParameterValue(variable, effect)
    {
        setParameterType(SpecularIntensity);
    }

    virtual void setParam(const Ibl::RenderRequest& request) const
    {
        if (const Material* material = request.material)
        {
            float specularIntensity = material->specularIntensity();
            _variable->set(&specularIntensity, sizeof(float));
        }
    }

    static bool supports(GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "SPECULARINTENSITY") == 0;
    }
};

class RoughnessScaleValue : public ShaderParameterValue
{
  public:
    RoughnessScaleValue(const GpuVariable* variable, Ibl::IEffect*effect) :
        ShaderParameterValue(variable, effect)
    {
        setParameterType(RoughnessScale);
    }

    virtual void setParam(const Ibl::RenderRequest& request) const
    {
        if (const Material* material = request.material)
        {
            float roughnessScale = material->roughnessScale();
            _variable->set(&roughnessScale, sizeof(float));
        }
    }

    static bool supports(GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "ROUGHNESSSCALE") == 0;
    }
};

class SpecularWorkflowValue : public ShaderParameterValue
{
  public:
      SpecularWorkflowValue(const GpuVariable* variable, Ibl::IEffect*effect) :
        ShaderParameterValue(variable, effect)
    {
        setParameterType(SpecularWorkflowType);
    }

    virtual void setParam(const Ibl::RenderRequest& request) const
    {
        if (const Material* material = request.material)
        {
            Ibl::Vector4f specularModifiers;
            switch (material->specularWorkflow())
            {
                case RoughnessMetal:
                {
                    specularModifiers.x = 0.0;
                    specularModifiers.y = 0.0;
                    break;
                }
                case GlossMetal:
                {
                    specularModifiers.x = 1.0;
                    specularModifiers.y = 0.0;
                    break;
                }
                case RoughnessInverseMetal:
                {
                    specularModifiers.x = 0.0;
                    specularModifiers.y = 1.0;
                    break;
                }
                case GlossInverseMetal:
                {
                    specularModifiers.x  = 1.0;
                    specularModifiers.y = 1.0;
                    break;
                }
            }
            _variable->setVector(&specularModifiers.x);
        }
    }

    static bool supports(GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "SPECULARWORKFLOW") == 0;
    }
};


class ViewProjectionValue :  public ShaderParameterValue
{
  public:
    ViewProjectionValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (ViewProjection);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        const Ibl::CameraTransformCachePtr ctc = request.camera->cameraTransformCache();
        Ibl::Matrix44f viewProj = ctc->viewProjMatrix();
        _variable->setMatrix((const float*)&viewProj);
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "VIEWPROJECTION")==0;
    }
};

class ProjectionValue :  public ShaderParameterValue
{
  public:
     ProjectionValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (Projection);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        const Ibl::CameraTransformCachePtr ctc = request.camera->cameraTransformCache();
        const Ibl::Matrix44f& projection = ctc->projMatrix();

        _variable->setMatrix((const float*)&projection);
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "PROJECTION")==0;
    }
};

class ScreenSizeValue :  public ShaderParameterValue
{
  public:
    ScreenSizeValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope (PerTechnique);
        setParameterType (ScreenSize);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        if (const PostEffect* rt = dynamic_cast <const PostEffect*>(request.mesh))
        {
            Ibl::Vector2f screenSize = Ibl::Vector2f ((float)rt->postEffectBounds().size().x, 
                                                      (float)rt->postEffectBounds().size().y);
            _variable->set ( (const float*)&screenSize, 
                                sizeof(Ibl::Vector2f));        
        }
        else
        {
            LOG ("Error, Screen size value supports only valid when using post effects");
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "SCREENSIZE")==0;
    }
};


class ViewValue :  public ShaderParameterValue
{
  public:
    ViewValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope (PerTechnique);
        setParameterType (View);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        const Ibl::CameraTransformCachePtr ctc = request.camera->cameraTransformCache();
        const Ibl::Matrix44f& matrix = ctc->viewMatrix();
        _variable->setMatrix ((const float*)&matrix);
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "VIEW")==0;
    }
};

class ViewRightValue :  public ShaderParameterValue
{
  public:
    ViewRightValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope (PerTechnique);
        setParameterType (ViewRight);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        const Ibl::CameraTransformCachePtr ctc = request.camera->cameraTransformCache();
        const Ibl::Matrix44f& matView = ctc->viewMatrix();
        Ibl::Vector3f right(matView._11, matView._21, matView._31);

        Ibl::Vector4f right4(right.x, right.y, right.z, 0.0f);

        _variable->setVector((const float*)&right4);
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "VIEWRIGHT")==0;
    }
};

class ViewUpValue :  public ShaderParameterValue
{
  public:
    ViewUpValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope (PerTechnique);
        setParameterType (ViewUp);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        const Ibl::CameraTransformCachePtr ctc = request.camera->cameraTransformCache();
        const Ibl::Matrix44f& matView = ctc->viewMatrix();

        Ibl::Vector3f up(matView._12, matView._22, matView._32);


        Ibl::Vector4f up4(up.x, up.y, up.z, 0.0f);

        _variable->setVector((const float*)&up4);
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "VIEWUP")==0;
    }
};

class ViewLookAtValue :  public ShaderParameterValue
{
  public:
    ViewLookAtValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope (PerTechnique);
        setParameterType (ViewLookAt);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        const Ibl::Vector3f& forward = request.camera->forward();
        Ibl::Vector3f normalized = forward.normalized();
        _variable->setVector((const float*)&normalized);
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "VIEWLOOKAT")==0;
    }
};

class WorldViewValue :  public ShaderParameterValue
{
  public:
    WorldViewValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (WorldView);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        const Ibl::CameraTransformCachePtr ctc = request.camera->cameraTransformCache();
        const Ibl::Matrix44f& matView = ctc->viewMatrix();

        Ibl::Matrix44f matrix = request.mesh->worldTransform() * request.camera->viewMatrix();                    
        _variable->setMatrix((const float*)&matrix);
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "WORLDVIEW")==0;
    }
};

class SpecularRMCMapValue :  public ShaderParameterValue
{
  public:
    SpecularRMCMapValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (SpecularRMCMap);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        const ITexture* texture  = 0;
        if (const Material* material = request.material)
        {
            if (texture = material->specularRMCMap())
            {
                _variable->setTexture(texture);
                return;
            }
        }
        if (_variable->texture())
        {
            if (texture = (const ITexture*)_variable->texture())
            {
                _variable->setTexture(texture);
            }
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "GLOSSMAP")==0;
    }
};

class RenderDebugTermValue :  public ShaderParameterValue
{
  public:
    RenderDebugTermValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope (PerTechnique);
        setParameterType (RenderDebugTermOut);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        if (const Material* material = request.material)
        {
            float debugTerm = (float)material->debugTerm();
            _variable->set ((const float*)&debugTerm, sizeof (float));
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "RENDERDEBUGTERM")==0;
    }
};


class DiffuseMapValue :  public ShaderParameterValue
{
  public:
    DiffuseMapValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (NormalMap);
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        const ITexture* texture  = 0;
        if (const Material* material = request.material)
        {
            if (texture = material->albedoMap())
            {
                _variable->setTexture(texture);
                return;
            }
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "DIFFUSEMAP")==0;
    }
};

class NormalMapValue :  public ShaderParameterValue
{
  public:
    NormalMapValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (NormalMap);
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        const ITexture* texture  = 0;
        if (const Material* material = request.material)
        {
            if (texture = material->normalMap())
            {
                _variable->setTexture(texture);
                return;
            }
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "NORMALMAP")==0;
    }
};

class TextureGammaValue :  public ShaderParameterValue
{
  public:
    TextureGammaValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (TextureGamma);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        {
            // mdavidson TODO
            float gamma = request.material->textureGamma();
            _variable->set ((const float*)&gamma, sizeof (float));
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "TEXTUREGAMMA")==0;
    }
};


class EyeLocationValue :  public ShaderParameterValue
{
  public:
   EyeLocationValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {        
        setParameterScope (PerMesh);
        setParameterType (EyeLocation);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        if (request.camera)
        {
            const Ibl::CameraTransformCachePtr ctc = 
                request.camera->cameraTransformCache();

            const Ibl::Vector3f& t = ctc->cameraLocation();

            // Paraboloid direction goes in location
            Ibl::Vector4f eyeLocation (t.x, t.y, t.z, ctc->dpDir());
            _variable->setVector ((const float*)&eyeLocation.x);
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "EYELOCATION")==0;
    }
};


class CameraZNearValue : public ShaderParameterValue
{
  public:
    CameraZNearValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope (PerTechnique);
        setParameterType (CameraZNear);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        float value = request.camera->zNear();
        _variable->set (&value, sizeof(float));
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "CAMERAZNEAR")==0;
    }
};

class CameraZFarValue : public ShaderParameterValue
{
  public:
    CameraZFarValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope (PerTechnique);
        setParameterType (CameraZFar);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        float value = request.camera->zFar();
        _variable->set (&value, sizeof(float));
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "CAMERAZFAR")==0;
    }
};

class IBLBrdfValue :  public ShaderParameterValue
{
  public:
    IBLBrdfValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (IBLBRDFMap);
        setParameterScope(PerTechnique);
    }
    
    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        {
            _variable->setTexture(request.scene->activeBrdf()->brdfLut());
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "IBLBRDF")==0;
    }
};


class IBLDiffuseProbeMapValue :  public ShaderParameterValue
{
  public:
    IBLDiffuseProbeMapValue(const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (IBLDiffuseProbeMap);
        setParameterScope(PerTechnique);
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        uint32_t valueIndex = _variable->valueIndex();
        if (valueIndex < request.scene->probes().size())
        {
            _variable->setTexture(request.scene->probes()[valueIndex]->diffuseCubeMap());
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "IBLDIFFUSEPROBE")==0;
    }
};

class CubeViewsValue : public ShaderParameterValue
{
public:
	CubeViewsValue(const GpuVariable* variable, Ibl::IEffect*effect) :
		ShaderParameterValue(variable, effect)
	{
		setParameterScope(PerTechnique);
		setParameterType(CubeViews);
	}

	virtual void setParam(const Ibl::RenderRequest& request) const
	{
		// Setup views. In a real world example you would cache these on the IBL probe
		// or against some camera or transform.
		static Ibl::Matrix44f cubeViews[6];
		float lookAt = 1.5f;

		// Setup views for paraboloid to environment transform.
		Ibl::Vector3f eyeLocation = request.camera->cameraTransformCache()->cameraLocation();

		Ibl::Vector3f lookDirection = eyeLocation + Ibl::Vector3f(lookAt, 0, 0.0f);
		Ibl::Vector3f upDirection = Ibl::Vector3f(0.0f, 1.0f, 0.0f);
		Ibl::viewMatrixLH(eyeLocation, lookDirection, upDirection, &cubeViews[0]);

		lookDirection = eyeLocation + Ibl::Vector3f(-lookAt, 0.0f, 0.0f);
		upDirection = Ibl::Vector3f(0.0f, 1.0f, 0.0f);
		Ibl::viewMatrixLH(eyeLocation, lookDirection, upDirection, &cubeViews[1]);

		lookDirection = eyeLocation + Ibl::Vector3f(0.0f, lookAt, 0.0f);
		upDirection = Ibl::Vector3f(0.0f, 0.0f, -1.0f);
		Ibl::viewMatrixLH(eyeLocation, lookDirection, upDirection, &cubeViews[2]);

		lookDirection = eyeLocation + Ibl::Vector3f(0.0f, -lookAt, 0.0f);
		upDirection = Ibl::Vector3f(0.0f, 0.0f, 1.0f);
		Ibl::viewMatrixLH(eyeLocation, lookDirection, upDirection, &cubeViews[3]);

		lookDirection = eyeLocation + Ibl::Vector3f(0.0f, 0, lookAt);
		upDirection = Ibl::Vector3f(0.0f, 1.0f, 0.0f);
		Ibl::viewMatrixLH(eyeLocation, lookDirection, upDirection, &cubeViews[4]);

		lookDirection = eyeLocation + Ibl::Vector3f(0.0f, 0, -lookAt);
		upDirection = Ibl::Vector3f(0.0f, 1.0f, 0.0f);
		Ibl::viewMatrixLH(eyeLocation, lookDirection, upDirection, &cubeViews[5]);

		_variable->setMatrixArray((const float*)&cubeViews[0], 6);
	}

	static bool supports(GpuVariable* variable)
	{
		return _strcmpi((char*)variable->semantic().c_str(), "CUBEVIEWS") == 0;
	}

};


class IBLSpecularProbeMapValue :  public ShaderParameterValue
{
  public:
    IBLSpecularProbeMapValue(const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (IBLSpecularProbeMap);
        setParameterScope(PerTechnique);
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {

        if (_variable->valueIndex() < request.scene->probes().size())
        {
            _variable->setTexture(request.scene->probes()[0]->specularCubeMap());
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "IBLSPECULARPROBE")==0;
    }
};

class IBLSourceEnvironmentScaleValue :  public ShaderParameterValue
{
  public:
    IBLSourceEnvironmentScaleValue(const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (IBLSourceEnvironmentScale);
        setParameterScope(PerTechnique);
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        if (_variable->valueIndex() < request.scene->probes().size())
        {
            float environmentScale = (float)(request.scene->probes()[0]->environmentScale());
            _variable->set (&environmentScale, sizeof(float));
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "IBLSOURCEENVIRONMENTSCALE")==0;
    }
};

class IBLSpecularMipDeltasValue :  public ShaderParameterValue
{
  public:
    IBLSpecularMipDeltasValue(const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (IBLSpecularMipDeltas);
        setParameterScope(PerTechnique);
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        if (_variable->valueIndex() < request.scene->probes().size())
        {
            float mipCountDelta = (float)(request.scene->probes()[0]->specularCubeMap()->resource()->mipLevels());
            mipCountDelta = Ibl::maxValue((mipCountDelta-request.scene->probes()[0]->mipDrop())-1.0f, 1.0f);
            _variable->set (&mipCountDelta, sizeof(float));
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "IBLSPECULARMIPDELTAS")==0;
    }
};

class IBLSourceMipCountValue :  public ShaderParameterValue
{
  public:
    IBLSourceMipCountValue(const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (IBLSourceMipCount);
        setParameterScope(PerTechnique);
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        if (_variable->valueIndex() < request.scene->probes().size())
        {
            float mipCountDelta = (float)(request.scene->probes()[0]->environmentCubeMap()->resource()->mipLevels());
            mipCountDelta = Ibl::maxValue(mipCountDelta-1.0f, 1.0f);
            _variable->set (&mipCountDelta, sizeof(float));
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "IBLSOURCEMIPCOUNT")==0;
    }
};

class IBLCorrectionValue :  public ShaderParameterValue
{
  public:
    IBLCorrectionValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope (PerTechnique);
        setParameterType (IBLCorrection);
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        if (request.scene->probes().size() > 0)
        {
            IBLProbe* probe = request.scene->probes()[0];
            // Contrast, Saturation, Hue, placeholder
            Ibl::Vector4f iblCorrection = 
                Ibl::Vector4f(probe->iblContrast(), probe->iblSaturation(), probe->iblHue(), 1.0f);
            _variable->setVector ((const float*)&iblCorrection.x);
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "IBLCORRECTION")==0;
    }
};

class IBLMaxValueValue :  public ShaderParameterValue
{
  public:
    IBLMaxValueValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope (PerTechnique);
        setParameterType (IBLMaxValue);
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        {
            Ibl::Vector4f iblCorrection = 
                Ibl::Vector4f(request.scene->probes()[0]->maxPixelR(),
                             request.scene->probes()[0]->maxPixelG(),
                             request.scene->probes()[0]->maxPixelB(),
                             1.0f);
            _variable->setVector ((const float*)&iblCorrection.x);
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "IBLMAXVALUE")==0;
    }
};

class EnvironmentMapValue :  public ShaderParameterValue
{
  public:
    EnvironmentMapValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (EnvironmentMap);
        setParameterScope(PerMesh);
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        const ITexture* texture  = 0;
        if (const Material* material = request.material)
        {
            if (texture = material->environmentMap())
            {
                _variable->setTexture(texture);
                return;
            }
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "ENVIRONMENTMAP")==0;
    }
};

class TextureFunctionParameterValue :  public ShaderParameterValue
{
  public:
    TextureFunctionParameterValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope (PerTechnique);
        setParameterType (TextureFunction);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        if (const ITexture* texture = _variable->texture())
        {
            if (texture = (const ITexture*)_variable->texture())
            {
                _variable->setTexture(texture);
            }
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "TEXTUREFUNCTION")==0;
    }
};





class ExposureParameterValue :  public ShaderParameterValue
{
  public:
    ExposureParameterValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope (PerTechnique);
        setParameterType (Exposure);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        float value = request.camera->exposureProperty()->get();
        _variable->set (&value, sizeof(float));
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "EXPOSURE")==0;
    }
};

class GammaParameterValue :  public ShaderParameterValue
{
  public:
    GammaParameterValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope (PerTechnique);
        setParameterType (Gamma);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        float value = request.camera->gammaProperty()->get();
        _variable->set (&value, sizeof(float));
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "GAMMA")==0;
    }
};

class PostProcessMapValue :  public ShaderParameterValue
{
  public:
    PostProcessMapValue  (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (PostProcessMap);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        if (const PostEffect* rt = dynamic_cast <const PostEffect*>(request.mesh))
        { 
            if (const ITexture* postProcessTexture = rt->currentSource())
            {
                _variable->setTexture(postProcessTexture);
            }
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "POSTPROCESSMAP")==0;
    }
};


class TextureInputValue :  public ShaderParameterValue
{
  public:
    TextureInputValue  (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope(PerMesh);
        setParameterType (TextureInput);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        if (const PostEffect* rt = dynamic_cast <const PostEffect*>(request.mesh))
        { 
            if (const ITexture* postProcessTexture = rt->input(_variable->valueIndex()))
            {
                _variable->setTexture(postProcessTexture);
            }
        }
    }

    static bool supports (GpuVariable* variable)
    {
        static std::string textureInputSemantic = "TEXTUREINPUT";
        if (variable->semantic() == textureInputSemantic)
        {
            return true;
        }
        return false;
    }
};

class TextureInputSizeValue :  public ShaderParameterValue
{
  public:
    TextureInputSizeValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope(PerMesh);
        setParameterType (TextureInputSize);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        if (const PostEffect* rt = dynamic_cast <const PostEffect*>(request.mesh))
        { 
            if (const ITexture* postProcessTexture = rt->input(_variable->valueIndex()))
            {
                Ibl::Vector2f size = Ibl::Vector2f((float) postProcessTexture->width(), (float) postProcessTexture->height());
                _variable->set (&size.x, sizeof (float) * 2);
            }
        }
    }

    static bool supports (GpuVariable* variable)
    {
        static std::string textureInputSemantic = "TEXTUREINPUTSIZE";
        if (variable->semantic() == textureInputSemantic)
        {
            return true;
        }
        return false;
    }
};

class TextureInputWidthValue :  public ShaderParameterValue
{
  public:
    TextureInputWidthValue  (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope(PerMesh);
        setParameterType (TextureInputWidth);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        if (const PostEffect* rt = dynamic_cast <const PostEffect*>(request.mesh))
        { 
            if (const ITexture* postProcessTexture = rt->input(_variable->valueIndex()))
            {
                float width = (float) postProcessTexture->width();
                _variable->set (&width, sizeof (float));
            }
        }
    }

    static bool supports (GpuVariable* variable)
    {
        static std::string textureInputSemantic = "TEXTUREINPUTWIDTH";
        if (variable->semantic() == textureInputSemantic)
        {
            return true;
        }
        return false;
    }
};

class TextureInputHeightValue :  public ShaderParameterValue
{
  public:
    TextureInputHeightValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope(PerMesh);
        setParameterType (TextureInputHeight);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        if (const PostEffect* rt = dynamic_cast <const PostEffect*>(request.mesh))
        { 
            if (const ITexture* postProcessTexture = rt->input(_variable->valueIndex()))
            {
                float height = (float) postProcessTexture->height();
                _variable->set (&height, sizeof (float));
            }
        }
    }

    static bool supports (GpuVariable* variable)
    {
        static std::string textureInputSemantic = "TEXTUREINPUTHEIGHT";
        if (variable->semantic() == textureInputSemantic)
        {
            return true;
        }
        return false;
    }
};

class TargetTextureSizeValue :  public ShaderParameterValue
{
  public:
    TargetTextureSizeValue (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope(PerMesh);
        setParameterType (TargetTextureSize);
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        if (const PostEffect* rt = dynamic_cast <const PostEffect*>(request.mesh))
        { 
            if (const ISurface* surface = rt->texture()->surface())
            {
                Ibl::Vector2f size = Ibl::Vector2f((float) surface->width(), (float) surface->height());
                _variable->set (&size.x, sizeof (float) * 2);
            }
        }
        else
        {
            // TODO: Need to access current back buffer.
            float width = (float)(request.mesh->device()->backbuffer()->width());
            float height = (float)(request.mesh->device()->backbuffer()->height());
            Ibl::Vector2f size (width, height);
            _variable->set (&size.x, sizeof (float) * 2);
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "TARGETTEXTURESIZE")==0;
    }
};

class TargetTextureWidthValue :  public ShaderParameterValue
{
  public:
    TargetTextureWidthValue  (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterScope(PerMesh);
        setParameterType (TargetTextureWidth);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        if (const PostEffect* rt = dynamic_cast <const PostEffect*>(request.mesh))
        { 
            if (const ITexture* postProcessTexture = rt->currentSource())
            {
                float width = (float) postProcessTexture->width();
                _variable->set (&width, sizeof (float));
            }
        }
        else
        {
            // TODO: Need to access current back buffer.
            float width = (float)(request.mesh->device()->backbuffer()->width());
            _variable->set (&width, sizeof (float));
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "TARGETTEXTUREWIDTH")==0;
    }
};

class TargetTextureHeightValue :  public ShaderParameterValue
{
  public:
    TargetTextureHeightValue  (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (TargetTextureHeight);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        if (const PostEffect* rt = dynamic_cast <const PostEffect*>(request.mesh))
        { 
            if (const ITexture* postProcessTexture = rt->currentSource())
            {
                float height = (float)postProcessTexture->height();
                _variable->set (&height, sizeof (float));
            }
        }
        else
        {
            // TODO: Need to access current back buffer.
            float height = (float)(request.mesh->device()->backbuffer()->height());
            _variable->set (&height, sizeof (float));
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "TARGETTEXTUREHEIGHT")==0;
    }
};

class BackBufferWidthValue :  public ShaderParameterValue
{
  public:
    BackBufferWidthValue  (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (BackBufferWidth);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        {
            // TODO: Need to access current back buffer.
            uint32_t width = (uint32_t)(request.mesh->device()->backbuffer()->width());
            _variable->set (&width, sizeof (uint32_t));
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "BACKBUFFERWIDTH")==0;
    }
};

class BackBufferHeightValue :  public ShaderParameterValue
{
  public:
    BackBufferHeightValue  (const GpuVariable* variable, Ibl::IEffect*effect) : 
        ShaderParameterValue (variable, effect)
    {
        setParameterType (BackBufferHeight);        
    }

    virtual void setParam (const Ibl::RenderRequest& request) const
    {
        {
            // TODO: Need to access current back buffer.
            uint32_t height = (uint32_t)(request.mesh->device()->backbuffer()->height());
            _variable->set (&height, sizeof (uint32_t));
        }
    }

    static bool supports (GpuVariable* variable)
    {
        return _strcmpi((char*)variable->semantic().c_str(), "BACKBUFFERHEIGHT")==0;
    }
};

ShaderParameterValueFactory::ShaderParameterValueFactory (Ibl::IDevice* device) : 
    _deviceInterface(device)
{

}

ShaderParameterValueFactory::~ShaderParameterValueFactory()
{
    // Delete any values that we created.
    for (auto it = _managedValues.begin(); it != _managedValues.end(); it++)
    {
        ShaderParameterValue * parameter = *it;
        safedelete(parameter);
    }
    
    for (auto it = _parameters.begin(); it != _parameters.end(); it++)
    {
        IShaderParameterFactory* parameter = *it;
        safedelete (parameter);
    }

    _managedValues.clear();
}

bool
ShaderParameterValueFactory::create()
{
    return true;
}

bool
ShaderParameterValueFactory::cache()
{
    return true;
}

bool
ShaderParameterValueFactory::free()
{
    for (auto it = _managedValues.begin(); it != _managedValues.end(); it++)
    {
        ShaderParameterValue * shaderValue = *it;
        safedelete (shaderValue);
    }
    _managedValues.clear();
    return true;
}

template <class T>
class ShaderParameterFactory : public IShaderParameterFactory
{
  public:
    ShaderParameterFactory() {};

    virtual bool supports (GpuVariable* variable) const
    {
        return T::supports(variable);
    }

    virtual ShaderParameterValue * make (GpuVariable* variable,
                                         Ibl::IEffect* effect)
    {
        if (T * parameter = new T (variable, effect))
        {
            variable->setParameterType (parameter->parameterType());
            return parameter;
        }
        return nullptr;
    }
};

void
ShaderParameterValueFactory::removeShaderParameterValue(const ShaderParameterValue* value)
{
    for (auto it = _managedValues.begin(); it != _managedValues.end(); it++)
    {
        if (*it == value)
        {
            ShaderParameterValue* valuePtr = *it;
            _managedValues.erase(it);
            safedelete(valuePtr);
            return;
        }
    }
}

const ShaderParameterValue*    
ShaderParameterValueFactory::createShaderParameterValue (Ibl::IDevice* device,
                                                         GpuVariable* variable, 
                                                         Ibl::IEffect*effect)
{

    if (_parameters.size() == 0)
    {
        _parameters.insert (new ShaderParameterFactory <WorldMatrixValue>);
        _parameters.insert (new ShaderParameterFactory <WorldViewProjectionValue>);
        _parameters.insert (new ShaderParameterFactory <ViewProjectionValue>());
        _parameters.insert (new ShaderParameterFactory <ProjectionValue>);
        _parameters.insert (new ShaderParameterFactory <WorldViewValue>());
		_parameters.insert(new ShaderParameterFactory <CubeViewsValue>());
		
        _parameters.insert (new ShaderParameterFactory <DiffuseMapValue>());
        _parameters.insert (new ShaderParameterFactory <RenderDebugTermValue>());
        
        _parameters.insert (new ShaderParameterFactory <NormalMapValue>());
        _parameters.insert (new ShaderParameterFactory <SpecularRMCMapValue>());
        _parameters.insert (new ShaderParameterFactory <TextureGammaValue>());
        _parameters.insert (new ShaderParameterFactory <EnvironmentMapValue>());
        _parameters.insert (new ShaderParameterFactory <IBLCorrectionValue>());
        _parameters.insert (new ShaderParameterFactory <IBLMaxValueValue>());
        
        _parameters.insert (new ShaderParameterFactory <PostProcessMapValue>());
        _parameters.insert (new ShaderParameterFactory <ViewUpValue>());
        _parameters.insert (new ShaderParameterFactory <ViewRightValue>());
        _parameters.insert (new ShaderParameterFactory <ViewLookAtValue>());
        _parameters.insert (new ShaderParameterFactory <ViewValue>());
        _parameters.insert (new ShaderParameterFactory <ExposureParameterValue>());
        _parameters.insert (new ShaderParameterFactory <GammaParameterValue>());
        _parameters.insert (new ShaderParameterFactory <ScreenSizeValue>());
        _parameters.insert (new ShaderParameterFactory <TextureFunctionParameterValue>());
        _parameters.insert (new ShaderParameterFactory <TargetTextureSizeValue>());
        _parameters.insert (new ShaderParameterFactory <TargetTextureWidthValue>());
        _parameters.insert (new ShaderParameterFactory <TargetTextureHeightValue>());
        _parameters.insert (new ShaderParameterFactory <TextureInputValue>());
        _parameters.insert (new ShaderParameterFactory <TextureInputSizeValue>());
        _parameters.insert (new ShaderParameterFactory <TextureInputWidthValue>());
        _parameters.insert (new ShaderParameterFactory <TextureInputHeightValue>());
        _parameters.insert (new ShaderParameterFactory <CameraZNearValue>());
        _parameters.insert (new ShaderParameterFactory <CameraZFarValue>());
        _parameters.insert (new ShaderParameterFactory <MeshGroupIdValue>());
        _parameters.insert (new ShaderParameterFactory <BackBufferWidthValue>());
        _parameters.insert (new ShaderParameterFactory <BackBufferHeightValue>());
        _parameters.insert (new ShaderParameterFactory <IBLDiffuseProbeMapValue>());
        _parameters.insert (new ShaderParameterFactory <IBLSpecularProbeMapValue>());

        _parameters.insert (new ShaderParameterFactory <IBLSpecularMipDeltasValue>());
        _parameters.insert (new ShaderParameterFactory <IBLSourceMipCountValue>());
        _parameters.insert (new ShaderParameterFactory <IBLSourceEnvironmentScaleValue>());

        _parameters.insert (new ShaderParameterFactory <IBLBrdfValue>());
        _parameters.insert (new ShaderParameterFactory <MaterialDiffuseValue>());
        _parameters.insert(new ShaderParameterFactory <EyeLocationValue>());

        _parameters.insert(new ShaderParameterFactory <DetailMapValue>());

        _parameters.insert(new ShaderParameterFactory <SpecularIntensityValue>());
        _parameters.insert(new ShaderParameterFactory <RoughnessScaleValue>());
        _parameters.insert(new ShaderParameterFactory <SpecularWorkflowValue>());

        // Christ I hate this code, which is why it is all going to die very soon and be reborn.
        _parameters.insert(new ShaderParameterFactory <UserAlbedoValue>());
        _parameters.insert(new ShaderParameterFactory <UserRMValue>());
        _parameters.insert(new ShaderParameterFactory <IblOcclValue>());
    }

    ShaderParameterValue* value = nullptr;
    for (auto it = _parameters.begin(); it != _parameters.end(); it++)
    {
        if ((*it)->supports (variable))
        {
            value = (*it)->make (variable, effect);
            break;
        }
    }

    if (value == nullptr)
    {
        if ( variable->semantic().size() > 0)
        {
            // LOG ("Could not find value provider for variable " << variable->name() << " with semantic " << variable->semantic() );
        }
    }

    // Check if the semantic is unknown or not.
    if (value == 0)
    {
        // if we get this far then this is a value unknown to the engine
        variable->setParameterType (UnknownParameter);
        value = new UnknownValue (variable, effect);
    }

    _managedValues.insert(value);
    return value;
}

}
