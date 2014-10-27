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

#include <IblBackbufferSurfaceD3D11.h>
#include <IblTextureD3D11.h>
#include <IblLog.h>
#include <IblTextureImage.h>
#include <assert.h>

namespace Ibl
{

BackbufferSurfaceD3D11::BackbufferSurfaceD3D11(Ibl::IDevice* device, IDXGISwapChain* swapChain) :
    SurfaceD3D11(device),
    _swapChain (swapChain)
{
    create();
}

BackbufferSurfaceD3D11::~BackbufferSurfaceD3D11()
{
    free();
}

bool
BackbufferSurfaceD3D11::recreateOnResize()
{
    return true;
}

bool
BackbufferSurfaceD3D11::create()
{
    assert(_swapChain);
    if (_swapChain)
    {
        // Get the back buffer and desc
        _swapChain->GetBuffer( 0, __uuidof(*_resource), (LPVOID*)&_resource);
        _resource->GetDesc (&_textureDesc);

        _deviceInterface->setViewport (&Ibl::Viewport(
                                       0.0f, 
                                       0.0f, 
                                       (float)(_textureDesc.Width),
                                       (float)(_textureDesc.Height),
                                       0.0f,
                                       1.0f));

        // Create the render target view
        _direct3d->CreateRenderTargetView (_resource, nullptr, &_surface);
        _surface->GetDesc (&_desc);
        _mipViews.push_back(_surface);
    }
    return (_surface != 0);
}

bool
BackbufferSurfaceD3D11::free()
{
    SurfaceD3D11::free();

    if (_swapChain)
    {
        LOG ("Freeing the backbuffer... \n");
        saferelease(_resource);
    }
    return true;
}

}
