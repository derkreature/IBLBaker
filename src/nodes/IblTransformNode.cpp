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
#include <IblTransformNode.h>
#include <IblTransformProperty.h>

namespace Ibl
{
TransformNode::TransformNode(Ibl::IDevice* device) : 
RenderNode(device), 
_parent (nullptr) 
{
    _translationProperty = new VectorProperty (this, std::string("Translation"));

    _rotationProperty = new VectorProperty(this, std::string("Rotation"), new TweakFlags(0.0f, 360.0f, 1e-6f, "Transforms"));
    _scaleProperty = new VectorProperty (this, std::string("Scale"));

    _worldTransformProperty = new TransformProperty(this, std::string("Transform"));
    _deviceProperty = new DeviceProperty (this,    std::string ("device"));
    _lastWorldTransformProperty = new MatrixProperty(this, std::string("lastWorld"));

    _deviceProperty->set (device);

    _worldTransformProperty->addDependency(_translationProperty, TransformProperty::Translation);
    _worldTransformProperty->addDependency (_rotationProperty, TransformProperty::Rotation);
    _worldTransformProperty->addDependency (_scaleProperty, TransformProperty::Scale);

    _translationProperty->set (Ibl::Vector3f(0.0f,0.0f,0.0f));
    _rotationProperty->set (Ibl::Vector3f(0.0f,0.0f,0.0f));
    _scaleProperty->set (Ibl::Vector3f(1.0f,1.0f,1.0f));
}

TransformNode::~TransformNode()
{
}

void
TransformNode::cacheLastWorldTransform() const
{
    _lastWorldTransformProperty->set(worldTransform());
}

MatrixProperty*
TransformNode::lastWorldTransformProperty() const
{
    return _lastWorldTransformProperty;
}

const Ibl::Matrix44f&
TransformNode::worldTransform() const
{
    return _worldTransformProperty->worldProperty()->get();
}

const Ibl::Vector3f&
TransformNode::translation() const
{
    return _translationProperty->get();
}

Ibl::Vector3f
TransformNode::worldTranslation() const
{
   const Ibl::Matrix44f& transform = 
       worldTransformProperty()->worldProperty()->get();
   return transform.translation();
}

const Ibl::Vector3f&
TransformNode::rotation()  const
{
    return _rotationProperty->get();
}

const Ibl::Vector3f&
TransformNode::scale () const
{
    return _scaleProperty->get();
}

const TransformProperty*    
TransformNode::worldTransformProperty() const
{
    return _worldTransformProperty;
}

const VectorProperty*    
TransformNode::translationProperty() const
{
    return _translationProperty;
}

const VectorProperty*    
TransformNode::scaleProperty() const
{
    return _scaleProperty;
}

const VectorProperty*    
TransformNode::rotationProperty() const
{
    return _rotationProperty;
}

VectorProperty*    
TransformNode::rotationProperty()
{
    return _rotationProperty;
}

VectorProperty*    
TransformNode::scaleProperty()
{
    return _scaleProperty;
}

TransformProperty*
TransformNode::worldTransformProperty()
{
    return _worldTransformProperty;
}

VectorProperty*
TransformNode::translationProperty()
{
    return _translationProperty;
}

const std::vector <Ibl::TransformNode*> &
TransformNode::children() const
{
    return _entities;
}

void
TransformNode::setParent(Ibl::TransformNode* parent)
{
    if (_parent)
    {
        worldTransformProperty()->removeDependency (_parent->worldTransformProperty()->worldProperty(), Ibl::TransformProperty::Parent);
    }
    if (parent)
    {
        worldTransformProperty()->addDependency (parent->worldTransformProperty()->worldProperty(), Ibl::TransformProperty::Parent);
    }
    _parent = parent;
}

Ibl::TransformNode*
TransformNode::parent()
{
    return _parent;
}

}