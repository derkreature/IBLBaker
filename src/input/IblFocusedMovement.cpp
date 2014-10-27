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

#include <IblFocusedMovement.h>
#include <IblViewProperty.h>
#include <IblMatrixAlgo.h>

namespace Ibl
{

FocusedMovement::FocusedMovement():
_focusOffset(0, 0, 0),
_minPosition(0, 0, 5),
_maxPosition(0, 0, 400),
_maxDistanceFromCenter(600),
_focusPosition (0,0,0)
{
}

float& 
FocusedMovement::maxDistanceFromCenter()
{
    return _maxDistanceFromCenter;
}

void 
FocusedMovement::maxDistanceFromCenter(float& distance)
{
    _maxDistanceFromCenter = distance;
}

Ibl::Vector3f& 
FocusedMovement::maxPosition()
{
    return _maxPosition;
}

void 
FocusedMovement::maxPosition(const Ibl::Vector3f& maxPosition)
{
    _maxPosition = maxPosition;
}

Ibl::Vector3f& 
FocusedMovement::minPosition()
{
    return _minPosition;
}

void 
FocusedMovement::minPosition(const Ibl::Vector3f& minPosition)
{
    _minPosition = Ibl::Vector3f(minPosition.x, minPosition.y, minPosition.z);
}

void 
FocusedMovement::setFocusPosition (const Ibl::Vector3f& position)
{
    _focusPosition = position;
}

void 
FocusedMovement::setFocusOffset(const Ibl::Vector3f& focusoffset)
{
    _focusOffset = focusoffset;
}

void 
FocusedMovement::update (float elapsedtime, 
                         Camera* camera, 
                         Ibl::Vector3f& translation, 
                         Ibl::Vector3f& rotation)
{
    if (camera)
    {
        Ibl::Vector3f orientationExtents = Ibl::Vector3f(0, 0, translation.y);
                
        // rotate orientationExtents by whatever the current rotation z and rotation y is.		
        Ibl::Matrix44f rotx;
        Ibl::Matrix44f rotz;
        Ibl::Matrix44f rotationMatrix;
        Ibl::Vector3f  translationDelta = Ibl::Vector3f(translation.x, translation.z, 0);
        Ibl::Quaternionf r = camera->viewProperty()->rotationQuaternionProperty()->get();
        translationDelta = Ibl::vecQuatTransform(translationDelta, r);

        _focusOffset += translationDelta;

        rotx.setRotationX (-rotation.y * (RAD));
        rotz.setRotationY (rotation.z * (RAD));
        rotationMatrix = rotx * rotz;

        Ibl::Vector3f cameraRotation = camera->rotationProperty()->get();
        cameraRotation.x = -rotation.y;
        cameraRotation.y = rotation.z;
        camera->rotationProperty()->set(cameraRotation);

        Ibl::Vector3f cameraTranslation = 
            rotationMatrix.transform (orientationExtents);

        cameraTranslation =  _focusPosition + cameraTranslation + _focusOffset;

        camera->translationProperty()->set (cameraTranslation);
    }
}

}