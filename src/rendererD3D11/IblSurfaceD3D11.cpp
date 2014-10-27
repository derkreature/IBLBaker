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

#include <IblSurfaceD3D11.h>
#include <IblTextureD3D11.h>
#include <IblLog.h>
#include <IblTextureImage.h>

namespace Ibl
{
SurfaceD3D11::SurfaceD3D11(Ibl::IDevice* device) :
    Ibl::ISurface (device),
    _surface (nullptr),
    _texture (nullptr),
    _resource (nullptr),
    _firstLevel (0),
    _numberOfLevels (1),
    _direct3d(nullptr),
    _immediateCtx(nullptr),
    _mipLevel(-1)
{
    if (Ibl::DeviceD3D11* _device = 
        dynamic_cast <Ibl::DeviceD3D11*>(device))
    {
        _direct3d = *_device;
        _immediateCtx = _device->immediateCtx();
    }

    memset (&_textureDesc, 0, sizeof (D3D11_TEXTURE2D_DESC));
}

SurfaceD3D11::~SurfaceD3D11()
{
    free();
}

bool
SurfaceD3D11::recreateOnResize()
{
    return false;
}

static DXGI_FORMAT EnsureNotTypeless( DXGI_FORMAT fmt )
{
    // Assumes UNORM or FLOAT; doesn't use UINT or SINT
    switch( fmt )
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS: return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case DXGI_FORMAT_R32G32B32_TYPELESS:    return DXGI_FORMAT_R32G32B32_FLOAT;
    case DXGI_FORMAT_R16G16B16A16_TYPELESS: return DXGI_FORMAT_R16G16B16A16_UNORM;
    case DXGI_FORMAT_R32G32_TYPELESS:       return DXGI_FORMAT_R32G32_FLOAT;
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:  return DXGI_FORMAT_R10G10B10A2_UNORM;
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:     return DXGI_FORMAT_R8G8B8A8_UNORM;
    case DXGI_FORMAT_R16G16_TYPELESS:       return DXGI_FORMAT_R16G16_UNORM;
    case DXGI_FORMAT_R32_TYPELESS:          return DXGI_FORMAT_R32_FLOAT;
    case DXGI_FORMAT_R8G8_TYPELESS:         return DXGI_FORMAT_R8G8_UNORM;
    case DXGI_FORMAT_R16_TYPELESS:          return DXGI_FORMAT_R16_UNORM;
    case DXGI_FORMAT_R8_TYPELESS:           return DXGI_FORMAT_R8_UNORM;
    case DXGI_FORMAT_BC1_TYPELESS:          return DXGI_FORMAT_BC1_UNORM;
    case DXGI_FORMAT_BC2_TYPELESS:          return DXGI_FORMAT_BC2_UNORM;
    case DXGI_FORMAT_BC3_TYPELESS:          return DXGI_FORMAT_BC3_UNORM;
    case DXGI_FORMAT_BC4_TYPELESS:          return DXGI_FORMAT_BC4_UNORM;
    case DXGI_FORMAT_BC5_TYPELESS:          return DXGI_FORMAT_BC5_UNORM;
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:     return DXGI_FORMAT_B8G8R8A8_UNORM;
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:     return DXGI_FORMAT_B8G8R8X8_UNORM;
    case DXGI_FORMAT_BC7_TYPELESS:          return DXGI_FORMAT_BC7_UNORM;
    default:                                return fmt;
    }
}

bool
SurfaceD3D11::writeToFile (const std::string& filename) const
{

    if (_surface)
    {
        ID3D11Texture2D * stagingTexture = nullptr;
        ID3D11Texture2D* sourceTexture = nullptr;
        if (TextureD3D11* texture = dynamic_cast<TextureD3D11*>(_texture))
        {
            sourceTexture = (ID3D11Texture2D*)texture->texture();
        }
        else if (_resource)
        {
            sourceTexture = _resource;
        }

        if (!sourceTexture)
        {
            LOG ("Source texture is null. Cannot save screenshot");
        }

        D3D11_TEXTURE2D_DESC desc;
        sourceTexture->GetDesc(&desc);
   

        D3D11_RESOURCE_DIMENSION resType = D3D11_RESOURCE_DIMENSION_UNKNOWN;
        sourceTexture->GetType( &resType );

        if ( resType != D3D11_RESOURCE_DIMENSION_TEXTURE2D )
        {
            LOG ("Format is not supported");
            return false;
        }
        HRESULT hr = E_FAIL;

        if ( desc.SampleDesc.Count > 1 )
        {
            // MSAA content must be resolved before being copied to a staging texture
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;

            ID3D11Texture2D* temp;
            hr = _direct3d->CreateTexture2D( &desc, 0, &temp );
            if ( FAILED(hr) )
            {
                LOG ("Could not create resolve texture for multisample buffer while taking screenshot");
                return false;
            }

            DXGI_FORMAT fmt = EnsureNotTypeless( desc.Format );

            UINT support = 0;
            hr = _direct3d->CheckFormatSupport( fmt, &support );
            if ( FAILED(hr) )
            {
                LOG ("Format is not supported");
                return false;
            }

            if ( !(support & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE) )
                return false;

            for( UINT item = 0; item < desc.ArraySize; ++item )
            {
                for( UINT level = 0; level < desc.MipLevels; ++level )
                {
                    UINT index = D3D11CalcSubresource( level, item, desc.MipLevels );
                    _immediateCtx->ResolveSubresource( temp, index, sourceTexture, index, fmt );
                }
            }

            desc.BindFlags = 0;
            desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            desc.Usage = D3D11_USAGE_STAGING;

            hr = _direct3d->CreateTexture2D( &desc, 0, &stagingTexture );
            if ( FAILED(hr) )
            {
                LOG ("Could not create staging texture " << __LINE__ << " " << __FILE__);
                return false;
            }

            _immediateCtx->CopyResource( stagingTexture, temp );
            saferelease(temp);
        }
        else
        {
            // Otherwise, create a staging texture from the non-MSAA source
            desc.BindFlags = 0;
            desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            desc.Usage = D3D11_USAGE_STAGING;

            hr = _direct3d->CreateTexture2D( &desc, 0, &stagingTexture );
            if ( FAILED(hr) )
            {
                LOG ("Could not create staging texture " << __LINE__ << " " << __FILE__);
                return false;
            }

            _immediateCtx->CopyResource( stagingTexture, sourceTexture);
        }

        // MAP and save
        D3D11_MAPPED_SUBRESOURCE mapped;
        if ( FAILED(_immediateCtx->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &mapped )) )
        {
            LOG ("Could not map staging resource " << __LINE__ << " " << __FILE__);
            _immediateCtx->Unmap(stagingTexture, 0);
            return false;
        }
        else
        {
            Ibl::TextureImagePtr imagePtr(new Ibl::TextureImage());
            Ibl::PixelFormat pf = PF_BYTE_RGBA;

            size_t width = desc.Width;
            size_t height = desc.Height;
            // Copy row by row to contiguous buffer.
            uint8_t * pixelData =(uint8_t*)malloc(sizeof(uint8_t) * width * height * 4);

            uint8_t * dstDataPtr = pixelData;
            uint8_t * srcDataPtr = (uint8_t*)(mapped.pData);
            size_t dstRowPitch = width * sizeof(uint8_t) * 4; 

            for (size_t y = 0; y < desc.Height; y++)
            {
                // mapped.RowPitch, mapped.RowPitch * desc.Height,
                memcpy (dstDataPtr, srcDataPtr, sizeof(uint8_t) * 4 * width);

                dstDataPtr += dstRowPitch;
                srcDataPtr += mapped.RowPitch;
            }

            imagePtr->loadDynamicTextureImage(pixelData, width, height, 1, pf, true);
            imagePtr->save(filename);
            _immediateCtx->Unmap(stagingTexture, 0);
            ::free(pixelData);
        }        

        saferelease(stagingTexture);
    }

    return false;
}

bool 
SurfaceD3D11::initialize (int firstLevel, 
                          int numberOfLevels, 
                          Ibl::ITexture* texture,
                          int mipLevel)
{
    _firstLevel = firstLevel;
    _numberOfLevels = numberOfLevels;
    _texture = texture;
    _mipLevel = mipLevel;

    return create();
}

bool
SurfaceD3D11::clearAll(float r, float g, float b, float a)
{
    float clearColor[4] = {r, g, b, a};

    for (size_t i = 0; i < _textureDesc.MipLevels; i++)
    {
        _immediateCtx->ClearRenderTargetView (_mipViews[i], clearColor);
    }

    return true;
}

bool
SurfaceD3D11::create()
{
    // Get the view from the current texture
    if (TextureD3D11* texture = dynamic_cast<TextureD3D11*>(_texture))
    {
        if (_resource = (ID3D11Texture2D*)(texture->texture()))
        {
            _resource->GetDesc (&_textureDesc);

            D3D11_RENDER_TARGET_VIEW_DESC desc;
            memset (&desc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
            desc.Format = texture->dxFormat();

            // Problem with individual surfaces is here.
            // Need to check the array level and for cube map here.
            // if cubemap && _num levels, face, otherwise, take the whole bloody lot.

            if (texture->resource()->dimension() == Ibl::CubeMap  || texture->resource()->textureCount() > 1)
            {
                desc.ViewDimension = texture->multiSampleCount() > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;

                if (_mipLevel == -1)
                {
                    desc.Texture2DArray.MipSlice = 0;
                }
                else
                {
                    desc.Texture2DArray.MipSlice = _mipLevel;
                }

                desc.Texture2DArray.FirstArraySlice = _firstLevel;
                desc.Texture2DArray.ArraySize = _numberOfLevels;
            }
            else
            {
                desc.ViewDimension = texture->multiSampleCount() > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;

                if (_mipLevel == -1)
                {
                    desc.Texture2D.MipSlice = 0;
                }
                else
                {
                    desc.Texture2D.MipSlice = _mipLevel;
                }

            }

            _direct3d->CreateRenderTargetView (_resource, &desc, &_surface);
            _surface->GetDesc (&_desc);
            _mipViews.push_back(_surface);

            if (_mipLevel == -1)
            {
                for (size_t mipLevel = 1; mipLevel < _textureDesc.MipLevels; mipLevel++)
                {
                    desc.Texture2D.MipSlice = (uint32_t)(mipLevel);
                    ID3D11RenderTargetView * mipView;
                    _direct3d->CreateRenderTargetView (_resource, &desc, &mipView);
                    _mipViews.push_back(mipView);
                }
            }
        }
        else
        {
            LOG ("Error, no texture to create render target view from ... ");
        }
    }

    return (_surface != 0);
}

bool
SurfaceD3D11::cache()
{
    return true;
}

bool
SurfaceD3D11::free()
{
    for (size_t i = 0;i < _mipViews.size(); i++)
    {
        saferelease(_mipViews[i]);
    }
    _mipViews.clear();
    // _surface is a member of _mipViews.
    _surface = nullptr;
    memset (&_desc, 0, sizeof (D3D11_RENDER_TARGET_VIEW_DESC)); 
    return true;
}

unsigned int 
SurfaceD3D11::width() const
{
    return _textureDesc.Width;
}

unsigned int
SurfaceD3D11::height() const
{
    return _textureDesc.Height;
}

bool
SurfaceD3D11::bind(uint32_t level) const
{
    _deviceInterface->bindSurface (level, this);
    return true;
}

bool
SurfaceD3D11::bindAndClear(uint32_t level) const
{
    float ClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; //red,green,blue,alpha
    _immediateCtx->ClearRenderTargetView (surface(), ClearColor );
    _deviceInterface->bindSurface (level, this);

    return true;
}   

ID3D11RenderTargetView*
SurfaceD3D11::surface() const
{
    return _surface;
}

ID3D11Texture2D*
SurfaceD3D11::resource() const
{
    return _resource;
}
}