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

#include <IblWindow.h>
#include <IblApplication.h>
#include <IblLog.h>

#if HAS_RESOURCES
//#include "resource.h"
#endif

namespace Ibl
{
static std::map <WindowHandle, Window*> _windows;

WindowData::WindowData (WindowHandle window, UINT msg, WPARAM wparam, LPARAM lparam) :
_window (window),
_msg (msg),
_wparam (wparam),
_lparam (lparam)
{
}

WindowDependency::WindowDependency(){}
WindowDependency::~WindowDependency(){}

Window::Window(const Application* application) 
{
    _application = 0;
#ifdef _DEBUG
    assert (application);
#endif
    _application = application;
    _showCursor  = false;
    _window         = 0;
    _width       = 0;
    _height      = 0;
    _windowed    = false;
    _resizing    = false;
}

Window::~Window()
{
    destroy();

}


Ibl::Vector2i
Window::mousePosition() const
{
    POINT cursorPosition;
    GetCursorPos( &cursorPosition );
    ScreenToClient(windowHandle(), &cursorPosition );
    return Ibl::Vector2i (cursorPosition.x, cursorPosition.y);
}


bool
Window::create(const std::string& name,
               bool windowed,
               int width,
               int height)
{
#ifdef _DEBUG
    assert (_application);
#endif

    _width = width;
    _height = height;

    _windowed = windowed;
    _windowName = std::string(name.c_str());

    _windowBounds =
        Ibl::Region2i (Ibl::Vector2i (0, 0),
                    Ibl::Vector2i (width, height));

    return true;
}


void
Window::sendWindowChanged (Ibl::WindowData* windowData)
{
    for (auto it = _dependencies.begin(); it != _dependencies.end(); it++)
    {
        (*it)->handleWindowChanged(windowData);
    }
}

void
Window::addWindowDependency(WindowDependency* dependency)
{
    auto it = std::find(_dependencies.begin(), _dependencies.end(), dependency);
    if (it == _dependencies.end())
    {
        _dependencies.push_back(dependency);
    }
}

void
Window::removeWindowDependency(WindowDependency* dependency)
{
    auto it = std::find(_dependencies.begin(), _dependencies.end(), dependency);
    if (it != _dependencies.end())
    {
        _dependencies.erase(it);
    }
}

const Ibl::Region2i &
Window::windowBounds() const
{
    return _windowBounds;
}

bool
Window::created() const
{
    return (_window != 0);
}

bool
Window::destroy()
{
    DestroyWindow (_window);
    _window = 0;
    return true;
}

void
Window::setWindowHandle (WindowHandle handle)
{
    _window = handle;
    _windows.insert (std::make_pair (_window, this));
    updateBounds();
}

void
Window::handleSizeChanged()
{
}

const Ibl::Region2i&
Window::clientRect() const
{
    return _clientRect;
}

void 
Window::updateBounds()
{
    RECT crect;
    GetClientRect(_window, &crect);

    if (crect.left < 0 && crect.right < 0 &&
        crect.top < 0 && crect.bottom < 0)
    {
        return;
    }

    _clientRect =
        Ibl::Region2i(Ibl::Vector2i(crect.left, crect.top),
        Ibl::Vector2i(crect.right, crect.bottom));

    Ibl::Region2i windowBounds =
        Ibl::Region2i(Ibl::Vector2i(crect.left, crect.top),
        Ibl::Vector2i(crect.right, crect.bottom));

    _width = _windowBounds.size().x;
    _height = _windowBounds.size().y;

    if (_windowBounds.size() != windowBounds.size() &&
        _windowBounds.size().x > 0 && _windowBounds.size().y > 0 &&
        windowBounds.size().x > 0 && windowBounds.size().y > 0)
    {
        handleSizeChanged();
    }

    _windowBounds = windowBounds;
}

const WindowHandle
Window::windowHandle() const
{
    return _window;
}

const Application* 
Window::application() const
{

    return _application;

}

void
Window::setShowCursor(bool show)
{
    if (_showCursor != show)
    {
        _showCursor = show;
        ShowCursor (_showCursor);
    }
}

const std::string&
Window::windowName () const
{
    return _windowName;
}

bool
Window::showCursor() const
{
    return _showCursor;
}

bool
Window::windowed() const
{
    return _windowed;
}

int
Window::width() const
{
    return _width;
}

void
Window::setWidth(int value)
{
    _width = value;
}

int
Window::height() const
{
    return _height;
}

void
Window::setHeight(int value)
{
    _height = value;
}

LRESULT
Window::callback(HWND window, 
                 UINT msg, 
                 WPARAM wParam, 
                 LPARAM lParam)
{
    switch (msg)
    {
        case WM_EXITSIZEMOVE:
        {
            _resizing = false;
            updateBounds();
            break;
        }
        case WM_MOVE:
        {
            updateBounds();
            break;
        }
        case WM_WINDOWPOSCHANGED:
        {
            _resizing = true;
            break;
        }
        case WM_RBUTTONUP:
        case WM_LBUTTONUP:
        {
            _resizing = false;
            updateBounds();
            break;
        }
    };

    Ibl::WindowData windowData(window, msg, wParam, lParam);
    sendWindowChanged (&windowData);

    return DefWindowProc (window, msg, wParam, lParam);
}

bool
Window::hasFocus() const
{
    return GetFocus() == _window;
}

LRESULT
Window::windowProc (HWND window, 
                    UINT msg, 
                    WPARAM wParam, 
                    LPARAM lParam)
{
    // LOG ( "Msg = " << msg << " WPARAM " << wParam << " LPARAM " << lParam);

    switch(msg)
    {
        case WM_CREATE:
        {
            if (lParam)
            {
                CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
                if (Window* initWindow =
                    reinterpret_cast<Window*>(cs->lpCreateParams))
                {
                    _windows.insert (std::make_pair (window, initWindow));
                }
            }
            break;
        }
    }

    Window * currentWindow = nullptr;
    auto it = _windows.find(window);
    if (it != _windows.end())
    {
        if (currentWindow = it->second)
        {
            return it->second->callback (window, msg, wParam, lParam);
        }
    }

    return DefWindowProc (window, msg, wParam, lParam);
}
}
