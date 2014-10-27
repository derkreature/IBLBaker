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
#include <IblX360Controller.h>
#include <IblInputState.h>
#include <IblVector3.h>
#include <IblLog.h>

namespace Ibl
{
X360Controller::X360Controller()
{
    memset (&_currentInputState, 0, sizeof (XINPUT_STATE));
}

X360Controller::~X360Controller()
{
}

bool
X360Controller::create(Ibl::InputState* state)
{
    _inputState = state;
    return true;
}

void
X360Controller::setMotorSpeeds (uint32_t gamePadIndex, float left, float right)
{
    if (gamePadIndex < MAX_GAME_PAD_COUNT)
    {
        XINPUT_VIBRATION vibration;
        memset( &vibration, 0, sizeof(XINPUT_VIBRATION) );
        vibration.wLeftMotorSpeed = WORD(left * 65535.0f); // use any value between 0-65535 here
        vibration.wRightMotorSpeed = WORD(right * 65535.0f); // use any value between 0-65535 here
        XInputSetState(gamePadIndex, &vibration );
    }
    else
    {
        LOG("Invalid X360 gamepadIndex " << gamePadIndex);
    }
}

bool
X360Controller::update()
{
    uint32_t connectedControllerIndex = 0;
    for (uint32_t gamePadIndex = 0; gamePadIndex < MAX_GAME_PAD_COUNT; gamePadIndex++)
    {
        Gamepad& gamepad = _inputState->_gamepads[connectedControllerIndex];
        memset (&_currentInputState, 0, sizeof (XINPUT_STATE)); 

        if (XInputGetState(gamePadIndex, &_currentInputState) == ERROR_SUCCESS)
        {
            gamepad._controllerLeftTriggerValue =
                (float)_currentInputState.Gamepad.bLeftTrigger / 256.0f;
            gamepad._controllerRightTriggerValue =
                (float)_currentInputState.Gamepad.bRightTrigger / 256.0f;

            gamepad._hatLeftTriggerValue =
                (float)((_currentInputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ? 1 : 0);
            gamepad._hatRightTriggerValue =
                (float)((_currentInputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? 1 : 0);

            gamepad._controllerAButton = 
                (float)((_currentInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A) ? 1 : 0);
            gamepad._controllerBButton = 
                (float)((_currentInputState.Gamepad.wButtons & XINPUT_GAMEPAD_B) ? 1 : 0);
            gamepad._controllerXButton = 
                (float)((_currentInputState.Gamepad.wButtons & XINPUT_GAMEPAD_X) ? 1 : 0);
            gamepad._controllerYButton = 
                (float)((_currentInputState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) ? 1 : 0);

            float x = 0;
            float y = 0;

            if( (_currentInputState.Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && 
                 _currentInputState.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) && 
                (_currentInputState.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && 
                _currentInputState.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) )
            {    
               x = 0;
               y = 0;
            }
            else
            {
                x = (float)_currentInputState.Gamepad.sThumbRX / 
                    (float)MAX_THUMBSTICK;
                y = (float)_currentInputState.Gamepad.sThumbRY / 
                    (float)MAX_THUMBSTICK;
            }

            gamepad._rightThumbX = x;
            gamepad._rightThumbY = y;

            // Global states

            if( (_currentInputState.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && 
                 _currentInputState.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) && 
                (_currentInputState.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && 
                _currentInputState.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) )
            {    
               x = 0;
               y = 0;
            }
            else
            {
                x = (float)_currentInputState.Gamepad.sThumbLX / 
                    (float)MAX_THUMBSTICK;
                y = (float)_currentInputState.Gamepad.sThumbLY / 
                    (float)MAX_THUMBSTICK;
            }

            gamepad._leftThumbX = x;
            gamepad._leftThumbY = y;
        
            /*
            _inputState->rotation = 
                Ibl::Vector3f (-(y* 3),
                               (x* 3),
                               0);
                               */
            connectedControllerIndex++;
        }
        else
        {
            gamepad.makeIdentity();
        }
     }
     return true;
}

}
