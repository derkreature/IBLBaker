//-----------//------------------------------------------------------------------------------------//
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

#include <IblRenderWindow.h>
#include <IblApplication.h>
#include <IblLog.h>

#include <../resources/resource.h>

#if HAS_RESOURCES
#include "resource.h"
#endif

namespace Ibl
{
namespace
{
void clientResize(HWND hWnd, int nWidth, int nHeight)
{
    RECT rcClient, rcWindow;
    POINT ptDiff;
    
    GetClientRect(hWnd, &rcClient);
    GetWindowRect(hWnd, &rcWindow);
    ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
    ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
    MoveWindow(hWnd,rcWindow.left, rcWindow.top, nWidth + ptDiff.x, nHeight + ptDiff.y, TRUE);

}
}

RenderWindow::RenderWindow(const Application* application) : 
    Window (application),
    _rendererHasFocus (true),
    _solidBrush(nullptr)
{
}

RenderWindow::~RenderWindow(void)
{
    if (_solidBrush)
    {
        DeleteObject(_solidBrush);
    }
}

bool
RenderWindow::rendererHasFocus() const
{
    return _rendererHasFocus;
}

bool
RenderWindow::create(const Application* application,
                     const std::string&  name,
                     bool windowed,
                     int width,
                     int height)
{
    Window::create (name, windowed, width, height);
    {

        _solidBrush = (HBRUSH)(CreateSolidBrush(RGB(0,0,0)));
        static const WCHAR* const windowClassStr = (L"IBLBaker Render Target Class");
        WNDCLASSEX windowClass = {
            sizeof(WNDCLASSEX),
            CS_OWNDC, 
            windowProc,
            0, 
            0, 
            application->instance(),
            0, 
            LoadCursor(0, IDC_ARROW), 
            _solidBrush, 
            0, 
            windowClassStr,
            0 
        };

        //set the icon instance for this application that we are creating for    
        windowClass.hIcon  = 
                LoadIcon (application->instance(), MAKEINTRESOURCE(IDI_DEFAULT));

        if (RegisterClassEx (&windowClass))
        {
            // adjust the window size so that the client area matches the arguments
            RECT size = { 0, 0, Window::width(), Window::height()};

            const std::string& windowNameStr = windowName();
            std::wstring windowNameW (windowNameStr.begin(), windowNameStr.end());
            HINSTANCE instance = application->instance();

            AdjustWindowRect(&size,
                WS_OVERLAPPEDWINDOW, FALSE);

            DWORD fullWidth = (size.right - size.left);
            DWORD fullHeight = (size.bottom - size.top);
            //create window
            WindowHandle handle = 
                    CreateWindow ((WCHAR*)windowClassStr, 
                                  (WCHAR*)windowNameW.c_str(),
                                  WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  fullWidth, fullHeight, 0,
                                  0,
                                  instance, this);
            if (handle)
            {
                setWindowHandle (handle);


                ShowWindow (windowHandle(), SW_SHOWNORMAL);
                updateBounds();

                clientResize(handle, width, height);
                UpdateWindow (windowHandle());
                return true;
            }
            else
            {
                THROW ("Failed to create window handle");
            }
        }
    }

    return false;
}

LRESULT
RenderWindow::callback (HWND window, 
                        UINT msg, 
                        WPARAM wParam, 
                        LPARAM lParam)
{
    if (windowHandle() == nullptr)
        return Window::callback (window, msg, wParam, lParam);

    switch (msg)
    {
        case WM_KEYDOWN:
            if(wParam == VK_ESCAPE)
                DestroyWindow(window);
            break;    
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }
    return Window::callback (window, msg, wParam, lParam);
}
}
