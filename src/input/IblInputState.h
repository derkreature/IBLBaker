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

#ifndef IBL_INPUT_STATE
#define IBL_INPUT_STATE

#include <IblPlatform.h>
#include <xinput.h>
#include <mmsystem.h>
#include <IblVector3.h>
#include <IblVector2.h>
#include <IblVector4.h>

namespace Ibl
{
#define MAX_GAME_PAD_COUNT 4

class  Gamepad
{
  public:
    Gamepad();

    void                        makeIdentity();

    float                       _leftThumbX;
    float                       _leftThumbY;
    float                       _rightThumbX;
    float                       _rightThumbY;
    float                       _controllerLeftTriggerValue;
    float                       _controllerRightTriggerValue;
    float                       _hatLeftTriggerValue;
    float                       _hatRightTriggerValue;
    float                       _controllerAButton;
    float                       _controllerBButton;
    float                       _controllerXButton;
    float                       _controllerYButton;
};

class RepeatKey 
{
public:
    RepeatKey(){_lastDown = _lastUp = 0.0f;};
    virtual ~RepeatKey() throw() {};
    
    __inline void update (DWORD state)
    {    
        if (state & 0x80) 
        {    
            _lastDown = (float)(timeGetTime() * 0.001f);
        }
        else
        {
            _lastUp = (float)(timeGetTime() * 0.001f);
        }
    };
    
    __inline bool isRepeated (float margin)
    {
        if (_lastDown - _lastUp < margin && 
            _lastDown - _lastUp > 0) 
            return true; 
        else 
            return false;
    }
    
    __inline bool isDownCheck (float margin)
    {
        if(_lastDown - _lastUp > margin)
            return true;
        else 
            return false;
    };
private:
    float _lastDown;
    float _lastUp;
};

class InputState
{
  public:
    InputState();
    virtual ~InputState();

    void                        update();
        
    bool                        middleMouseDown();
    bool                        leftMouseDown();
    bool                        rightMouseDown();
    
    void                        updateRepeat(DWORD key,
                                             DWORD state)throw();
    bool                        getKeyState(DWORD key)throw();
    bool                        buttonOrKeyPressed() throw();
    bool                        getRepeat(int index) throw()
        {    return _repeated[index].isRepeated (_repeatMargin); }; 

    bool                        hasGUIFocus() { return _hasGuiFocus;}
    void                        setHasGUIFocus (bool hasFocus) { _hasGuiFocus = hasFocus;}

    Ibl::Vector3f                rotation;
    Ibl::Vector3f                translation;
    
    DIDEVICEOBJECTDATA            _didodkb[32];
    DIDEVICEOBJECTDATA            _didodms[32];

    DWORD                        _keyboardElements;
    DWORD                        _mouseElements;

    float                        _x;
    float                        _y;
    float                        _z;

    float                        _elapsedTime;
    DWORD                        _persistKey[512];
    float                        _repeatMargin;
    RepeatKey                    _repeated[512];
    bool                        _leftMouseDown;
    bool                        _rightMouseDown;
    bool                        _middleMouseDown;

    Gamepad                     _gamepads[MAX_GAME_PAD_COUNT];

    bool                        _hasGuiFocus;

    int32_t                     _cursorPositionX;
    int32_t                     _cursorPositionY;

};

}

#endif
