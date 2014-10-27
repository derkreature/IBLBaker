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

#include <IblCamera.h>
#include <IblProjectionProperty.h>
#include <IblViewProperty.h>
#include <IblViewportProperty.h>
#include <IblISurface.h>

namespace Ibl
{

CameraTransformCache::CameraTransformCache () :
    _zNear(0),
    _zFar(1000),
    _dpDir(1.0f)
{
}

CameraTransformCache::~CameraTransformCache ()
{
}

const Matrix44f& 
CameraTransformCache::viewMatrix() const
{
    return _view;
}

const Matrix44f& 
CameraTransformCache::projMatrix() const
{
    return _proj;
}

const Matrix44f& 
CameraTransformCache::viewProjMatrix() const
{
    return _viewProj;
}

float
CameraTransformCache::zNear() const
{
    return _zNear;
}

float
CameraTransformCache::zFar() const
{
    return _zFar;
}

float
CameraTransformCache::dpDir() const
{
    return _dpDir;
}

const Vector3f&
CameraTransformCache::cameraLocation() const
{
    return _cameraLocation;
}

void
CameraTransformCache::set(const Ibl::Matrix44f& view, 
                          const Ibl::Matrix44f& proj, 
                          const Ibl::Matrix44f& viewProj, 
                          const Ibl::Vector3f& location, 
                          float zNear, 
                          float zFar, 
                          float dp)
{
    _view = view;
    _proj = proj;
    _viewProj = viewProj;
    _cameraLocation = location;
    _zNear = zNear;
    _zFar = zFar;
    _dpDir = dp;
}

Camera::Camera (Ibl::IDevice* device) :
TransformNode (device)
{
    // Create graph properties.
    _viewProperty = new ViewProperty (this,    std::string("v"));
    _projectionProperty = new ProjectionProperty (this, std::string("p"));
    _fovYProperty = new FloatProperty (this, std::string ("fovy"));
    _fovXProperty = new FloatProperty (this, std::string ("fovx"));
    _aspectRatioProperty = new FloatProperty (this, std::string ("aspectRatio"));
    _nearClipProperty = new FloatProperty (this, std::string ("nearClip"));
    _farClipProperty = new FloatProperty (this, std::string ("farClip"));
    _viewportProperty = new ViewportProperty (this, std::string ("viewportOp"));
    _aspectWidthProperty = new FloatProperty (this, std::string ("aspectWidthOp"));
    _aspectHeightProperty = new FloatProperty (this, std::string ("aspectHeightOp"));
    _displayWidthProperty = new IntProperty (this, std::string ("displayWidth"));
    _displayHeightProperty = new IntProperty (this, std::string ("displayHeight"));
    _gammaProperty = new FloatProperty(this, std::string("gamma"), new TweakFlags(0, 6, 1e-2f, "Color"));
    _exposureProperty = new FloatProperty(this, std::string("exposure"), new TweakFlags(0, 6, 1e-2f, "Color"));
    _useBakedUpForwardsProperty = new BoolProperty(this, std::string("ubf"));
    _bakedUpProperty = new VectorProperty(this,std::string("bu"));
    _bakedForwardsProperty = new VectorProperty(this, std::string("bf"));
    _useDualPerspectiveProperty = new BoolProperty(this, std::string("udp"));
    _focalLengthProperty = new FloatProperty (this, std::string ("focalLen"));

    _cameraTransformCache.reset(new Ibl::CameraTransformCache());


    // Slight hack until I have a property which can take aspect ratios.
    float _renderTargetWidth  =  (float)_device->backbuffer()->width();
    float _renderTargetHeight =  (float)_device->backbuffer()->height();

    // Set default values.
    _exposureProperty->set (2.0f);
    _gammaProperty->set (2.2f);
    _useDualPerspectiveProperty->set(false);
    _displayWidthProperty->set (_device->backbuffer()->width());
    _displayHeightProperty->set (_device->backbuffer()->height());
    _focalLengthProperty->set (50.0f); // 24mm
    
    fovYProperty()->set (40.0f * RAD);
    fovXProperty()->set (65.0f * RAD);
    
    aspectRatioProperty()->set ((_renderTargetWidth / _renderTargetHeight)); // need to look at this.
    nearClipProperty()->set (1.0f);
    farClipProperty()->set (20000);

    _useBakedUpForwardsProperty->set(false);

    _viewProperty->addDependency(_translationProperty, ViewProperty::Translation);
    _viewProperty->addDependency(_rotationProperty, ViewProperty::Rotation);
    _viewProperty->addDependency(_useBakedUpForwardsProperty, ViewProperty::UseBakedForwards);
    _viewProperty->addDependency(_bakedUpProperty, ViewProperty::BakedUp);
    _viewProperty->addDependency(_bakedForwardsProperty, ViewProperty::BakedForwards);

    _projectionProperty->addDependency(focalLengthProperty(), ProjectionProperty::FocalLength);
    _projectionProperty->addDependency(fovYProperty(), ProjectionProperty::FovY);
    _projectionProperty->addDependency(fovXProperty(), ProjectionProperty::FovX);
    _projectionProperty->addDependency(useDualPerspectiveProperty(), ProjectionProperty::DualPerspective);
    _projectionProperty->addDependency(aspectRatioProperty(), ProjectionProperty::AspectRatio);
    _projectionProperty->addDependency(nearClipProperty(), ProjectionProperty::NearClip);
    _projectionProperty->addDependency(farClipProperty(), ProjectionProperty::FarClip);


    _viewportProperty->addDependency(_aspectWidthProperty, ViewportProperty::Width);
    _viewportProperty->addDependency(_aspectHeightProperty, ViewportProperty::Height);
    _viewportProperty->addDependency(_deviceProperty, ViewportProperty::Device);
    

    _aspectWidthProperty->set (_renderTargetWidth);
    _aspectHeightProperty->set (_renderTargetHeight);

}

void
Camera::updateViewProjection()
{
    // Slight hack until I have a property which can take aspect ratios.

    if (_aspectWidthProperty->get() != _device->backbuffer()->width() ||
        _aspectHeightProperty->get() != _device->backbuffer()->height())
    {
        float renderTargetWidth  =  (float)_device->backbuffer()->width();
        float renderTargetHeight =  (float)_device->backbuffer()->height();
        _aspectRatioProperty->set ((renderTargetWidth / renderTargetHeight));
        _aspectWidthProperty->set (renderTargetWidth);
        _aspectHeightProperty->set (renderTargetHeight);
    }
}

Camera::~Camera()
{
}

CameraTransformCachePtr&
Camera::cameraTransformCache()
{
    return _cameraTransformCache;
}

const CameraTransformCachePtr&
Camera::cameraTransformCache() const
{
    return _cameraTransformCache;
}

void
Camera::cacheCameraTransforms() const
{
    _cameraTransformCache->set(viewMatrix(), projMatrix(), viewProjMatrix(),  translation(), zNear(), zFar(), 1.0f);
}

void
Camera::setCameraTransformCache(const CameraTransformCachePtr& cameraCache)
{
    _cameraTransformCache = cameraCache;
}

float
Camera::aspectRatio() const
{
    return _aspectRatioProperty->get();
}

const Ibl::Vector3f*
Camera::frustumVertices()
{
   // Compute Field of View as function of width at z==1

   float nearClip = _nearClipProperty->get();
   float farClip = _farClipProperty->get();
    
   float width = 0.25f;
   float height = 0.25f;
    
   float fov = 2 * atanf(width);
   float verticalFov = 2 * atanf(height);
   float aspect = 1.0f;

   // Size of near and far quads
   float nearWidth  = nearClip * tanf(fov/2.0f);
   float nearHeight = nearClip * tanf(verticalFov/2.0f);

   float farWidth  = farClip * tanf(fov/2.0f);
   float farHeight = farClip * tanf(verticalFov/2.0f);

   // Near quad of wireframe frustum
   _frustumVertices[0] = Ibl::Vector3f (-nearWidth,  nearHeight, nearClip);
   _frustumVertices[1] = Ibl::Vector3f ( nearWidth,  nearHeight, nearClip);
   _frustumVertices[2] = Ibl::Vector3f ( nearWidth, -nearHeight, nearClip);
   _frustumVertices[3] = Ibl::Vector3f (-nearWidth, -nearHeight, nearClip);
   _frustumVertices[4] = Ibl::Vector3f (-farWidth,  farHeight, farClip);
   _frustumVertices[5] = Ibl::Vector3f ( farWidth,  farHeight, farClip);
   _frustumVertices[6] = Ibl::Vector3f ( farWidth, -farHeight, farClip);
   _frustumVertices[7] = Ibl::Vector3f (-farWidth, -farHeight, farClip);

    return &_frustumVertices[0];
}

float
Camera::zNear() const
{
    return _nearClipProperty->get();
}

float
Camera::zFar() const
{
    return _farClipProperty->get();
}


FloatProperty*
Camera::exposureProperty() const
{
    return _exposureProperty;
}

FloatProperty*
Camera::gammaProperty() const
{
    return _gammaProperty;
}

float
Camera::exposure() const
{
    return _exposureProperty->get();
}

BoolProperty*
Camera::useDualPerspectiveProperty() const
{
    return _useDualPerspectiveProperty;
}

FloatProperty*
Camera::focalLengthProperty()
{
    return _focalLengthProperty;
}

float
Camera::gamma() const
{
    return _gammaProperty->get();
}

FloatProperty*
Camera::aspectWidthProperty()
{
    return _aspectWidthProperty;
}

FloatProperty*
Camera::aspectHeightProperty()
{
    return _aspectHeightProperty;
}

const Ibl::Vector3f&
Camera::lookAt() const
{
    return _viewProperty->lookAtProperty()->get();
}

const Ibl::Matrix44f&
Camera::projMatrix() const
{
    return _projectionProperty->projMatrixProperty()->get();
}

const Ibl::Matrix44f&
Camera::viewMatrix() const
{
    return _viewProperty->viewMatrixProperty()->get();
}

Ibl::Matrix44f
Camera::viewProjMatrix() const
{
    return viewMatrix() * projMatrix();
}

const Ibl::Vector3f&
Camera::forward() const
{
    return _viewProperty->forwardProperty()->get();
}

FloatProperty*
Camera::fovYProperty()
{
    return _fovYProperty;
}

FloatProperty*
Camera::fovXProperty()
{
    return _fovXProperty;
}

float
Camera::fovY() const
{
    return _fovYProperty->get();
}

FloatProperty*
Camera::aspectRatioProperty()
{
    return _aspectRatioProperty;
}

FloatProperty*
Camera::nearClipProperty()
{
    return _nearClipProperty;
}

FloatProperty*
Camera::farClipProperty()
{
    return _farClipProperty;
}

ViewProperty*
Camera::viewProperty() const
{
    return _viewProperty;
}

ProjectionProperty*
Camera::projectionProperty()
{
    return _projectionProperty;
}

const ProjectionProperty*
Camera::projectionProperty() const
{
    return _projectionProperty;
}

ViewportProperty*
Camera::viewportProperty() const
{
    return _viewportProperty;
}

}
