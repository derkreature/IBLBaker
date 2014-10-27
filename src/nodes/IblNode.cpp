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
#include <IblNode.h>
#include <IblLog.h>
#include <IblProperty.h>

namespace Ibl
{
Node::Node(const std::string& name) :
_name (name)
{
}

Node::~Node()
{
    for (auto  it = _properties.begin(); it != _properties.end(); it++)
    {
        Property* propertyPtr = *it;
        if(propertyPtr)
        {
            delete propertyPtr;
        }
    }
    _properties.erase(_properties.begin(), _properties.end());
}


void
Node::cache (const Property*p)
{
    auto task = _tasks.find(p);
    if (task != _tasks.end())
        task->second(p);

#if _DEBUG
    else
    {
        std::string errorGroup = p->group() ? p->group()->name() : std::string("");
        LOG("No task handler for property " << p->name() << " in group " << errorGroup)
        assert(0);
    }
#endif
}

void
Node::uncache ()
{
    for (auto it = _properties.begin(); it != _properties.end(); it++)
    {
        (*it)->uncache();
    }
    
    for (auto it = _referenceProperties.begin(); it != _referenceProperties.end(); it++)
    {
        (*it)->uncache();
    }
}

const std::string&
Node::name() const
{
    return _name;
}

void
Node::setName (const std::string& name)
{
    _name = name;
}

const Property*
Node::property (const std::string& name) const
{
    return nullptr;
}

Property*
Node::property (const std::string& name)
{
    return nullptr;
}

void
Node::addProperty (Property* property, PropertyOwnership type)
{
    if (type == PropertyOwner)
    {
        if (_properties.find (property) == _properties.end())
        {
            _properties.insert (property);
        }
    }
    else
    {
        if (_referenceProperties.find (property) == _referenceProperties.end())
        {
            _referenceProperties.insert (property);
        }
    }
}

void
Node::removeProperty (Property* property, PropertyOwnership type)
{
    if (type == PropertyOwner)
    {
        auto it = _properties.find (property);
        if (it != _properties.end())
        {
            _properties.erase(it);
        }
     }
     else
     {
        auto it = _referenceProperties.find(property);
        if (it != _referenceProperties.end())
        {
            _referenceProperties.erase (it);
        }
     }
}

}