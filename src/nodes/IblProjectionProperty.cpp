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
#include <IblProjectionProperty.h>
#include <IblMatrixAlgo.h>

namespace Ibl
{

ProjectionProperty::ProjectionProperty(Ibl::Node* node, const std::string& name) : 
Property (node, name),
_viewWidthDependency (nullptr),
_viewHeightDependency (nullptr),
_fovYDependency (nullptr),
_fovXDependency (nullptr),
_aspectRatioDependency (nullptr),    
_nearClipDependency (nullptr),    
_farClipDependency (nullptr),
_focalLengthDependency (nullptr),
_useDualPerspectiveDependency (nullptr)
{
    _projectionProperty = new MatrixProperty (this, std::string("projection"), this);

    using std::placeholders::_1;
    _tasks.insert(std::make_pair(_projectionProperty, std::bind(&ProjectionProperty::computeProjection, this, _1)));
}

ProjectionProperty::~ProjectionProperty()
{
}

void
ProjectionProperty::computeProjection(const Property* p) const
{
    Matrix44f projection;
    if (_useDualPerspectiveDependency->get())
    {
        Ibl::projectionPerspectiveMatrixLH(_fovYDependency->get(),
            _fovXDependency->get(),
            _aspectRatioDependency->get(),
            _nearClipDependency->get(),
            _farClipDependency->get(),
            &projection);
    }
    else
    {
        Ibl::projectionPerspectiveMatrixLH(_fovYDependency->get(),
            _aspectRatioDependency->get(),
            _nearClipDependency->get(),
            _farClipDependency->get(),
            &projection);
    }
    _projectionProperty->set(projection);
}

void
ProjectionProperty::addDependency(Property* p, size_t dependencyId)
{
    switch ((ProjectionProperty::DependencyId)(dependencyId))
    {
        case ProjectionProperty::FovY:
            _fovYDependency = dynamic_cast<const FloatProperty*>(p);
            break;
        case ProjectionProperty::FovX:
            _fovXDependency = dynamic_cast<const FloatProperty*>(p);
            break;
        case ProjectionProperty::DualPerspective:
            _useDualPerspectiveDependency = dynamic_cast<const BoolProperty*>(p);
            break;
        case ProjectionProperty::AspectRatio:
            _aspectRatioDependency = dynamic_cast<const FloatProperty*>(p);
            break;
        case ProjectionProperty::NearClip:
            _nearClipDependency = dynamic_cast<const FloatProperty*>(p);
            break;
        case ProjectionProperty::FarClip:
            _farClipDependency = dynamic_cast<const FloatProperty*>(p);
            break;
        case ProjectionProperty::FocalLength:
            _focalLengthDependency = dynamic_cast<const FloatProperty*>(p);
            break;
        case ProjectionProperty::ViewWidth:
            _viewWidthDependency = dynamic_cast<const FloatProperty*>(p);
            break;
        case ProjectionProperty::ViewHeight:
            _viewHeightDependency = dynamic_cast<const FloatProperty*>(p);
            break;
    }
    Property::addDependency(p, dependencyId);
}

void
ProjectionProperty::removeDependency(Property* p, size_t dependencyId)
{
    switch ((ProjectionProperty::DependencyId)(dependencyId))
    {
        case FovY:
            _fovYDependency = nullptr;
            break;
        case FovX:
            _fovXDependency = nullptr;
            break;
        case DualPerspective:
            _useDualPerspectiveDependency = nullptr;
            break;
        case AspectRatio:
            _aspectRatioDependency = nullptr;
            break;
        case NearClip:
            _nearClipDependency = nullptr;
            break;
        case FarClip:
            _farClipDependency = nullptr;
            break;
        case FocalLength:
            _focalLengthDependency = nullptr;
            break;
        case ViewWidth:
            _viewWidthDependency = nullptr;
            break;
        case ViewHeight:
            _viewHeightDependency = nullptr;
            break;
    }

    Property::removeDependency(p, dependencyId);
}

const MatrixProperty*
ProjectionProperty::projMatrixProperty() const
{
    return _projectionProperty;
}

}
