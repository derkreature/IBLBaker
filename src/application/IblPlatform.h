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

#ifndef INCLUDED_PLATFORM
#define INCLUDED_PLATFORM

#ifdef _WIN32
#include <windows.h>
#endif

// Include Platform
#include <memory>
#include <cstddef>

// Include STL
#include <set>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <algorithm>

// Include streams
#include <iostream>
#include <fstream>
#include <streambuf>
#include <sstream>


// Include common system headers
#include <math.h>
#include <time.h>
#include <assert.h>
#include <limits.h>
#include <float.h>
#include <string.h>
#include <stdint.h>


// Typedef Win32 stuff out of the way
#if _WIN32 || _WIN64

typedef HINSTANCE ApplicationHandle;
typedef HWND      WindowHandle;
typedef HANDLE    OSHandle;

#define saferelease(p) { if(p) { (p)->Release(); (p)= 0; } }
#define assertfail(p)  { if(p != S_OK)throw std::runtime_error("Encountered a problem with direct3d11");}
#define succeeded(p)   { if(p == S_OK) return true; else return false; }
#define failed(p)      { if(p != S_OK) return true; else return false; }

// CRT's memory leak detection
#if defined(DEBUG) | defined(_DEBUG)
#include <crtdbg.h>
#endif

#if _WIN64
#define _64BIT 1
#define _32BIT 0
#else
#define _32BIT 1
#define _64BIT 0
#endif


#if _WIN32 || _WIN64
#include <d3d11.h>
#include <d3dx11effect.h>

#include <dxgi.h>
#include <D3D11.h>
#include <D3D11SDKLayers.h>
#include <D3D11Shader.h>
#include "EffectsD3D11/inc/d3dx11effect.h"
#include <DXGIType.h>
#include <d3d9Types.h>
#include <dinput.h> 
#include <strsafe.h>
#endif

#else

// Linux / Mac [TODO]
#if __GNUC__
#if __x86_64__ || __ppc64__
#define _64BIT 1
#define _32BIT 0
#else
#define _64BIT 0
#define _32BIT 1
#endif
#endif

#endif

#define THROW(text)                                                \
{                                                                  \
    std::ostringstream s;                                          \
    s << text << "\0";                                             \
    throw std::runtime_error (std::string(s.str().c_str()));       \
}

#define safedelete(p)       { if(p) { delete (p);     (p)= 0; } }
#define safedeletearray(p) { if(p) { delete[] (p);   (p)= 0; } }
#define failifnull(p)      { if(p == nullptr) THROW("nullptr exception");}



#endif