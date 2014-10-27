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

#include <IblRenderTargetTextureD3D11.h>
#include <IblEffectD3D11.h>
#include <IblISurface.h>
#include <IblGpuVariableD3D11.h>
#include <IblLog.h>
#include <IblFormatConversionD3D11.h>
#include <comdef.h>
#include <crtdbg.h>
//#include <DxErr.h>
//#pragma comment (lib, "DXErr.lib")


namespace Ibl
{
RenderTargetTextureD3D11::RenderTargetTextureD3D11 (Ibl::DeviceD3D11 * device) :
Ibl::TextureD3D11 (device),
_surface (nullptr),
_texture (nullptr),
_uav(nullptr),
_mappedSubresourceId(-1),
_stagingTexture(nullptr)
{
}

RenderTargetTextureD3D11::~RenderTargetTextureD3D11()
{
    free();
}

bool
RenderTargetTextureD3D11::clearSurface(uint32_t layerId, float r, float g, float b, float a)
{
    _surface->clearAll(r, g, b, a);
    return true;
}


bool
RenderTargetTextureD3D11::recreateOnResize()
{
    if (_resource->width() == MIRROR_BACK_BUFFER ||
        _resource->width() == MIRROR_BACK_BUFFER_HALF ||
        _resource->width() == MIRROR_BACK_BUFFER_QUARTER ||
        _resource->width() == MIRROR_BACK_BUFFER_EIGTH)
    {
        return true;
    }
    return false;
}
bool
RenderTargetTextureD3D11::map(uint32_t imageLevel, uint32_t mipLevel) const
{
    if (_mappedSubresourceId == -1)
    {
        _mappedSubresourceId = D3D11CalcSubresource( static_cast<UINT>( mipLevel ), static_cast<UINT>( imageLevel  ), static_cast<UINT>( resource()->mipLevels()) );
        if (SUCCEEDED(_immediateCtx->Map(_stagingTexture, _mappedSubresourceId, D3D11_MAP_READ, 0, &_mappedResource)))
        {
            _mapped = true;
            return true;
        }
    }
    return false;
}

bool
RenderTargetTextureD3D11::unmap() const
{
    if (_mappedSubresourceId >= 0)
    {
        _immediateCtx->Unmap(_stagingTexture, _mappedSubresourceId);
        _mappedSubresourceId = -1;
        return true;
    }
    return false;
}

bool
RenderTargetTextureD3D11::unmapFromRead() const
{
    if (_mapped)
    {
        saferelease(_stagingTexture);
        _mapped = false;
        return true;
    }

    return false;
}

bool                
RenderTargetTextureD3D11::mapForRead() const
{
    if (!_mapped)
    {
        // If it's a texture2D
        D3D11_TEXTURE2D_DESC stagingDesc;
        
        _texture->GetDesc(&stagingDesc);
        stagingDesc.CPUAccessFlags =   D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
        stagingDesc.Usage =            D3D11_USAGE_STAGING;
        stagingDesc.BindFlags =        0;
        
        // Clobber misc flags.
        stagingDesc.MiscFlags &= ~D3D11_RESOURCE_MISC_GENERATE_MIPS;
        stagingDesc.MiscFlags &= ~D3D11_RESOURCE_MISC_SHARED;
        
        uint32_t support = 0;
        HRESULT hr = _direct3d->CheckFormatSupport(stagingDesc.Format, &support );
        if ( FAILED(hr) )
        {
//            LOG ("Failure " << DXGetErrorDescriptionA(hr));
			//            LOG ("Error: " << DXGetErrorStringA(hr));
            return false;
        }
        
        hr = _direct3d->CreateTexture2D( &stagingDesc, nullptr, &_stagingTexture);
        if (SUCCEEDED(hr))
        {
            // Copy contents of height map into staging version
            _immediateCtx->CopyResource(_stagingTexture, _texture);
            _mapped = true;
            return true;
        }
        else
        {   
        
			//            LOG ("Failure " << DXGetErrorDescriptionA(hr));
			//           LOG ("Error: " << DXGetErrorStringA(hr));
        }
    }
    return false;
}


bool
RenderTargetTextureD3D11::create()
{
    // check multi sample information
    for (uint32_t i = 0; i < 9; i++)
    {
        uint32_t quality = 0;
        _direct3d->CheckMultisampleQualityLevels (findFormat(_resource->format()), i, &quality);    
    }
    D3D11_TEXTURE2D_DESC desc;
    memset (&desc, 0, sizeof (D3D11_TEXTURE2D_DESC));

    bool cubeMap = false;
    uint32_t arraySize = _resource->textureCount();

    if (_resource->dimension() == Ibl::CubeMap)
    {
        arraySize = 6;
        cubeMap = true;
    }

    desc.ArraySize = arraySize;
    desc.CPUAccessFlags = 0;
    desc.Format = findFormat(_resource->format());

    if (_resource->width() == MIRROR_BACK_BUFFER)
    {
        _width = _deviceInterface->backbuffer()->width();
    }
    else if (_resource->width() == MIRROR_BACK_BUFFER_HALF)
    {
        _width = _deviceInterface->backbuffer()->width() / 2;
    }
    else if (_resource->width() == MIRROR_BACK_BUFFER_QUARTER)
    {
        _width = _deviceInterface->backbuffer()->width() / 4;
    }
    else if (_resource->width() == MIRROR_BACK_BUFFER_EIGTH)
    {
        _width = _deviceInterface->backbuffer()->width() / 8;
    }

    if (_resource->height() == MIRROR_BACK_BUFFER)
    {
        _height = _deviceInterface->backbuffer()->height();
    }
    else if (_resource->width() == MIRROR_BACK_BUFFER_HALF)
    {
        _height = _deviceInterface->backbuffer()->height() / 2;
    }
    else if (_resource->width() == MIRROR_BACK_BUFFER_QUARTER)
    {
        _height = _deviceInterface->backbuffer()->height() / 4;
    }
    else if (_resource->width() == MIRROR_BACK_BUFFER_EIGTH)
    {
        _height = _deviceInterface->backbuffer()->height() / 8;
    }
        
    desc.Width = _width;
    desc.Height = _height;
    desc.MipLevels = (uint32_t)(_resource->mipLevels());

    //if (desc.ArraySize <= 1) // Why?
    //{
    if (_resource->generateMipMaps())
    {
        desc.MiscFlags = desc.MipLevels > 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0 ;
    }
    //}

    if (cubeMap)
    {
        desc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
    }

    desc.SampleDesc.Count = _resource->multiSampleCount();
    desc.SampleDesc.Quality = _resource->multiSampleQuality();
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    desc.Usage = D3D11_USAGE_DEFAULT;
    
    if (_resource->useUAV())
        desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;    
    
    if (desc.SampleDesc.Count > 1)
    {
        uint32_t quality;
        _direct3d->CheckMultisampleQualityLevels (findFormat(_resource->format()), 
                                                  desc.SampleDesc.Count, &quality);
        if (quality <= desc.SampleDesc.Quality)
        {
            // Take the highest we can get
            desc.SampleDesc.Quality = quality - 1;
        }
    }

    _desc = desc;
    //Create the texture
    if (FAILED(_direct3d->CreateTexture2D( &desc, nullptr, &_texture)))
    {
        LOG ("TextureD3D11 Error: could not create render target texture");
        return false;
    }

    if (_resource->useUAV())
        _direct3d->CreateUnorderedAccessView(_texture, nullptr, &_uav);

    if (_texture)
    {
        ID3D11ShaderResourceView * resourceView = nullptr;
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = desc.Format;

        if (cubeMap)
        {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            srvDesc.TextureCube.MipLevels = desc.MipLevels;
            srvDesc.TextureCube.MostDetailedMip = 0;
        }
        else if (desc.ArraySize > 1)
        {
            srvDesc.ViewDimension = (_resource->multiSampleCount() > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            srvDesc.Texture2DArray.ArraySize = desc.ArraySize;
            srvDesc.Texture2DArray.FirstArraySlice = 0;
            srvDesc.Texture2DArray.MipLevels = desc.MipLevels;
            srvDesc.Texture2DArray.MostDetailedMip = 0;
        }
        else
        {
            srvDesc.Texture2D.MipLevels = desc.MipLevels;
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.ViewDimension = (_resource->multiSampleCount() > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
        }

        if (SUCCEEDED (_direct3d->CreateShaderResourceView (_texture, 
                                                            &srvDesc, 
                                                            &resourceView)))
        {
            setResourceView (resourceView); /* needs index as does texture bind */
            setTexture (_texture);
            setFormat (desc.Format);

            if (!_surface)
            {
                _surface = new SurfaceD3D11 (_deviceInterface);
                if (!_surface->initialize(0, desc.ArraySize, this))
                {
                    LOG("Failed to create Surface!");
                    assert(0);
                }
            }
        }
        else
        {
            LOG ("Failed to create shader resource view for rendertargettexture.");
            return false;
        }

        // Create resource views for each index in the array. All mips for all single images.
        srvDesc.Format = desc.Format;
        if (desc.ArraySize > 1)
        {
            srvDesc.ViewDimension = (_resource->multiSampleCount() > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            srvDesc.Texture2DArray.ArraySize = desc.ArraySize;
            srvDesc.Texture2DArray.FirstArraySlice = 0;
            srvDesc.Texture2DArray.MipLevels = desc.MipLevels;
            srvDesc.Texture2DArray.MostDetailedMip = 0;

            for (uint32_t i = 0; i < desc.ArraySize; i++)
            {
                ID3D11ShaderResourceView * sliceResourceView = nullptr;
                srvDesc.Texture2DArray.ArraySize = 1;
                srvDesc.Texture2DArray.FirstArraySlice = i;

                HRESULT hr = _direct3d->CreateShaderResourceView (_texture, 
                                                                     &srvDesc, 
                                                                     &sliceResourceView);

                if (SUCCEEDED (hr))
                {
                    _sliceResources.push_back (sliceResourceView);
                }
                else
                {
					//                    LOG ("Failed because: " << DXGetErrorStringA(hr) << " long reason: " << DXGetErrorDescriptionA(hr));

					//                    LOG ("Failed to allocate slice resource for slice " << i);
                    return false;
                }

                Ibl::SurfaceD3D11 * surface = new SurfaceD3D11 (_deviceInterface);
                if (surface->initialize (i, 1, this))
                {
                    _sliceSurfaces.push_back (surface);
                }
                else
                {
                    LOG ("Failed to allocate slice surface for slice " << i);
                    safedelete (surface);
                    return false;
                }
            }
        }


        // Resources and surfaces for all arrayIds with specified mipmap.
        for (uint32_t mipLevel = 0; mipLevel < desc.MipLevels; mipLevel++)
        {
            // Shader resource.
            ID3D11ShaderResourceView* mipResource = nullptr;
            if (desc.ArraySize > 1)
            {
                srvDesc.ViewDimension = (_resource->multiSampleCount() > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                srvDesc.Texture2DArray.ArraySize = desc.ArraySize;
                srvDesc.Texture2DArray.FirstArraySlice = 0;
                srvDesc.Texture2DArray.MipLevels = 1;
                srvDesc.Texture2DArray.MostDetailedMip = mipLevel;
            }
            else
            {
                srvDesc.ViewDimension = (_resource->multiSampleCount() > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
                srvDesc.Texture2D.MipLevels = 1;
                srvDesc.Texture2D.MostDetailedMip = mipLevel;
            }

            if (SUCCEEDED (_direct3d->CreateShaderResourceView (_texture, &srvDesc, &mipResource)))
            {
                _resourcesAllImagesPerMip.push_back (mipResource);
            }
            else
            {
                LOG ("Failed to allocate slice resource for mip level " << mipLevel);
                return false;
            }

            // Surface / Rendertarget
            Ibl::SurfaceD3D11* mipSurface = new SurfaceD3D11 (_deviceInterface);
            mipSurface->initialize (0, desc.ArraySize, this, mipLevel);
            _surfacesAllImagesPerMip.push_back(mipSurface);
        }

        // Resources for specified arrayId with specified mipmap
        for (uint32_t arrayId = 0; arrayId < desc.ArraySize; arrayId++)
        {
            std::vector<ID3D11ShaderResourceView*>       resourceMipArray;
            std::vector<Ibl::SurfaceD3D11*>       surfaceMipArray;

            for (uint32_t mipLevel = 0; mipLevel < desc.MipLevels; mipLevel++)
            {
                // Shader resource
                ID3D11ShaderResourceView* mipResource = nullptr;
                if (desc.ArraySize > 1)
                {

                    srvDesc.ViewDimension = (_resource->multiSampleCount() > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                    srvDesc.Texture2DArray.ArraySize = 1;
                    srvDesc.Texture2DArray.FirstArraySlice = 0;
                    srvDesc.Texture2DArray.MipLevels = 1;
                    srvDesc.Texture2DArray.MostDetailedMip = mipLevel;
                }
                else
                {
                    srvDesc.ViewDimension = (_resource->multiSampleCount() > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
                    srvDesc.Texture2D.MipLevels = 1;
                    srvDesc.Texture2D.MostDetailedMip = mipLevel;
                }

                if (SUCCEEDED (_direct3d->CreateShaderResourceView (_texture, &srvDesc, &mipResource)))
                {
                    resourceMipArray.push_back (mipResource);
                }
                else
                {
                    LOG ("Failed to allocate slice resource for array level " << arrayId <<" mip level " << mipLevel);
                    return false;
                }

                // Surface / rendetarget
                Ibl::SurfaceD3D11* mipSurface = new SurfaceD3D11 (_deviceInterface);
                mipSurface->initialize (arrayId, 1, this, mipLevel);
                surfaceMipArray.push_back(mipSurface);
            }

            _resourcesPerImagePerMip.insert(std::make_pair(arrayId, resourceMipArray));
            _surfacesPerImagePerMip.insert(std::make_pair(arrayId, surfaceMipArray));
        }
    }


    return true;
}

// Clear UAV, texture must be created with UAV support.
void
RenderTargetTextureD3D11::clearUnorderedAccessViewUint(uint32_t clearValue) const
{
    if (_uav)
    {
        const UINT clearValueUINT[4] = { clearValue, clearValue, clearValue, clearValue };
        _immediateCtx->ClearUnorderedAccessViewUint (_uav, &clearValueUINT[0]);
    }
}

bool
RenderTargetTextureD3D11::free()
{
    // TODO, nothing should *store* a surface aside from the backbuffer.
    for (uint32_t i = 0; i < _sliceResources.size(); i++)
    {
        saferelease (_sliceResources[i]);
    }
    _sliceResources.clear();

    for (uint32_t i = 0; i < _sliceSurfaces.size(); i++)
    {
        safedelete (_sliceSurfaces[i]);
    }
    _sliceSurfaces.clear();

    for (uint32_t i = 0; i < _surfacesAllImagesPerMip.size(); i++)
    {
        safedelete (_surfacesAllImagesPerMip[i]);
    }
    _surfacesAllImagesPerMip.clear();

    for (uint32_t i = 0; i < _resourcesAllImagesPerMip.size(); i++)
    {
        saferelease (_resourcesAllImagesPerMip[i]);
    }
    _resourcesAllImagesPerMip.clear();

    for (auto it = _surfacesPerImagePerMip.begin(); it != _surfacesPerImagePerMip.end(); it++)
    {
        std::vector<SurfaceD3D11*>      & surfaces = it->second;
        for (size_t i = 0; i < surfaces.size(); i++)
        {
            safedelete (surfaces[i]);
        }
    }
    _surfacesPerImagePerMip.clear();

    for (auto it = _resourcesPerImagePerMip.begin(); it != _resourcesPerImagePerMip.end(); it++)
    {
        std::vector<ID3D11ShaderResourceView *>      & resources = it->second;
        for (size_t i = 0; i < resources.size(); i++)
        {
            saferelease (resources[i]);
        }
    }

    _sliceSurfaces.clear();

    safedelete(_surface);
    saferelease(_uav);

    return __super::free();
}


bool
RenderTargetTextureD3D11::bindSurface(int renderTargetIndex)  const
{
    if (_activeSlice >= 0)
    {
        return _sliceSurfaces[_activeSlice]->bind (renderTargetIndex);
    }
    else
    {
        return _surface->bind (renderTargetIndex);
    }
}

ID3D11ShaderResourceView *
RenderTargetTextureD3D11::resourceView(int32_t arrayId, int32_t mipId) const
{
    if (arrayId == -1 && mipId == -1)
    {
        // Normal case.
        return TextureD3D11::_resourceView;
    }
    else if (arrayId >= 0 && mipId == -1)
    {
        if (arrayId < (int)_sliceResources.size())
        {
            return _sliceResources[arrayId];
        }
    }
    // Array range with specific mipmap.
    else if (arrayId == -1 && mipId >= 0)
    {
        return _resourcesAllImagesPerMip[mipId];
    }
    // Array Id with specific mipmap
    else if (arrayId >= 0 && mipId >= 0)
    {
        auto it = _resourcesPerImagePerMip.find((size_t)(arrayId));
        return it->second[mipId];
    }

    return nullptr;
}

const Ibl::ISurface*
RenderTargetTextureD3D11::surface (int32_t arrayId, int32_t mipId) const
{
    if (arrayId == -1 && mipId == -1)
    {
        // Normal case.
        return _surface;
    }
    else if (arrayId >= 0 && mipId == -1)
    {
        // 0th mip Id for an arrayId.
        if (arrayId < (int32_t)_sliceSurfaces.size())
        {
            return _sliceSurfaces[arrayId];
        }
        return nullptr;
    }
    // Array range with specific mipmap.
    else if (arrayId == -1 && mipId >= 0)
    {
        return _surfacesAllImagesPerMip[mipId];
    }
    // Array Id with specific mipmap
    else if (arrayId >= 0 && mipId >= 0)
    {
        auto it = _surfacesPerImagePerMip.find((size_t)(arrayId));
        return (it->second)[mipId];
    }

    return nullptr;
}

ID3D11UnorderedAccessView* 
RenderTargetTextureD3D11::unorderedView() const
{
    return _uav;
}

bool
RenderTargetTextureD3D11::isCubeMap() const
{
    return (_desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE) == D3D11_RESOURCE_MISC_TEXTURECUBE;
}

}