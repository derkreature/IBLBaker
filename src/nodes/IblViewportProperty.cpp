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
#include <IblViewportProperty.h>
#include <IblISurface.h>

namespace Ibl
{
ViewportProperty::ViewportProperty(Node* node, const std::string& name) : 
    Property (node, name),
_widthDependency (nullptr),
_heightDependency (nullptr),
_deviceDependency (nullptr)
{
    _totalRegionProperty = new ViewportTypedProperty(this, std::string("total"), this);

    using std::placeholders::_1;
    _tasks.insert(std::make_pair(_totalRegionProperty, std::bind(&ViewportProperty::computeTotalRegion, this, _1)));
}

ViewportProperty::~ViewportProperty()
{
}

const ViewportTypedProperty*
ViewportProperty::totalRegionProperty() const
{
    return _totalRegionProperty;
}

void
ViewportProperty::computeTotalRegion(const Property* p)
{
    if (IDevice* device = _deviceDependency->get())
    {
        Viewport viewport;
        float width = _widthDependency->get();
        float height = _heightDependency->get();

        float screenWidth = (float)(device->backbuffer()->width());
        float screenHeight = (float)(device->backbuffer()->height());

        viewport._x = 0;
        viewport._y = 0;
        viewport._minZ = 0;
        viewport._maxZ = 1;
        viewport._width = (float)(screenWidth);
        viewport._height = (float)(screenHeight);
        _totalRegionProperty->set(viewport);
        
    }
}

void
ViewportProperty::addDependency(Property* p, size_t dependencyId)
{
    switch ((ViewportProperty::DependencyId)(dependencyId))
    {
        case ViewportProperty::Width:
            _widthDependency = dynamic_cast<const FloatProperty*>(p);
            break;
        case ViewportProperty::Height:
            _heightDependency = dynamic_cast<const FloatProperty*>(p);
            break;
        case ViewportProperty::Device:
            _deviceDependency = dynamic_cast<const DeviceProperty*>(p);
            break;
    }
    Property::addDependency(p, dependencyId);
}

void
ViewportProperty::removeDependency(Property* p, size_t dependencyId)
{
    switch ((ViewportProperty::DependencyId)(dependencyId))
    {
        case ViewportProperty::Width:
            _widthDependency = nullptr;
            break;
        case ViewportProperty::Height:
            _heightDependency = nullptr;
            break;
        case ViewportProperty::Device:
            _deviceDependency = nullptr;
            break;
    }
    Property::removeDependency(p, dependencyId);
}

}
