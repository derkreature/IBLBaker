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
#include <IblProperty.h>
#include <IblNode.h>
#include <IblLog.h>
#include <Iblimgui.h>

namespace Ibl
{
EnumTweakType::EnumTweakType(ImguiEnumVal* enumValues,
                             uint32_t enumCount, 
                             const std::string& typeName) :
    _enumCount(enumCount),
    _typeName(typeName),
    _enumValues(nullptr)
{
    assert(enumValues);
    _enumValues = (ImguiEnumVal*)malloc(sizeof(ImguiEnumVal) * enumCount);
    memcpy(_enumValues, enumValues, sizeof(ImguiEnumVal) * enumCount);
}

EnumTweakType::~EnumTweakType()
{
    if (_enumValues)
        free(_enumValues);
}

uint32_t
EnumTweakType::enumCount() const
{
    return _enumCount;
}

const ImguiEnumVal*
EnumTweakType::enumValues() const
{
    return _enumValues;
}

const std::string& 
EnumTweakType::typeName() const
{
    return _typeName;
}

TweakFlags::TweakFlags() :
    minValue(0),
    maxValue(0),
    step(0),
    enumType(nullptr)
{
}

TweakFlags::~TweakFlags()
{
}

TweakFlags::TweakFlags(float minValueIn, float maxValueIn, float stepIn, const std::string& categoryIn) :
    minValue(minValueIn),
    maxValue(maxValueIn),
    step(stepIn),
    category(categoryIn),
    enumType(nullptr)
{
}

TweakFlags::TweakFlags(const EnumTweakType* enumTypeIn, const std::string& categoryIn) :
    minValue(0),
    maxValue(0),
    step(1.0),
    category(categoryIn),
    enumType(enumTypeIn)
{
}


Property::Property(Node* node, 
                   const std::string& name, 
                   Node* group) : 
    Node (name),
    _node (node), /* Container of property */
    _group (group), /* group for property */
    _cached (false),
    _tweakFlags(nullptr)
{
    _node->addProperty (this);
}

Property::Property(Node* node,
                   const std::string& name,
                   TweakFlags* tweakFlags)  : 
    Node(name),
    _node(node), /* Container of property */
    _group(nullptr), /* group for property */
    _cached(false),
    _tweakFlags(tweakFlags)
{
    _node->addProperty(this);
}

Property::~Property()
{
    safedelete(_tweakFlags);
}

const TweakFlags*
Property::tweakFlags() const
{
    return _tweakFlags;
}

bool
Property::cached() const
{
    return _cached;
}

const Node* 
Property::group() const
{
    return _node;
}

Node*
Property::group()
{
    return _node;
}

const Node*
Property::node() const
{
    return _node;
}

Node*
Property::node()
{
    return _node;
}

void
Property::uncache ()
{
    _cached = false;
    Node::uncache();
}

void
Property::removeDependency(Property* p, size_t dependencyId)
{
    auto it = _dependencies.find (p);
    if (it != _dependencies.end())
    {
        _dependencies.erase(it);
        p->removeProperty(this, PropertyReference);
    }
    uncache();
}

void
Property::addDependency(Property* p, size_t dependencyId)
{
    if (_dependencies.find (p) == _dependencies.end())
    {
        // Add the property to the dependency list.
        _dependencies.insert (p);
        p->addProperty(this, PropertyReference);
    }

    _cached = false;
    Node::uncache();
}

}