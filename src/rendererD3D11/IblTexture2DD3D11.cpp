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

#include <IblTexture2DD3D11.h>
#include <IblEffectD3D11.h>
#include <IblFormatConversionD3D11.h>


namespace Ibl
{
Texture2DD3D11::Texture2DD3D11(Ibl::DeviceD3D11* deviceArg) : 
    TextureD3D11 (deviceArg),
    _stagingTexture(nullptr),
    _mappedSubresourceId (-1)
{
}

Texture2DD3D11::~Texture2DD3D11()
{
    free();
}

bool
Texture2DD3D11::map(uint32_t imageLevel, uint32_t mipLevel) const
{
    if (_mappedSubresourceId == -1)
    {
        uint32_t mipLevels = (uint32_t)(resource()->mipLevels());
		_mappedSubresourceId = D3D11CalcSubresource(mipLevel, imageLevel, (uint32_t)(resource()->mipLevels()));
        if (SUCCEEDED(_immediateCtx->Map(_stagingTexture, _mappedSubresourceId, D3D11_MAP_READ, 0, &_mappedResource)))
        {
            _mapped = true;
            return true;
        }
    }
    return false;
}

bool
Texture2DD3D11::unmap() const
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
Texture2DD3D11::unmapFromRead() const
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
Texture2DD3D11::mapForRead() const
{
    if (!_mapped)
    {
        // If it's a texture2D
        D3D11_TEXTURE2D_DESC stagingDesc = desc();
        stagingDesc.CPUAccessFlags =   D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
        stagingDesc.Usage =            D3D11_USAGE_STAGING;
        stagingDesc.BindFlags =        0;
        _direct3d->CreateTexture2D( &stagingDesc, nullptr, &_stagingTexture);

        _immediateCtx->CopyResource(_stagingTexture, _texture);
        _mapped = true;
        return true;
    }
    return false;
}

bool
Texture2DD3D11::free()
{
    saferelease(_stagingTexture);
    return TextureD3D11::free();
}

bool 
Texture2DD3D11::create()
{
    ID3D11Resource * resource = 0;
    bool staging = _resource->type() == Ibl::StagingFromFile;

    D3D11_TEXTURE2D_DESC desc;
    ID3D11Texture2D* map = nullptr;
    D3D11_SUBRESOURCE_DATA* initData = nullptr;

    bool isCubeMap = false;
    bool isTextureArray = false;
    const std::vector<std::string>      & filenames = ITexture::_resource->filenames();
    std::ostringstream fileNameSet;
    std::vector<Ibl::TextureImagePtr>       images;

    if (_resource->type() != Ibl::Procedural)
    {
        for (auto it = filenames.begin(); it != filenames.end(); it++)
        {
            const std::string& filename = (*it);
            fileNameSet << filename.c_str();
            TextureImagePtr image;
            image.reset(new Ibl::TextureImage());
            image->load(filename, std::string());

            if (image)
            {
                images.push_back(image);
            }
            else
            {
                LOG ("Failed to load image " << filename << " " << __LINE__ << " " << __FILE__);
                return false;
            }
        }

        ITexture::_filename = fileNameSet.str().c_str();    
        isTextureArray = images.size() > 1;
        if (images.size() == 0)
        {
            LOG ("Failed to load any images for " << fileNameSet.str() << " " << __LINE__ << " " << __FILE__);
            return false;
        }


        // Check that all of the images are the same.
        if (isTextureArray)
        {
            // Check everyone is the same
            const Ibl::TextureImagePtr& image = images[0];
            for (auto it = images.begin()+1; it != images.end(); it++)
            {
                if ((*it)->getWidth() != image->getWidth() ||
                    (*it)->getHeight() != image->getHeight())
                {
                    LOG ("Failed to load image array due to size mismatch");
                    return false;
                }
                if ((*it)->getFormat() != image->getFormat())
                {
                    LOG ("Failed to load image array due to format mismatch");
                    return false;
                }
            }
        }

        size_t mipMapCount = images[0]->getNumMipmaps();
        if (mipMapCount == 0)
            mipMapCount = 1;

        // Force mip maps.
        bool forceMipMaps = true;


        // HDR loads without the alpha channel data in place.
    
        if (images[0]->hasFlag(IF_CUBEMAP))
        {
            LOG ("Cubemap...");
        }

        PixelFormat flipped = images[0]->getFormat();


        desc.Width = static_cast<UINT>(images[0]->getWidth());
        desc.Height = static_cast<UINT>(images[0]->getHeight());
        desc.MipLevels = static_cast<UINT>( mipMapCount );
        desc.ArraySize = static_cast<UINT>( images[0]->getNumFaces() * images.size());
        desc.Format = findFormat(flipped); 
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = staging ? D3D11_USAGE_STAGING : D3D11_USAGE_DEFAULT;
        desc.BindFlags = staging ? 0 : D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = staging ? D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ : 0;
        desc.MiscFlags = (images[0]->hasFlag(IF_CUBEMAP)) ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

        // Override the format on the resource.
        _format = findFormat(desc.Format);
        _resource->setFormat(_format);

        _resource->setWidth((float)(desc.Width));
        _resource->setHeight((float)(desc.Height));
        _resource->setNumMipLevels(desc.MipLevels);

        initData = new D3D11_SUBRESOURCE_DATA[ images.size() * images[0]->getNumFaces() * mipMapCount ];

        // Load initialization data for subresource information.
        size_t offset = 0;
        for (size_t imageId = 0; imageId < images.size(); imageId++)
        {
            const Ibl::TextureImagePtr& image = images[imageId];
            for (size_t face = 0; face < image->getNumFaces(); face++)
            {
                for (size_t m = 0; m < mipMapCount; m++)
                {
                    size_t outNumBytes = 0;
                    size_t outNumRows = 0;
                    size_t outRowBytes = 0;
                    Ibl::PixelBox box = image->getPixelBox(face, m);

                    GetSurfaceInfo( box.size().x,
                                    box.size().y,
                                    desc.Format,
                                    &outNumBytes,
                                    &outRowBytes,
                                    &outNumRows);
    
                    initData[offset].pSysMem = box.data;
                    initData[offset].SysMemPitch = unsigned int(outRowBytes);
                    initData[offset].SysMemSlicePitch = unsigned int(outNumBytes);

                    offset++;
                }
            }
        }
        isCubeMap = (images[0]->hasFlag(IF_CUBEMAP));
    }
    else
    {
        D3D11_TEXTURE2D_DESC desc;
        desc.ArraySize = 1;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.Format = findFormat(_resource->format());
        desc.Width = _resource->width();
        desc.Height = _resource->height();
        desc.MipLevels = 1;
        desc.MiscFlags = 0;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.Usage = D3D11_USAGE_DYNAMIC;
    }

    HRESULT hr = _direct3d->CreateTexture2D(&desc, initData, &map);
    if (SUCCEEDED(hr))
    {
        map->GetDesc (&_desc);

        _width = _desc.Width;
        _height = _desc.Height;
        _depth = 0;

        if (!staging)
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

            if (isCubeMap)
            {
                    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
                    srvDesc.Format = _desc.Format;
                    srvDesc.TextureCube.MipLevels = _desc.MipLevels;
                    srvDesc.TextureCube.MostDetailedMip = 0;
            }
            else if (isTextureArray)
            {
                srvDesc.Format = _desc.Format;
                srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                srvDesc.Texture2DArray.MipLevels = desc.MipLevels;
                srvDesc.Texture2DArray.ArraySize = textureCount();
            }
            else
            {
                srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D; 
                srvDesc.Format = _desc.Format;
                srvDesc.Texture2D.MipLevels = _desc.MipLevels;
                srvDesc.Texture2D.MostDetailedMip = 0;        
            }

            ID3D11ShaderResourceView * resourceView = nullptr;    
            if (SUCCEEDED (_direct3d->CreateShaderResourceView( map, 
                                                                &srvDesc, 
                                                                &resourceView)))
            {
                setResourceView (resourceView);
                setTexture (map);
                setFormat (_desc.Format);
                safedeletearray(initData);
                return true;
            }
        }
        else
        {
            setResourceView(nullptr);
            setTexture(map);
            setFormat(_desc.Format);
            safedeletearray(initData);
            return true;
        }
    }
    else
    {
        LOG ("Failed to load texture " << this->_filename);
    }
    safedeletearray(initData);
    return false;
}

bool
Texture2DD3D11::isCubeMap() const
{
    return (_desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE) == D3D11_RESOURCE_MISC_TEXTURECUBE;
}

}