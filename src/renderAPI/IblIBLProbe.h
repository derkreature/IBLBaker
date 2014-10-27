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
#ifndef INCLUDED_BB_IBL_PROBE
#define INCLUDED_BB_IBL_PROBE

#include <IblPlatform.h>
#include <IblTransformNode.h>
#include <IblRenderTextureProperty.h>
#include <IblViewport.h>
#include <IblMatrix44.h>
#include <IblITexture.h>
#include <IblHash.h>

namespace Ibl
{
class IDepthSurface;
class IDevice;
class Scene;

class IBLProbe : public Ibl::TransformNode
{
  public:
    IBLProbe(Ibl::IDevice * device);
    virtual ~IBLProbe();

    void                       destroy();

    // Swaps the ids for rendered and computed maps.
    int32_t                    samplesRemaining() const;
    int32_t                    sampleOffset() const;

    void                       updateSamples();

    void                       markComputed(bool computed);
    bool                       isCached ();
    void                       update();
    void                       uncache();
    bool                       computed() const;

    const Ibl::Viewport&        iblViewport() const;
    const Ibl::Viewport&        specularViewport() const;

    const Ibl::Matrix44f &      basis() const;
    const Ibl::Matrix44f &      inverseBasis() const;

    const IDevice*             device() const;

    const Ibl::Vector3f &       center() const;
    void                       setCenter(const Ibl::Vector3f &);

    const Ibl::ITexture*        environmentCubeMap() const;
    const Ibl::ITexture*        diffuseCubeMap() const;
    const Ibl::ITexture*        specularCubeMap() const;

    const Ibl::ITexture*        environmentCubeMapMDR() const;
    const Ibl::ITexture*        diffuseCubeMapMDR() const;
    const Ibl::ITexture*        specularCubeMapMDR() const;

    const Ibl::ITexture*        lastDiffuseCubeMap() const;
    const Ibl::ITexture*        lastSpecularCubeMap() const;

    IntProperty *              sampleCountProperty();
    int32_t                    sampleCount() const;

    int32_t                    samplesPerFrame() const;
    IntProperty*               samplesPerFrameProperty();

    int32_t                    mipDrop() const;
    IntProperty*               mipDropProperty();

    int32_t                    diffuseResolution() const;
    IntProperty*               diffuseResolutionProperty();

    IntProperty*               specularResolutionProperty();
    int32_t                    specularResolution() const;

    float                      iblContrast() const;
    FloatProperty*             iblContrastProperty();

    float                      iblHue() const;
    FloatProperty*             iblHueProperty();

    float                      iblSaturation() const;
    FloatProperty*             iblSaturationProperty();

    float                      environmentScale() const;
    FloatProperty*             environmentScaleProperty();

    FloatProperty*             maxPixelRProperty();
    float                      maxPixelR() const;

    FloatProperty*             maxPixelGProperty();
    float                      maxPixelG() const;

    FloatProperty*             maxPixelBProperty();
    float                      maxPixelB() const;

    IntProperty*               sourceResolutionProperty();
    int32_t                    sourceRespolution() const;

    Ibl::PixelFormatProperty* hdrPixelFormatProperty();
    Ibl::PixelFormat           hdrPixelFormat() const;

  protected:
    void                       setupCubeMap(Ibl::RenderTextureProperty* cubeMapProperty, 
                                            IntProperty*  size, 
                                            PixelFormatProperty* format);

    int32_t                    _renderId;
    int32_t                    _samplesRemaining;
    int32_t                    _sampleOffset;

    // Mip drop for roughness.
    // Diffuse is encoded on export into numMips -_mipDrop as an option.
    // Default is 3.
    IntProperty*               _mipDropProperty;
    IntProperty*               _samplesPerFrameProperty;
    IntProperty*               _sampleCountProperty;
    BoolProperty*              _markedComputedProperty;
    IntProperty*               _diffuseResolutionProperty;
    IntProperty*               _specularResolutionProperty;
    FloatProperty*             _environmentScaleProperty;
    FloatProperty*             _iblSaturationProperty;
    FloatProperty*             _iblContrastProperty;
    FloatProperty*             _iblHueProperty;
    DeviceProperty*            _deviceProperty;

    // 16 byte murmer hash.
    Hash                       _probeHash;

    // This will need a policy at some stage.
    Ibl::Vector3f               _cachedRotation;
    Ibl::Vector3f               _cachedTranslation;


    // Matrix basis for paraboloid view directions.
    Ibl::Matrix44f              _basis;
    Ibl::Matrix44f              _inverseBasis;

    // Computed maps.
    PixelFormatProperty*       _mdrPixelFormatProperty;
    PixelFormatProperty*       _hdrPixelFormatProperty;
    IntProperty*               _dimensionProperty;
    IntProperty*               _sourceResolutionProperty;

    RenderTextureProperty*     _specularCubeMap[2];
    RenderTextureProperty*     _diffuseCubeMap[2];

    RenderTextureProperty*     _environmentCubeMap;

    RenderTextureProperty*     _environmentCubeMapMDR;
    RenderTextureProperty*     _diffuseCubeMapMDR;
    RenderTextureProperty*     _specularCubeMapMDR;

    FloatProperty*             _maxPixelRProperty;
    FloatProperty*             _maxPixelGProperty;
    FloatProperty*             _maxPixelBProperty;

    IDevice*                   _device;
    Ibl::Vector3f               _center;

};

}

#endif