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


#ifndef INCLUDED_BB_SHADER_PARAMETER_VALUE
#define INCLUDED_BB_SHADER_PARAMETER_VALUE

#include <IblPlatform.h>
#include <IblRenderRequest.h>

namespace Ibl
{
class Camera;
class Mesh;
class PostEffect;
class GpuVariable;
class GpuTechnique;
class IEffect;
class IDevice;

enum ShaderParameter
{
    UnknownParameter = 0,
    WorldMatrix,
    WorldInverse,
    WorldITMatrix,
    WorldViewITMatrix,
    WorldViewProjection,
    // Still needs factory
    LastWorldViewProjection,
    WorldViewProjectionInverse,
    Projection,
    ViewProjection,
    ViewProjectionInverse,
    ProjectionInverse,
    WorldView,
    WorldViewInverse,
    ViewInverse,
    ShadowBlur,
    LightViewProjectionTexScale,/* 0 ... n */
    LightViewProjectionTexScaleSansWorld, /* 0 ... n */
    FresnelModulation,
    FresnelVector,
    EyeLocation,
    TexMatrix,
    LightPosition, /* 0 ... n */
    LightDirection, /* 0 ... n */
    SceneAmbient, /* 0 ... n */
    LightDiffuse, /* 0 ... n */
    LightEmissive,
    MaterialDiffuse,
    MaterialSpecular,
    DiffuseLightModelParams,
    DiffuseMap,  
    SpecularEnvironmentAlbedoMap, // Environment reflection modulation map. RGB strength.
    SpecularModelParameters, // 4 component of spec model parameters. Roughness, Metalness, Cavity for BRDF based materials.
    SpecularFresnel, // Old specular model. strength, exponent, fresnel strength, fresnel exponent. 
    SpecularRMCMap, // Specular Roughness, Metalness, Cavity
    EffectsMaskMap, 
    BRDFMap,
    DisplacementMap,
    HeatGlowMap, 
    HeatGlowMapOutput,
    NormalMap,
    LightMap,
    ShadowMap,
    ShadowMapArray, 
    ReflectionMap,
    EnvironmentMap,
    PostProcessMap,
    FakeSSSMap,
    FakeSSSTerms,
    CameraNormalAndDepthMap,
    Joints,
    DualQuaternionJoints,
    SampleOffsets,
    ViewUp,
    ViewRight,
    ViewLookAt,
    LightViewProjection, /* 0 ... n */
    LightViewProjectionSansWorld, /* 0 ... n */    
    LightView, /* 0 ... n */
    LightViewSansWorld, /* 0 ... n */
    LightProj, /* 0 ... n */
    LightViewProjBias, /* 0 ... n */
    LightViewProjScrollA, /* 0 ... n */
    LightViewProjScrollB, /* 0 ... n */
    LightViewMinBounds, /* 0 ... n */
    LightViewMaxBounds, /* 0 ... n */
    CameraToLightViewProj,
    CameraToLightView,
    GaussOffsets,
    GaussWeights,
    BlurMapH,
    BlurMapV,
    View,
    ShadowTexelSize,
    DOFMap,
    FarFocus,
    NearFocus,
    Focus,
    Exposure,
    Gamma,
    ElapsedTime,
    NormalizedTime,
    DemoTime,
    ScreenSize,
    TextureFunction,
    TargetTextureSize,
    TargetTextureWidth,
    TargetTextureHeight,
    FractionOfMesh,
    TextureInput,
    TextureInputSize,
    TextureInputWidth,
    TextureInputHeight,
    HardwareDepthTexture,
    VerticalPassFlag,
    LastFrame,
    PostEffectWeight,
    CameraZNear,
    CameraZFar,
    LightZNear, /* 0 ... n */
    LightZFar, /* 0 ... n */
    AmbientOcclusionMap,
    ApplySSAO,
    ApplyDepthOfField,
    MorphTargetPitch,
    MorphTargetCount,
    MorphTargetOffsets,
    MorphTargetWeights,
    MorphTargetVertices,
    SimulationVertices,
    DisplacementFactor,
    TessellationFactors,
    TangentStencil,
    TangentCoefficients,
    MeshMetaData,
    DensityBufferData,
    SeamlessTexCoords,
    LightLeakScale,
    ParallelSplitDistance,
    ParallelSplitMatrices,
    CameraSplitMatrices,
    ParallelSplitCount,
    BlurSampleWidth,
    NoiseNormalMap,
    SpecularColorMap,
    BezierWeights,
    GregoryPatchInternalData,
    GregoryStencil,
    PatchCount,
    PatchPrimitiveSize,
    BackBufferWidth,
    BackBufferHeight,
    CustomParameter,
    RefractionCoefficient,
    ViewITMatrix,
    BeckmannMap,
    LightAttenuationRange,
    TextureGamma,
    ApplyBloom,
    ApplyFilmicTone,
    BloomMagnitude,
    BloomMap,
    AdaptedLuminanceMap,
    BloomKey,
    MeshGroupId,
    MeshGroupMap,
    VignetteParameters,
    VignetteColor,
    LensDistortion,
    HueShift,
    HalfLambert,
    FresnelDiffuseColor,
    FresnelSpecularColor,
    FresnelDiffuseConstants,
    EnvironmentReflectTerm,
    NormalMapParams,

    SpecularWorkflowType,
    SpecularIntensity,
    RoughnessScale,
    
    DetailMap,
    DetailTerms,

    LocalScale,
    UVTransformPrimary, // Main Transform for UV's
    UVTransformSecondary,

    // IBL Lighting.
    ShadowDepthOffset,
    IBLDiffuseProbeMap,
    IBLSpecularProbeMap,
    IBLSourceMipCount,
    IBLSpecularMipDeltas,
    IBLSourceEnvironmentScale,
    IBLProbeCount,
    IBLBRDFMap,
    IBLCorrection,
    IBLMaxValue,
    // Debug parameters
    RenderDebugTermOut
};

enum ParameterScope
{
    PerMesh,
    PerTechnique
};

class ShaderParameterValue
{
  public:    
    ShaderParameterValue (const GpuVariable* variable, IEffect*effect);
    virtual ~ShaderParameterValue();

    virtual void                setParam (const Ibl::RenderRequest& request) const = 0;

    uint32_t                    parameterIndex() const {return _parameterIndex;}
    ShaderParameter                parameterType();
    ParameterScope              parameterScope() const { return _parameterScope; }
    void                        setParameterScope (ParameterScope scope) { _parameterScope = scope; };
    virtual bool                bindsResource() const { return false; }
    virtual void                unbind() const;

    const GpuVariable*            variable() const { return _variable; }

  protected:
    void                        setParameterType (ShaderParameter parameterType);    
    void                        setParameterIndex (uint32_t index) { _parameterIndex = index;}

    ParameterScope              _parameterScope;
    IEffect*                    _effect;
    const GpuVariable*          _variable;
    ShaderParameter             _parameterType;
    uint32_t                    _parameterIndex;
};

}

#endif