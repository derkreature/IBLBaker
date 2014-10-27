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

#include <IblInputState.h>
#include <IblLog.h>

namespace Ibl
{
Gamepad::Gamepad() :
_leftThumbX(0),
_leftThumbY(0),
_rightThumbX(0),
_rightThumbY(0),
_controllerLeftTriggerValue(0),
_controllerRightTriggerValue(0),
_hatLeftTriggerValue(0),
_hatRightTriggerValue(0),
_controllerAButton(0),
_controllerBButton(0),
_controllerXButton(0),
_controllerYButton(0)
{
}

void
Gamepad::makeIdentity()
{
     _leftThumbX= 0;
     _leftThumbY= 0;
     _rightThumbX= 0;
     _rightThumbY= 0;
     _controllerLeftTriggerValue= 0;
     _controllerRightTriggerValue= 0;
     _hatLeftTriggerValue= 0;
     _hatRightTriggerValue= 0;
     _controllerAButton= 0;
     _controllerBButton= 0;
     _controllerXButton= 0;
     _controllerYButton= 0;
}

InputState::InputState()
{
    memset (&_persistKey[0], 0, sizeof(DWORD) * 512);
    _x = 0;
    _y = 0;
    _z = 0;

    memset (_didodkb, 0, sizeof(DIDEVICEOBJECTDATA) * 32);
    memset (_didodms, 0, sizeof(DIDEVICEOBJECTDATA) * 32);

    _leftMouseDown   = false;
    _rightMouseDown  = false;
    _middleMouseDown = false;

    _hasGuiFocus = false;
    rotation = Ibl::Vector3f (0,0,0);
    translation = Ibl::Vector3f (0,0,0);

    _keyboardElements = 0;
    _mouseElements = 0;
    _elapsedTime = 0;
}

InputState::~InputState()
{
}

bool InputState::getKeyState (DWORD key)
{
    //returns the correct key state for the persistant keys...
    //and the mouse buttons
    if(key >= 256 && key <= 259)
    {
        for (uint32_t keystroke = 0; keystroke < (uint32_t)_mouseElements; keystroke++)
        {
            if (_didodms[keystroke].dwOfs == key-256+DIMOFS_BUTTON0)
            {
                if(_didodms[keystroke].dwData & 0x80)
                    return true;
                else
                    return false;
            }
        }
        return false;
    }
    else
    {
        return (_persistKey[key] & 0x80) != 0;
    }
}

void InputState::updateRepeat(DWORD key, DWORD state)
{
    _repeated [key].update (state);
}

bool InputState::buttonOrKeyPressed()
{
    if(_keyboardElements > 0)
    {
        for(uint32_t keystroke = 0; keystroke < _keyboardElements; keystroke++)
        {
            if(_didodkb[keystroke].dwData & 0x80)
            {
                return true; 
            }
        }
    }
    
    if(_mouseElements > 0)
    {
        for (uint32_t buttonPress = 0; 
             buttonPress < _mouseElements; 
             buttonPress++)
        {
            if (_didodms[buttonPress].dwData & 0x80)
            {
                switch(_didodms[buttonPress].dwOfs)
                {
                case DIMOFS_BUTTON0:
                case DIMOFS_BUTTON1:
                case DIMOFS_BUTTON2:
                case DIMOFS_BUTTON3:
                    return true;
                }
            }
        }
    }
    return false;
}

bool InputState::middleMouseDown()
{
    return _middleMouseDown;
}
bool InputState::leftMouseDown()
{
    return  _leftMouseDown;
}

bool InputState::rightMouseDown()
{
    return  _rightMouseDown;
}

void InputState::update()
{
    _x = 0;
    _y = 0;
    _z = 0;

    if(_mouseElements > 0)
    {
        for(uint32_t keystroke = 0; keystroke < _mouseElements; keystroke++)
        {
            switch(_didodms[keystroke].dwOfs)
            {
            case DIMOFS_X:
                    _x += (int)_didodms[keystroke].dwData;            
                    break;
            case DIMOFS_Y:
                    _y += (int)_didodms[keystroke].dwData;
                     break;
            case DIMOFS_Z:
                    _z += (int)_didodms[keystroke].dwData;
                    break;
            case DIMOFS_BUTTON0:
                    _leftMouseDown = (_didodms[keystroke].dwData & 0x80) != 0;
                    break;
            case DIMOFS_BUTTON1:
                    _rightMouseDown = (_didodms[keystroke].dwData & 0x80) != 0;
                    break;
            case DIMOFS_BUTTON2:
                    _middleMouseDown = (_didodms[keystroke].dwData & 0x80) != 0;
                    break;
                break;
            case DIMOFS_BUTTON3:
                    break;            
            default:
                    break;
            }
        }
    }
}

}