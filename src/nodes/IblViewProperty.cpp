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
#include <IblViewProperty.h>
#include <IblMatrixAlgo.h>

namespace Ibl
{


ViewProperty::ViewProperty(Node* node, const std::string& name) :
    Property (node, name),
    _up (0, 1, 0),
    _right(1, 0, 0),
    _forward (0, 0, 1),
    _rotationDependency (nullptr),
    _translationDependency (nullptr),
    _useBakedUpForwardsDependency(nullptr),
    _bakedUpDependency(nullptr),
    _bakedForwardsDependency(nullptr)
{
    _viewMatrixProperty = new MatrixProperty (this, std::string("viewMatrix"), this);
    _rightProperty = new VectorProperty (this, std::string("viewMatrix"), this);
    _lookAtProperty = new VectorProperty (this, std::string("lookAt"), this);
    _forwardProperty = new VectorProperty (this, std::string("forward"), this);
    _upProperty = new VectorProperty (this, std::string("up"), this);
    _rotationQuatProperty = new QuaternionProperty (this, std::string("rotation"), this);
    _lookAtProperty = new VectorProperty(this, std::string("lookAt"), this);

    using std::placeholders::_1;
    _tasks.insert(std::make_pair(_viewMatrixProperty, std::bind(&ViewProperty::computeView, this, _1)));
    _tasks.insert(std::make_pair(_lookAtProperty, std::bind(&ViewProperty::computeLookAt, this, _1)));
    _tasks.insert(std::make_pair(_rightProperty, std::bind(&ViewProperty::computeRight, this, _1)));
    _tasks.insert(std::make_pair(_forwardProperty, std::bind(&ViewProperty::computeForward, this, _1)));
    _tasks.insert(std::make_pair(_upProperty, std::bind(&ViewProperty::computeUp, this, _1)));
    _tasks.insert(std::make_pair(_rotationQuatProperty, std::bind(&ViewProperty::computeRotation, this, _1)));
}

ViewProperty::~ViewProperty()
{
}

void
ViewProperty::computeView(const Property* p)
{
    Ibl::Matrix44f view;
    Ibl::Vector3f translation = _translationDependency->get();


    Ibl::Vector3f lookat;
    Ibl::Vector3f up;
    up = _upProperty->get();
    lookat = _lookAtProperty->get();
    Ibl::viewMatrixLH(translation, lookat, up, &view);
    _viewMatrixProperty->set(view);
}

void 
ViewProperty::computeLookAt(const Property* p)
{
    _lookAtProperty->set(_forwardProperty->get() +
        _translationDependency->get());
}

void
ViewProperty::computeRight(const Property* p)
{
    Quaternionf r = _rotationQuatProperty->get();
    Ibl::Vector3f right = Ibl::vecQuatTransform <float>(_right, r);
    _rightProperty->set(right);
}

void
ViewProperty::computeForward(const Property* p)
{
    Quaternionf r = _rotationQuatProperty->get();

    if (!_useBakedUpForwardsDependency->get())
    {
        Ibl::Vector3f forward = Ibl::vecQuatTransform <float>(_forward, r);
        _forwardProperty->set(forward);
    }
    else
    {
        _forwardProperty->set(_bakedForwardsDependency->get());
    }
}

void
ViewProperty::computeUp(const Property* p)
{
    Quaternionf r = _rotationQuatProperty->get();
    if (!_useBakedUpForwardsDependency->get())
    {
        Ibl::Vector3f up = Ibl::vecQuatTransform <float>(_up, r);
        _upProperty->set(up);
    }
    else
    {
        _upProperty->set(_bakedUpDependency->get());
    }
}

void
ViewProperty::computeRotation(const Property* p)
{
    Ibl::Vector3f rotation = _rotationDependency->get();

    float rx = (rotation.x * (RAD));
    float ry = (rotation.y * (RAD));
    float rz = (rotation.z * (RAD));

    _rotationQuatProperty->set(Ibl::Quaternionf(rx, ry, rz));
}

void
ViewProperty::addDependency (Property* p, size_t dependencyId)
{
    switch ((ViewProperty::DependencyId)dependencyId)
    {
      case ViewProperty::Rotation:
          _rotationDependency = dynamic_cast<VectorProperty*>(p);
          break;
      case ViewProperty::Translation:
          _translationDependency = dynamic_cast<VectorProperty*>(p);
          break;
      case ViewProperty::UseBakedForwards:
          _useBakedUpForwardsDependency = dynamic_cast<BoolProperty*>(p);
          break;
      case ViewProperty::BakedForwards:
          _bakedForwardsDependency = dynamic_cast<VectorProperty*>(p);
          break;
      case ViewProperty::BakedUp:
          _bakedUpDependency = dynamic_cast<VectorProperty*>(p);
          break;
    }

    Property::addDependency(p, dependencyId);
}

void
ViewProperty::removeDependency(Property* p, size_t dependencyId)
{
    switch ((ViewProperty::DependencyId)dependencyId)
    {
      case ViewProperty::Rotation:
          _rotationDependency = nullptr;
          break;
      case ViewProperty::Translation:
          _translationDependency = nullptr;
          break;
      case ViewProperty::UseBakedForwards:
          _useBakedUpForwardsDependency = nullptr;
          break;
      case ViewProperty::BakedForwards:
          _bakedForwardsDependency = nullptr;
          break;
      case ViewProperty::BakedUp:
          _bakedUpDependency = nullptr;
          break;
    }

    Property::removeDependency(p, dependencyId);
}

MatrixProperty*    
ViewProperty::viewMatrixProperty()
{
    return _viewMatrixProperty;
}


QuaternionProperty*
ViewProperty::rotationQuaternionProperty()
{
    return _rotationQuatProperty;
}

VectorProperty*
ViewProperty::lookAtProperty()
{
    return _lookAtProperty;
}

VectorProperty*
ViewProperty::forwardProperty()
{
    return _forwardProperty;
}

VectorProperty*
ViewProperty::upProperty()
{
    return _upProperty;
}

VectorProperty*
ViewProperty::rightProperty()
{
    return _rightProperty;
}

}
