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

#include <IblInput.h>
#include <IblLog.h>
#include <IblApplication.h>
#include <IblX360Controller.h>
#include <WTypes.h>

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif 

namespace Ibl
{
Input::Input() : 
    _x360Controller(nullptr)
{
    _bufferSize = 16;
    _hasFocus = false;
    _x360Controller = 0;
}

Input::~Input()
{
    if(keyboard)
        keyboard->Unacquire();
    if (mouse)
        mouse->Unacquire();

    safedelete(_x360Controller);
}


Ibl::X360Controller* 
Input::xboxController()
{
    return _x360Controller;
}

bool Input::create(const Ibl::Application* application)
{
    _application = application;
    _x360Controller = new X360Controller();
    _x360Controller->create(&_inputState);

    // construct the base DirectInput8 device
    if (FAILED (DirectInput8Create(
        GetModuleHandle(NULL), 
        DIRECTINPUT_VERSION, 
        IID_IDirectInput8, 
        reinterpret_cast<void**>(&dinput), 
        0)))
    {
        LOG ("Input::Create(): Create InputDevice failed");
        return false;
    }
    
    // create a keyboard device
    if (FAILED (dinput->CreateDevice (GUID_SysKeyboard, &keyboard, 0)))
    {
        LOG ("Input::Create(): CreateDevice failed");
        return false;
    }

    HWND window = _application->window()->windowHandle();
    while (HWND hwnd = GetParent (window))
    {
        window = hwnd;
    }

    // set the keyboard cooperative level
    HRESULT hr = keyboard->SetCooperativeLevel(window, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    if (FAILED(hr))
    {
        if (hr == DIERR_INVALIDPARAM)
        {
            LOG ("Invalid parameter ...\n");
        }
        if (hr == E_HANDLE)
        {
            LOG ("Invalid wnd handle ...\n");
        }
        LOG ("Failed set keyboard coop "); // DXGetErrorDescription (hr)
    }

    DIPROPDWORD dipkdw;
    dipkdw.diph.dwSize       = sizeof(DIPROPDWORD);
    dipkdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipkdw.diph.dwObj        = 0;
    dipkdw.diph.dwHow        = DIPH_DEVICE;
    dipkdw.dwData            = _bufferSize;

    if (FAILED (keyboard->SetProperty (DIPROP_BUFFERSIZE, &dipkdw.diph)))
    {
        LOG ("Input::Create(): SetProperty keyboard failed");    
        return false;
    }

    //set the keyboard data format
    if (FAILED (keyboard->SetDataFormat (&c_dfDIKeyboard)))
    {
        LOG ("Input::Create(): SetDataFormat failed");    
        return false;
    }
    // attempt to aquire the keyboard
    keyboard->Acquire();

    if (FAILED (dinput->CreateDevice (GUID_SysMouse, &mouse, 0)))
    {
        LOG ("Input::Create(): Create Mouse failed");    
        return false;
    }

    if (FAILED (mouse->SetCooperativeLevel (window, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
    {
        LOG ("Input::Create(): mouse Set Cooperative Level failed"); // DXGetErrorDescription (hr)    
        return false;
    }

    DIPROPDWORD dipdw;
    dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj        = 0;
    dipdw.diph.dwHow        = DIPH_DEVICE;
    dipdw.dwData            = _bufferSize; // Arbitary buffer size

    if(FAILED (mouse->SetProperty (DIPROP_BUFFERSIZE, &dipdw.diph)))
    {
        LOG ("Input::Create(): set property for mouse failed");    
        return false;
    }

    if (FAILED ( mouse->SetDataFormat (&c_dfDIMouse)))
    {        
        LOG ("Input::Create(): mouse SetDataFormat failed");    
        return false;
    }

    mouse->Acquire();

    LOG ("Direct Input Systems Created Successfully");
    return true;
}

bool Input::readMouseInputBuffer()
{         
    if(keyboard == 0)
    {
        THROW (std::string ("There is also no Mouse"));
    }
    _inputState._mouseElements = 32;
    return SUCCEEDED (mouse->GetDeviceData (sizeof (DIDEVICEOBJECTDATA),
                                 _inputState._didodms, 
                                 &_inputState._mouseElements, 
                                 0));
}

bool Input::readKeyBoardInputBuffer()
{
    if (mouse == 0) 
    {
        THROW (std::string ("There is no Keyboard"));
    }
    _inputState._keyboardElements = 32;
    return SUCCEEDED (keyboard->GetDeviceData (sizeof(DIDEVICEOBJECTDATA),
                                               _inputState._didodkb, 
                                               &_inputState._keyboardElements, 
                                               0));
}

bool Input::update()
{    
    bool result = false;
    if ((keyboard->Acquire() == DIERR_OTHERAPPHASPRIO ) ||
        (mouse->Acquire() == DIERR_OTHERAPPHASPRIO)) 
    {
        _hasFocus = false;
        result = false;
    }

    POINT cursorPosition;
    GetCursorPos(&cursorPosition);
    ScreenToClient(_application->window()->windowHandle(), &cursorPosition);
    
    _inputState._x = (float)cursorPosition.x;
    _inputState._y = (float)cursorPosition.y;

    if (readKeyBoardInputBuffer() && 
        readMouseInputBuffer())
    {
        result = true;
    }
    else
    {
        result = false;
    }

    Ibl::Vector2i pos = Ibl::Vector2i ((int)_inputState._x, (int)_inputState._y);
    const Ibl::Region2i& box = _application->window()->windowBounds();
    bool hasFocus = _application->window()->hasFocus();

    if (!(pos.x > box.minExtent.x && pos.x < box.maxExtent.x &&
        pos.y > box.minExtent.y && pos.y < box.maxExtent.y) ||
          !hasFocus)
    {
        memset (_inputState._didodms, 0, sizeof (DIDEVICEOBJECTDATA) * 32);
    }

    if (_x360Controller)
    {
        _x360Controller->update();
    }

    _hasFocus = true;
    updatePersistantKeys();

    if (result)
    {
        _inputState.update();
    }

    return result;
}

void Input::updatePersistantKeys()
{
    for (uint32_t keystroke = 0; keystroke < _inputState._keyboardElements; keystroke++)
    {
        if (_inputState._didodkb [keystroke].dwOfs < 256)
        {
            _inputState._persistKey [_inputState._didodkb[keystroke].dwOfs] = 
                _inputState._didodkb [keystroke].dwData;
            _inputState.updateRepeat (_inputState._didodkb[keystroke].dwOfs, 
                                      _inputState._didodkb [keystroke].dwData);
        }
    }
}

}