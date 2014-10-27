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
#ifndef INCLUDED_BB_VIEW_PROPERTY
#define INCLUDED_BB_VIEW_PROPERTY

#include <IblTypedProperty.h>

namespace Ibl
{
class ViewProperty : public Property
{
  public:
    ViewProperty(Node* node, 
                 const std::string& name);
    virtual ~ViewProperty();

    MatrixProperty*            viewMatrixProperty();
    VectorProperty*            lookAtProperty();
    VectorProperty*            forwardProperty();
    VectorProperty*            upProperty();
    VectorProperty*            rightProperty();
    QuaternionProperty*        rotationQuaternionProperty();

    enum DependencyId
    {
        Rotation,
        Translation,
        UseBakedForwards,
        BakedForwards,
        BakedUp
    };

    virtual void                addDependency(Property* p, size_t dependencyId);
    virtual void                removeDependency(Property* p, size_t dependencyId);

  private:
    void                        computeView(const Property* p);
    void                        computeLookAt(const Property* p);
    void                        computeRight(const Property* p);
    void                        computeUp(const Property* p);
    void                        computeForward(const Property* p);
    void                        computeRotation(const Property* p);

    VectorProperty*             _rotationDependency;
    VectorProperty*             _translationDependency;

    BoolProperty*               _useBakedUpForwardsDependency;
    VectorProperty*             _bakedUpDependency;
    VectorProperty*             _bakedForwardsDependency;

    MatrixProperty*             _viewMatrixProperty;
    MatrixProperty*             _yUpViewMatrixProperty;
    VectorProperty*             _lookAtProperty;
    VectorProperty*             _forwardProperty;
    VectorProperty*             _upProperty;
    VectorProperty*             _rightProperty;
    QuaternionProperty*         _rotationQuatProperty;

    const Ibl::Vector3f          _up;
    const Ibl::Vector3f          _right;
    const Ibl::Vector3f          _forward;

};
}

#endif