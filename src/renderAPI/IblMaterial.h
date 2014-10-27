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
#ifndef INCLUDED_MATERIAL
#define INCLUDED_MATERIAL

#include <IblPlatform.h>
#include <IblRenderNode.h>
#include <IblVector4.h>

namespace Ibl
{
class GpuTechnique;
class IShader;

enum SpecularWorkflow
{
    RoughnessMetal,
    GlossMetal,
    RoughnessInverseMetal,
    GlossInverseMetal
};

enum RenderDebugTerm
{
    NoDebugTerm = 0,
    NormalTerm = 1,
    AmbientOcclusionTerm = 2,
    AlbedoTerm = 3,
    IBLDiffuseTerm = 4,
    IBLSpecularTerm = 5,
    MetalTerm = 6,
    RoughnessTerm = 7,
    BrdfTerm = 8
};

class Material : public RenderNode
{
  public:
    Material(Ibl::IDevice* device, 
             const std::string& filePathName = std::string());
    virtual ~Material();

    // Load from xml.
    bool                       load (const std::string& filename);

    const std::string &        shaderName() const;
    const std::string &        techniqueName() const;
    void                       setShaderName(const std::string &);
    void                       setTechniqueName(const std::string &);

    void                       setTechnique(const GpuTechnique*);
    void                       setShader(const IShader* shader);

    void                       addPass(const std::string& passName);

    const GpuTechnique*        technique() const;
    const IShader*             shader() const;

    const Ibl::ITexture*       specularRMCMap() const;
    const Ibl::ITexture*       normalMap() const;
    const Ibl::ITexture*       environmentMap() const;
    const Ibl::ITexture*       albedoMap() const;
    const Ibl::ITexture*       detailMap() const;

    void                       setAlbedoMap(const Ibl::ITexture* texture);
    void                       setNormalMap(const Ibl::ITexture* texture);
    void                       setEnvironmentMap(const Ibl::ITexture* texture);
    void                       setSpecularRMCMap(const Ibl::ITexture* texture);
    void                       setDetailMap(const Ibl::ITexture* texture);

    void                       setAlbedoMap(const std::string& filePathName);
    void                       setNormalMap(const std::string& filePathName);
    void                       setEnvironmentMap(const std::string& filePathName);
    void                       setSpecularRMCMap(const std::string& filePathName);
    void                       setDetailMap(const std::string& filePathName);

    const std::vector<std::string>& passes() const;

    Ibl::BoolProperty*          twoSidedProperty();
    bool                        twoSided() const;

    Ibl::FloatProperty*         textureGammaProperty();
    float                       textureGamma() const;

    Ibl::Vector4fProperty*      albedoColorProperty();
    const Ibl::Vector4f&        albedoColor() const;

    Ibl::IntProperty*           debugTermProperty();
    int32_t                     debugTerm() const;

    Ibl::IntProperty*           specularWorkflowProperty();
    int32_t                     specularWorkflow() const;
    Ibl::FloatProperty*         roughnessScaleProperty();
    float                       roughnessScale() const;
    Ibl::FloatProperty*         specularIntensityProperty();
    float                       specularIntensity() const;

    Ibl::Vector4fProperty*      detailTermsProperty();
    const Ibl::Vector4f&        detailTerms() const;

  private:
    // Shader and pass management
    std::vector<std::string>   _passes;
    std::string                _shaderName;
    std::string                _techniqueName;
    const GpuTechnique*        _technique;
    const IShader*             _shader;
    RenderDebugTerm            _debugTerm;

    // Maps
    const Ibl::ITexture*        _specularRMCMap;
    const Ibl::ITexture*        _normalMap;
    const Ibl::ITexture*        _environmentMap;
    const Ibl::ITexture*        _albedoMap;
    const Ibl::ITexture*        _detailMap;

    // Flags
    Ibl::BoolProperty *         _twoSidedProperty;
    Ibl::FloatProperty *        _textureGammaProperty;
    Ibl::IntProperty*           _debugTermProperty;
    Ibl::IntProperty*           _specularWorkflowProperty;
    Ibl::FloatProperty*         _roughnessScaleProperty;
    Ibl::FloatProperty*         _specularIntensityProperty;
    Ibl::Vector4fProperty*      _albedoColorProperty;

    Ibl::Vector4fProperty*      _detailTermsProperty;
};
}

#endif