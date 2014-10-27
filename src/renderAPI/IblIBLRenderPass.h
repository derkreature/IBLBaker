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
#ifndef INCLUDED_BB_IBL_RENDERPASS
#define INCLUDED_BB_IBL_RENDERPASS

#include <IblPlatform.h>
#include <IblRenderPass.h>
#include <IblCamera.h>
#include <IblScene.h>
#include <IblIDepthSurface.h>
#include <IblIBLProbe.h>

namespace Ibl
{
class IShader;
class GPUTechnique;
class GPUVariable;
class IBLConvolutions;

// Render pass for volume generation.
class IBLRenderPass : public Ibl::RenderPass
{
  public:
    IBLRenderPass(Ibl::IDevice* device);
    virtual ~IBLRenderPass(void);

    virtual bool               cache();
    virtual bool               free ();
    virtual void               render(Ibl::Scene* scene);

    void                       colorConvert(Ibl::Scene* scene,
                                            Ibl::IBLProbe* probe);
    void                       colorConvert(bool applyMDR,
                                            bool useMips,
                                            Ibl::Scene* scene,
                                            const Ibl::ITexture* dst,
                                            const Ibl::ITexture* src,
                                            const Ibl::IBLProbe* probe);

  protected:
    bool                       loadMesh();

  private:
    // Refine importance sampling for specular cube.
    void                       refineSpecular(Ibl::Scene* scene,
                                              const Ibl::IBLProbe* probe);

    void                       refineDiffuse(Ibl::Scene* scene,
                                             const Ibl::IBLProbe* probe);


    // the objects that are visible to the camera.
    Ibl::CameraTransformCachePtr _paraboloidTransformCache;
    Ibl::CameraTransformCachePtr _environmentTransformCache;
    Ibl::IBLConvolutions*        _convolve;


    Ibl::Matrix44f               _cubeViews[6];

    // Procedural Splat Geometry
    Ibl::Entity*                _sphereEntity;
    Ibl::Mesh*                  _sphereMesh; 
    Ibl::Material*              _material;
    bool                        _cached;

    // Invalidation.
    Hash                       _diffuseHash;
    Hash                       _specularHash;
    Hash                       _colorHash;

    // Color Conversion shader to LDR and MDR.
    const Ibl::IShader*        _colorConversionShader;
    const Ibl::GpuTechnique*   _colorConversionTechnique;
    const Ibl::GpuVariable*    _colorConversionMipLevelVariable;
    const Ibl::GpuVariable*    _colorConversionIsMDRVariable;
    const Ibl::GpuVariable*    _colorConversionGammaVariable;
    const Ibl::GpuVariable*    _colorConversionLDRExposureVariable;
    const Ibl::GpuVariable*    _colorConversionMDRScaleVariable;

    float                       _colorConversionGamma;
    float                       _colorConversionLDRExposure;
    float                       _colorConversionMDRScale;
};

}

#endif
