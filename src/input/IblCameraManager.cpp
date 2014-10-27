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

#include <IblCameraManager.h>
#include <IblDampenedInput.h>
#include <IblFocusedInput.h>
#include <IblFocusedMovement.h>
#include <IblCamera.h>
#include <IblScene.h>
#include <IblInputState.h>

namespace Ibl
{
FocusedDampenedCamera::FocusedDampenedCamera(Ibl::InputState* inputState) :
    _inputState(inputState),
    _movementPolicy(nullptr),
    _inputPolicy(nullptr)
{
}

FocusedDampenedCamera::~FocusedDampenedCamera()
{
    safedelete(_movementPolicy);
    safedelete(_inputPolicy);
    _inputState = nullptr;
    _scene = nullptr;
}

//-----------------------------------------------------------
// Initializes SensateCamera Managment
// Checks that passed in pointers are valid before assigning.
//-----------------------------------------------------------
void 
FocusedDampenedCamera::create (Scene* scene)
{
    _scene = scene;
    _movementPolicy = new Ibl::FocusedMovement();
    _inputPolicy = new Ibl::DampenedInput(_inputState);
}

//-----------------------------------------------------------------------
// Sets the current SensateCamera position rotation
// the exact behaviour of this variable is defined by the movement policy
//-----------------------------------------------------------------------
void 
FocusedDampenedCamera::setRotation (Ibl::Vector3f& rotation)
{
    _rotation = rotation;
    _movementPolicy->update (0, _scene->camera(), _translation, _rotation);
}

//-----------------------------------------------------------------------
// Sets the current SensateCamera position translation
// the exact behaviour of this variable is defined by the movement policy
//-----------------------------------------------------------------------
void 
FocusedDampenedCamera::setTranslation (Ibl::Vector3f& translation)
{        
    _translation = translation;
    _movementPolicy->update (0, _scene->camera(), _translation, _rotation);
    _translation = Ibl::Vector3f(0,translation.y,0); // y is accumulated.
}


bool 
FocusedDampenedCamera::update (float elapsedTime, 
                               bool updateCameraInput, 
                               bool forceUpdate)
{ 
    if (_inputPolicy->update (elapsedTime, _translation, 
                              _rotation, updateCameraInput) ||
                              forceUpdate)
    { 
    _movementPolicy->update (elapsedTime,  
                             _scene->camera(), 
                             _translation,  
                             _rotation);

    }

    return false;
}

DampenedInput* 
FocusedDampenedCamera::inputPolicy()
{
    return _inputPolicy;
}

FocusedMovement* 
FocusedDampenedCamera::movementPolicy()
{
    return _movementPolicy;
}

}