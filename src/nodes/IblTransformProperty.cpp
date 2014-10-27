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
#include <IblTransformProperty.h>
#include <IblLog.h>
#include <IblMath.h>

using namespace Ibl;

namespace Ibl
{
TransformProperty::TransformProperty(Node* node, const std::string& name) : 
    Property (node, name),
    _translationDependency (nullptr),
    _rotationDependency (nullptr),
    _scaleDependency (nullptr),
    _parentDependency (nullptr),
    _drivenDependency (nullptr)
{
    _preWorldProperty = new MatrixProperty(this, std::string("preWorldProperty"), this);
    _worldProperty = new MatrixProperty(this, std::string("worldProperty"), this);

    using std::placeholders::_1;
    _tasks.insert(std::make_pair(_worldProperty, std::bind(&TransformProperty::computeWorld, this, _1)));
    _tasks.insert(std::make_pair(_preWorldProperty, std::bind(&TransformProperty::computePreWorld, this, _1)));
}

TransformProperty::~TransformProperty()
{
}

void
TransformProperty::addDependency (Property* p, size_t dependencyId)
{
    switch (((TransformProperty::DependencyId)dependencyId))
    {
        case Parent:
            _parentDependency = dynamic_cast<const MatrixProperty*>(p);
            break;
        case Driven:
            _drivenDependency = dynamic_cast<const MatrixProperty*>(p);
            break;        
        case Translation:
            _translationDependency = dynamic_cast<const VectorProperty*>(p);
            break;        
        case Rotation:
            _rotationDependency = dynamic_cast<const VectorProperty*>(p);
            break;        
        case Scale:
            _scaleDependency = dynamic_cast<const VectorProperty*>(p);
            break;
    }

    Property::addDependency(p, dependencyId);
}

void
TransformProperty::removeDependency(Property* p, size_t dependencyId)
{
     switch (((TransformProperty::DependencyId)dependencyId))
    {
        case Parent:
            _parentDependency = nullptr;
            break;
        case Driven:
            _drivenDependency = nullptr;
            break;
        case Translation:
            _translationDependency = nullptr;
            break;        
        case Rotation:
            _rotationDependency = nullptr;
            break;        
        case Scale:
            _scaleDependency = nullptr;
            break;
    }
    Property::removeDependency(p, dependencyId);
}

void
TransformProperty::computeWorld (const Property* property) const
{
    Ibl::Matrix44f world;
    if (_drivenDependency)
    {
        memcpy(&world, &_drivenDependency->get(), sizeof (Ibl::Matrix44f));
        world = _preWorldProperty->get() * world;
    }
    else
    {
        world = _preWorldProperty->get();
    }

    if (_parentDependency)
    {
        world = world * _parentDependency->get();
    }
    _worldProperty->set(world);
}

void
TransformProperty::computePreWorld(const Property* property) const
{
    Ibl::Matrix44f world;
    {
        Ibl::Vector3f translation = _translationDependency->get();
        Ibl::Vector3f rotation = _rotationDependency->get();
        Ibl::Vector3f scale = _scaleDependency->get();

        Ibl::Matrix44f rotationMatrix;
        Ibl::Matrix44f translationMatrix;
        Ibl::Matrix44f scaleMatrix;
        scaleMatrix.scaling(scale);

        translationMatrix.setTranslation(translation);
        Quaternionf quat(rotation.x*RAD, rotation.y *RAD, rotation.z*RAD);
        rotationMatrix.rotation(quat);
        world = scaleMatrix * rotationMatrix * translationMatrix;
    }
    _preWorldProperty->set(world);
}

const MatrixProperty*
TransformProperty::worldProperty() const
{
    return _worldProperty;
}

MatrixProperty*
TransformProperty::worldProperty()
{
    return _worldProperty;
}

}