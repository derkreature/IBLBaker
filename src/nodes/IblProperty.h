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
#ifndef INCLUDED_BB_PROPERTY
#define INCLUDED_BB_PROPERTY

#include <IblPlatform.h>
#include <IblNode.h>
#include <IblNonCopyable.h>
#include <AntTweakBar.h>
#include <functional>

namespace Ibl
{
class EnumTweakType
{
  private:
    NON_COPYABLE(EnumTweakType)

  public:
    EnumTweakType(TwEnumVal* enumVal, uint32_t enumCount, const std::string& typeName);
    virtual ~EnumTweakType();

    TwType                     type() const;
    const std::string &        typeName() const;

  protected:
    TwEnumVal*                 _enumValues;
    uint32_t                   _enumCount;
    std::string                _typeName;
    mutable TwType             _type;
};

class TweakFlags
{
  private:
    NON_COPYABLE(TweakFlags)

  public:
    TweakFlags();
    ~TweakFlags();
    TweakFlags(float minValue, float maxValue, float step, const std::string& category);
    TweakFlags(const EnumTweakType* enumTypeIn, const std::string& category);

    float                      minValue;
    float                      maxValue;
    float                      step;
    std::string                category;
    // Optional
    const EnumTweakType*       enumType;
};

class Node;
class Property : public Node
{
  private:
    NON_COPYABLE(Property)

  public:
    Property (Node* node,
              const std::string& name, 
              Node* group = nullptr);

    Property(Node* node,
             const std::string& name,
             TweakFlags* tweakFlags);

    virtual ~Property ();

    virtual void               uncache ();

    const Node*                node() const;
    Node*                      node();

    const Node*                group() const;
    Node*                      group();

    const TweakFlags*          tweakFlags() const;

    void                       removeDependency(Property* p, size_t dependencyId);
    void                       addDependency(Property* p, size_t dependencyId);

  protected:
    bool                       cached() const;

  protected:
    std::set<const Property*>  _dependencies;
    Node*                      _node;
    Node*                      _group;
    TweakFlags*                _tweakFlags;
    bool                       _cached;
};
}

#endif