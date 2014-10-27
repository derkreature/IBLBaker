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

#ifndef INCLUDED_BB_CAMERA
#define INCLUDED_BB_CAMERA

#include <IblPlatform.h>
#include <IblTransformNode.h>

namespace Ibl
{
class ViewProperty;
class ProjectionProperty;
class ViewportProperty;
class RenderTextureProperty;
class DepthSurfaceProperty;

class CameraTransformCache 
{
  public:
    CameraTransformCache ();
    virtual ~CameraTransformCache ();

    void                       set(const Ibl::Matrix44f& view, 
                                   const Ibl::Matrix44f& proj, 
                                   const Ibl::Matrix44f& viewProj, 
                                   const Ibl::Vector3f& location, 
                                   float zNear, 
                                   float zFar, 
                                   float dp);

    const Matrix44f&           viewMatrix() const;
    const Matrix44f&           projMatrix() const;
    const Matrix44f&           viewProjMatrix() const;

    const Ibl::Vector3f&       cameraLocation() const;

    float                      zNear() const;
    float                      zFar() const;
    float                      dpDir() const;

    Matrix44f                  _view;
    Matrix44f                  _proj;
    Matrix44f                  _viewProj;
    float                      _zNear;
    float                      _zFar;
    float                      _dpDir;
    Vector3f                   _cameraLocation;
};

typedef std::shared_ptr<CameraTransformCache> CameraTransformCachePtr;

class Camera : public TransformNode
{
  public:
    Camera(Ibl::IDevice* device);
    virtual ~Camera();

    void                       updateViewProjection();

    const Ibl::Matrix44f&       projMatrix() const;
    const Ibl::Matrix44f&       viewMatrix() const;

    Ibl::Matrix44f              viewProjMatrix() const;
    const Ibl::Vector3f&        lookAt() const;
    const Ibl::Vector3f&        forward() const;

    const Ibl::Vector3f*        frustumVertices();

    float                      aspectRatio() const;

    FloatProperty*             focalLengthProperty();
 
    FloatProperty*             fovYProperty();
    FloatProperty*             fovXProperty();                
    FloatProperty*             aspectRatioProperty();

    FloatProperty*             nearClipProperty();
    FloatProperty*             farClipProperty();

    FloatProperty*             aspectWidthProperty();
    FloatProperty*             aspectHeightProperty();

    ViewProperty*              viewProperty() const;
    ProjectionProperty*        projectionProperty();
    const ProjectionProperty*  projectionProperty()const;

    ViewportProperty*          viewportProperty() const;
    FloatProperty*             exposureProperty() const;
    FloatProperty*             gammaProperty() const;
    
    BoolProperty*              useDepthOfFieldProperty() const;
    BoolProperty*              useAmbientOcclusionProperty() const;
    BoolProperty*              useDualPerspectiveProperty() const;
    BoolProperty*              useBakedUpForwardsProperty();

    VectorProperty*            bakedUpProperty();
    VectorProperty*            bakedForwardsProperty();

    void                       setUseDepthOfField(bool value);
    void                       setUseAmbientOcclusion(bool value);

    bool                       useDepthOfField() const;
    bool                       useAmbientOcclusion() const;    
    
    float                      exposure() const;
    float                      gamma() const;

    float                      zNear() const;
    float                      zFar() const;
    float                       fovY() const;

    void                       swapLuminanceBuffer() const;

    uint32_t                     adaptedLuminanceBufferIndex() const;
    RenderTextureProperty*     adaptedLuminanceTextureProperty() const;
    RenderTextureProperty*     adaptedLuminanceTextureProperty(uint32_t index) const;

    RenderTextureProperty*      initialLuminanceTextureProperty() const;
    RenderTextureProperty*      bloomTextureProperty() const;

    float                       bloomThreshold() const;
    float                       bloomMagnitude() const;
    float                       bloomBlurSigma() const;
    float                       bloomTau() const;
    float                       bloomKeyValue() const;

    FloatProperty*              bloomThresholdProperty() const;
    FloatProperty*              bloomMagnitudeProperty() const;
    FloatProperty*              bloomBlurSigmaProperty() const;
    FloatProperty*              bloomTauProperty() const;
    FloatProperty*              bloomKeyValueProperty() const;

    // This layer provides an override layer to the camera system.
    // It provides a virtual layer of the camera container for other 
    // in place modification of transform information.
    CameraTransformCachePtr&    cameraTransformCache();
    const CameraTransformCachePtr& cameraTransformCache() const;
    void                        setCameraTransformCache(const CameraTransformCachePtr& cache);
    void                        cacheCameraTransforms() const;

  protected:    
    IntProperty*                _displayWidthProperty;
    IntProperty*                _displayHeightProperty;

  private:
    FloatProperty*             _fovYProperty;
    FloatProperty*             _fovXProperty;
    FloatProperty*             _aspectRatioProperty;    
    FloatProperty*             _nearClipProperty;        
    FloatProperty*             _farClipProperty;            
    FloatProperty*             _focalLengthProperty;

    FloatProperty*             _aspectWidthProperty;
    FloatProperty*             _aspectHeightProperty;

    ViewProperty*              _viewProperty;
    ViewportProperty*          _viewportProperty;
    ProjectionProperty*        _projectionProperty;

    FloatProperty*             _gammaProperty;
    FloatProperty*             _exposureProperty;

    Ibl::Vector3f               _frustumVertices[8];

    BoolProperty*              _useBakedUpForwardsProperty;
    VectorProperty*            _bakedUpProperty;
    VectorProperty*            _bakedForwardsProperty;
    BoolProperty*              _useDualPerspectiveProperty;

    CameraTransformCachePtr    _cameraTransformCache;
};
}

#endif