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

#include <IblDampenedInput.h>
#include <IblLog.h>
namespace Ibl
{
DampenedInput::DampenedInput(InputState* state) :
_inputState(state),
_lastTranslation(0,0,0),
_lastRotation(0,0,0),
_elapsedTime(0)
{
}

DampenedInput::~DampenedInput()
{
}

bool 
DampenedInput::updateRotation (Ibl::Vector3f& rotate)
{
    if (_elapsedTime > 0)
    {
        if (_inputState->leftMouseDown() &&
            !_inputState->hasGUIFocus() &&
            !_inputState->getKeyState(DIK_LCONTROL))
        {    
            rotate += Ibl::Vector3f (0, ((float)-_inputState->_y * 0.2f), ((float)-_inputState->_x * 0.2f));

            _lastRotation.x = -((_lastRotation.x) + -(_inputState->_x * 0.15f) / (_elapsedTime) )/ 2.0f;
            _lastRotation.y = (_lastRotation.y + ((-(_inputState->_y  * 0.15f) / (_elapsedTime ) ))) / 2.0f;     

            return true;
        }
        else if (fabsf(_inputState->rotation.x) > 0 || fabsf(_inputState->rotation.y) > 0)
        {
            rotate += Ibl::Vector3f (0, ((float)_inputState->rotation.x * 0.15f), ((float)_inputState->rotation.y * 0.15f));

            _lastRotation.x = -((_lastRotation.x) + -(_inputState->_x * 0.15f) / (_elapsedTime)) / 2.0f;
            _lastRotation.y = (_lastRotation.y + ((-(_inputState->_y  * 0.15f) / (_elapsedTime)))) / 2.0f;

            return true;        
        }    
    }

    return false;
}

bool 
DampenedInput::updateTranslation (Ibl::Vector3f& translation)
{
    bool buser = false;
    if (_elapsedTime > 0)
    {
        if (_inputState != 0 && !_inputState->hasGUIFocus())
        {
            if (_inputState->getKeyState(DIK_W))
            {
                translation.y += 55 * _elapsedTime;
                _lastTranslation.y = -55;    
                buser = true;
            }

            if (_inputState->getKeyState(DIK_S))
            {    
                translation.y += -55 * _elapsedTime;
                _lastTranslation.y = 55;
                buser = true;
            }

            translation.y += (_elapsedTime * 4.0f) * _inputState->_z;
            if (_inputState->_z != 0)
            {
                _lastTranslation.y =  (float)(0.05f * -_inputState->_z );    
                buser = true;
            }

            if (fabsf(_inputState->translation.z) != 0)
            {
                buser = true;
            }
        

            if (_inputState->rightMouseDown() && !_inputState->hasGUIFocus())
            {
                translation.x = (-_inputState->_x * 0.25f);
                translation.z = _inputState->_y * 0.25f;
                buser = true;
            }
            else
            {
                translation.x = 0;
                translation.z = 0;
                buser = true;
            }
        }
    }

    return buser;
}

void 
DampenedInput::continueTranslation (float& lastTranslation, 
                                    float& translation)
{
    float dampen = lastTranslation * _elapsedTime;
    translation -= dampen;
    translation = 0;  
}

void 
DampenedInput::continueRotation (float &lastRotation, 
                                 float &rotate, 
                                 float dampen)
{
    if (lastRotation != 0)
    {
        rotate -= dampen;
        float decfact = ((lastRotation * 3)) * _elapsedTime;

        if (lastRotation > 0)
        {
            lastRotation - (decfact) < 0 ? lastRotation = 0 : lastRotation -= (decfact );
        }
        else
        {
            lastRotation + decfact> 0 ? lastRotation = 0 : lastRotation -= (decfact );
        }
    }
}

bool 
DampenedInput::update (float elapsedTime, 
                       Ibl::Vector3f& translation, 
                       Ibl::Vector3f& rotate, 
                       bool& updatecamerainput)
{
    _elapsedTime = elapsedTime;

    Ibl::Vector3f ttrans  = translation;
    Ibl::Vector3f trotate = rotate;

    bool userRotated = false;
    bool userTranslated = false;


    userRotated = updateRotation(rotate);
    userTranslated = updateTranslation(translation);
   

    if (!userRotated )
    {
        continueRotation (_lastRotation.x, rotate.z, _lastRotation.x * _elapsedTime);
        continueRotation (_lastRotation.y, rotate.y, -(_lastRotation.y * _elapsedTime));            
    }

    bool translationEqual = (ttrans == translation);
    bool rotationEqual = (trotate == rotate);
    if ((translationEqual == false) || (rotationEqual == false))
    {
        return true;
    }
    else
    {
        return false;
    }

}

}