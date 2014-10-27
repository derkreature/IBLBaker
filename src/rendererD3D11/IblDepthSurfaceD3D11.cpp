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

#include <IblDepthSurfaceD3D11.h>
#include <IblRenderDeviceD3D11.h>
#include <IblFormatConversionD3D11.h>
#include <IblLog.h>

namespace Ibl
{
DepthSurfaceD3D11::DepthSurfaceD3D11(Ibl::DeviceD3D11* device) :
    Ibl::IDepthSurface (device),
    _depthSurface (0),
    _depthTexture (0),
    _initializationData (0),
    _depthShaderResourceView (0),
    _direct3d(nullptr),
    _immediateCtx(nullptr)
{
    if (Ibl::DeviceD3D11* _device = 
        dynamic_cast <Ibl::DeviceD3D11*>(device))
    {
        _direct3d = *_device;
        _immediateCtx = _device->immediateCtx();
    }

    memset (&_desc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
}

DepthSurfaceD3D11::~DepthSurfaceD3D11()
{
    safedelete (_initializationData);
    free();
}

ID3D11ShaderResourceView*
DepthSurfaceD3D11::resourceView() const
{
    return _depthShaderResourceView;
}

ID3D11DepthStencilView *
DepthSurfaceD3D11::surface () const
{
    return _depthSurface;
}

ID3D11Texture2D*
DepthSurfaceD3D11::depthTexture() const
{
    return _depthTexture;
}

bool
DepthSurfaceD3D11::initialize (const Ibl::DepthSurfaceParameters* resource)
{
    _initializationData = new DepthSurfaceParameters(*resource);
    return create();
}

void
DepthSurfaceD3D11::setSize (const Ibl::Vector2i& size)
{
    _initializationData->setWidth (size.x);
    _initializationData->setHeight (size.y);
}

int
DepthSurfaceD3D11::multiSampleCount() const
{
    return _textureDesc.SampleDesc.Count;
}

int
DepthSurfaceD3D11::multiSampleQuality() const
{
    return _textureDesc.SampleDesc.Quality;
}

bool
DepthSurfaceD3D11::create()
{
    if (_initializationData)
    {
         if (_initializationData->width() == MIRROR_BACK_BUFFER)
        {
             _textureDesc.Width  = _deviceInterface->backbuffer()->width();
        }
        else if (_initializationData->width() == MIRROR_BACK_BUFFER_HALF)
        {
             _textureDesc.Width  = _deviceInterface->backbuffer()->width() / 2;
        }
        else if (_initializationData->width() == MIRROR_BACK_BUFFER_QUARTER)
        {
             _textureDesc.Width  = _deviceInterface->backbuffer()->width() / 4;
        }
        else if (_initializationData->width() == MIRROR_BACK_BUFFER_EIGTH)
        {
             _textureDesc.Width  = _deviceInterface->backbuffer()->width() / 8;
        }
        else
        {
            _textureDesc.Width = _initializationData->width();
        }

        if (_initializationData->height() == MIRROR_BACK_BUFFER)
        {
             _textureDesc.Height = _deviceInterface->backbuffer()->height();
        }
        else if (_initializationData->width() == MIRROR_BACK_BUFFER_HALF)
        {
             _textureDesc.Height = _deviceInterface->backbuffer()->height() / 2;
        }
        else if (_initializationData->width() == MIRROR_BACK_BUFFER_QUARTER)
        {
             _textureDesc.Height = _deviceInterface->backbuffer()->height() / 4;
        }
        else if (_initializationData->width() == MIRROR_BACK_BUFFER_EIGTH)
        {
             _textureDesc.Height = _deviceInterface->backbuffer()->height() / 8;
        }
        else
        {
            _textureDesc.Height = _initializationData->height();
        }
    


        // Create depth stencil texture

        _textureDesc.MipLevels = 1;
        
        if (_initializationData->slices() > 1)
        {
            _textureDesc.ArraySize = _initializationData->slices();
        }
        else
        {
            _textureDesc.ArraySize = 1;
        }

        _textureDesc.Format = findFormat (_initializationData->format());
        _textureDesc.SampleDesc.Count = _initializationData->multiSampleCount();
        _textureDesc.SampleDesc.Quality = _initializationData->multiSampleQuality();
        _textureDesc.Usage = D3D11_USAGE_DEFAULT;
        _textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE; 

        _textureDesc.CPUAccessFlags = 0;
        _textureDesc.MiscFlags = 0;
         
        HRESULT hr = _direct3d->CreateTexture2D( &_textureDesc, nullptr, &_depthTexture );
        if (FAILED(hr))
        {
            LOG ("FAILED TO CREATE TEXTURE2D for depth!\n");
        }

        DXGI_FORMAT srvFormat = DXGI_FORMAT_UNKNOWN;
        DXGI_FORMAT dsvFormat = DXGI_FORMAT_UNKNOWN;
        
        if (_textureDesc.Format == DXGI_FORMAT_R16_TYPELESS)
        {
            srvFormat = DXGI_FORMAT_R16_UNORM;
            dsvFormat = DXGI_FORMAT_D16_UNORM;
        }
        else if (_textureDesc.Format == DXGI_FORMAT_R32_TYPELESS)
        {
            srvFormat = DXGI_FORMAT_R32_FLOAT;
            dsvFormat = DXGI_FORMAT_D32_FLOAT;
        }
        else if (_textureDesc.Format ==  DXGI_FORMAT_R24G8_TYPELESS)
        { 
            dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
            srvFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        }

        // Create the depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
        memset (&descDSV, 0, sizeof(descDSV));
        descDSV.Format = dsvFormat;

        if (_initializationData->slices() > 1)
        {
            descDSV.ViewDimension = _initializationData->multiSampleCount() > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            descDSV.Texture2DArray.ArraySize = _initializationData->slices();    
            descDSV.Texture2DArray.FirstArraySlice = 0;
            descDSV.Texture2DArray.MipSlice = 0;
        }
        else
        {
            descDSV.ViewDimension = _initializationData->multiSampleCount() > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
            descDSV.Texture2D.MipSlice = 0;
        }

        descDSV.Flags = 0;

        if (SUCCEEDED(_direct3d->CreateDepthStencilView (_depthTexture, &descDSV, &_depthSurface )))
        {
            _depthSurface->GetDesc (&_desc);
        }
        else
        {
            LOG ("Failed to to create depth surface view");
        }

        {
            // Create a shader resource view
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

            srvDesc.Format = srvFormat;

            if (_initializationData->slices() > 1)
            {
                srvDesc.ViewDimension = _initializationData->multiSampleCount() > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                srvDesc.Texture2DArray.ArraySize = _initializationData->slices();    
                srvDesc.Texture2DArray.FirstArraySlice = 0;
                srvDesc.Texture2DArray.MipLevels = 1;
                srvDesc.Texture2DArray.MostDetailedMip = 0;
            }
            else
            {
                srvDesc.ViewDimension = _initializationData->multiSampleCount() > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
                srvDesc.Texture2D.MipLevels = 1;
                srvDesc.Texture2D.MostDetailedMip = 0;
            }

            ID3D11ShaderResourceView * resourceView = nullptr;
            if (SUCCEEDED (_direct3d->CreateShaderResourceView( _depthTexture, 
                                                                &srvDesc, 
                                                                &resourceView)))
            {
                _depthShaderResourceView = resourceView;
            }
            else
            {
                LOG ("FAILED in create shader resource view for depthTexture");
            }
        }


        return SUCCEEDED (hr);
    }    
    return false;
}

int
DepthSurfaceD3D11::width() const 
{ 
    return _textureDesc.Width;
}

int
DepthSurfaceD3D11::height() const 
{ 
    return _textureDesc.Height;
}

bool
DepthSurfaceD3D11::clearStencil()
{
    if (surface())
    {
        _immediateCtx->ClearDepthStencilView (surface(),  D3D11_CLEAR_STENCIL, 1, 0);
        return true;
    }
    return false;
}

bool
DepthSurfaceD3D11::clear() 
{
    if (surface())
    {
        _immediateCtx->ClearDepthStencilView (surface(), 
                                              D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
        return true;
    }
    return false;
}

bool
DepthSurfaceD3D11::free()
{
    saferelease(_depthShaderResourceView);
    saferelease(_depthSurface);
    saferelease(_depthTexture);
    return true;
}

bool
DepthSurfaceD3D11::cache()
{
    return true;
}

bool
DepthSurfaceD3D11::bind(uint32_t index) const
{
    _deviceInterface->bindDepthSurface (this);
    return false;
}
}