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

#include <IblTextureD3D11.h>
#include <IblLog.h>
#include <IblFormatConversionD3D11.h>
#include <IblFilterCubemap.h>
#include <strstream>
namespace Ibl
{

template <typename T>
void
splitChannelsForFormat (TextureImagePtr& src, TextureImagePtr& rgb, TextureImagePtr& mmm)
{
    for(size_t face = 0; face < src->getNumFaces(); face++)
    {
        for (size_t m = 0; m < src->getNumMipmaps(); m++)
        {
            Ibl::PixelBox srcBox = src->getPixelBox(face, m);
            Ibl::PixelBox rgbBox = rgb->getPixelBox(face, m);
            Ibl::PixelBox mmmBox = mmm->getPixelBox(face, m);

            typename T * srcPtr = (typename T*)srcBox.data;
            typename T * rgbPtr = (typename T*)rgbBox.data;
            typename T * mmmPtr = (typename T*)mmmBox.data;

            size_t width = srcBox.size().x;
            size_t height = srcBox.size().y;

            // Take into account row skip alignment.
            for (size_t y = 0; y < height; y++)
            {
                for (size_t x = 0; x < width; x++)
                {
                    // RGBA only sources. 4 channels
                    size_t srcId = ((y * width) + x) * 4;
                    // RGB/BGR only dests (for now). 3 channels
                    size_t dstId = ((y * width) + x) * 3;

                    // Split channels - RGB
                    rgbPtr[dstId+0] = srcPtr[srcId];
                    rgbPtr[dstId+1] = srcPtr[srcId+1];
                    rgbPtr[dstId+2] = srcPtr[srcId+2];

                    // Split channels - MMM
                    mmmPtr[dstId+0] = srcPtr[srcId+3];
                    mmmPtr[dstId+1] = srcPtr[srcId+3];
                    mmmPtr[dstId+2] = srcPtr[srcId+3];
                }
            }
        }
    }
}

// Copy a source mip slice to the top mip of a specified image.
void
copyMip (TextureImagePtr& dst, const TextureImagePtr& src, int32_t mipLevel)
{
    // Copy number of images for specified mip level
    for (uint32_t faceId = 0; faceId < dst->getNumFaces(); faceId++)
    {
        PixelBox dstBox = dst->getPixelBox(faceId);
        PixelBox srcBox = src->getPixelBox(faceId, mipLevel);
        // Copy A to B
        uint8_t * dstPixels = (uint8_t*)dstBox.data;
        uint8_t * srcPixels = (uint8_t*)srcBox.data;
        size_t byteSize = dstBox.size().y * dstBox.rowPitch * dstBox.getNumChannels();
        memcpy (dstPixels, srcPixels, byteSize);
    }
}

//--------------------------------------------------------------------------------------
// Get surface information for a particular format
//--------------------------------------------------------------------------------------
void GetSurfaceInfo(size_t width,
                    size_t height,
                    DXGI_FORMAT fmt,
                    size_t* outNumBytes,
                    size_t* outRowBytes,
                    size_t* outNumRows )
{
    size_t numBytes = 0;
    size_t rowBytes = 0;
    size_t numRows = 0;

    bool bc = false;
    bool packed  = false;
    size_t bcnumBytesPerBlock = 0;
    switch (fmt)
    {
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        bc=true;
        bcnumBytesPerBlock = 8;
        break;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        bc = true;
        bcnumBytesPerBlock = 16;
        break;

    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        packed = true;
        break;
    }

    if (bc)
    {
        size_t numBlocksWide = 0;
        if (width > 0)
        {
            numBlocksWide = std::max<size_t>( 1, (width + 3) / 4 );
        }
        size_t numBlocksHigh = 0;
        if (height > 0)
        {
            numBlocksHigh = std::max<size_t>( 1, (height + 3) / 4 );
        }
        rowBytes = numBlocksWide * bcnumBytesPerBlock;
        numRows = numBlocksHigh;
    }
    else if (packed)
    {
        rowBytes = ( ( width + 1 ) >> 1 ) * 4;
        numRows = height;
    }
    else
    {
        size_t bpp = bitsPerPixel( fmt );
        rowBytes = ( width * bpp + 7 ) / 8; // round up to nearest byte
        numRows = height;
    }

    numBytes = rowBytes * numRows;
    if (outNumBytes)
    {
        *outNumBytes = numBytes;
    }
    if (outRowBytes)
    {
        *outRowBytes = rowBytes;
    }
    if (outNumRows)
    {
        *outNumRows = numRows;
    }
}

TextureD3D11::TextureD3D11(Ibl::DeviceD3D11* device) :
        Ibl::ITexture (device),
        _dxFormat (DXGI_FORMAT_UNKNOWN),
        _resourceView (nullptr),
        _texture (nullptr),
        _mapped (false),
        _direct3d(nullptr),
        _immediateCtx(nullptr),
        _maxValueCached(false)
{ 
    if (Ibl::DeviceD3D11* _device = 
        dynamic_cast <Ibl::DeviceD3D11*>(device))
    {
        _direct3d = *_device;
        _immediateCtx = _device->immediateCtx();
    }

};

TextureD3D11::~TextureD3D11() 
{
    safedelete (_resource);
}

bool 
TextureD3D11::cache()
{
    return true;
}

const Ibl::Vector4f&
TextureD3D11::maxValue() const
{
    if (!_maxValueCached)
    {
        Ibl::TextureImagePtr textureImage(new Ibl::TextureImage());

        const Ibl::TextureParameters* parameters = resource();
        textureImage->create(Ibl::Vector2i(parameters->width(), parameters->height()), 
                             parameters->format(),
                             (uint32_t)parameters->mipLevels(),
                             parameters->dimension() == Ibl::CubeMap ? IF_CUBEMAP : 0);

        Ibl::Vector4f maxValue;

        if (mapForRead())
        {
            size_t dstOffset = 0;
            size_t srcOffset = 0;

            uint8_t * dstData = (uint8_t*)textureImage->getData();
            uint32_t bytesPerPixel = (uint32_t)(bitsPerPixel(findFormat(this->format())) / 8);

            for(size_t face = 0; face < textureImage->getNumFaces(); face++)
            {
                for (size_t m = 0; m < parameters->mipLevels(); m++)
                {
                    map((uint32_t)face, (uint32_t)m);
                    uint8_t * srcData = (uint8_t*)_mappedResource.pData;

                    size_t outNumBytes = 0;
                    size_t outNumRows = 0;
                    size_t outRowBytes = 0;
                    Ibl::PixelBox box = textureImage->getPixelBox(face, m);

                    size_t dstRowPitch = box.size().x * bytesPerPixel;

                    GetSurfaceInfo( box.size().x,
                                    box.size().y,
                                    findFormat(this->format()),
                                    &outNumBytes,
                                    &outRowBytes,
                                    &outNumRows);

                    // Take into account row skip alignment.
                    for (size_t y = 0; y < outNumRows; y++)
                    {
                        // Copy each row for each mipmap
                        memcpy(dstData, srcData, dstRowPitch);

                        // If row stride == row bytestride.
                        // 16 is for d3d11 cards, if d3d10 support is ever added, this must be altered to 
                        // 4 for cards that support it.
                        srcData += _mappedResource.RowPitch;
                        dstData += dstRowPitch;
                    }
                    unmap();
                }
             }
            unmapFromRead();
        }

        // Compute the max value.
        PixelFormat internalFormat = parameters->format();
        float maxValueDistance = 0;
        Ibl::Vector4f zero (0,0,0,0);
        switch (internalFormat)
        {
            case PF_FLOAT32_RGBA:
            case PF_FLOAT32_RGB:
            case PF_FLOAT32_GR:
            case PF_FLOAT32_R:
            {
                for (uint32_t faceId = 0; faceId < textureImage->getNumFaces(); faceId++)
                {
                    PixelBox pixelBox = textureImage->getPixelBox(faceId, 0);

                    float* pixels = (float*)pixelBox.data;
                    for (uint32_t y = 0; y < pixelBox.size().y; y++)
                    {
                        for (uint32_t x = 0; x < pixelBox.size().x; x++)
                        {
                            Ibl::Vector4f testValue(0,0,0,0);
                            memcpy(&testValue.x, &pixels[(((y * pixelBox.size().x) + x) * pixelBox.getNumChannels())], sizeof(float)*pixelBox.getNumChannels());
                            testValue.w = 0.0;
                            // Rescale is color
                            float testDistance = testValue.distance(zero);
                            if (testDistance > maxValueDistance)
                            {
                                _maxValue = testValue;
                                maxValueDistance = testDistance;
                            }
                        }
                    }
                }
                break;
            }
            case PF_FLOAT16_RGBA:
            case PF_FLOAT16_RGB:
            case PF_FLOAT16_GR:
            case PF_FLOAT16_R:
            {
                // TODO: Half support
                LOG("TODO: Unhandled format computing maxvalue. " << __LINE__ << " " << __FILE__)
            }
            case PF_A8B8G8R8:
            case PF_A8R8G8B8:
            case PF_R8G8B8:
            case PF_B8G8R8:
            case PF_R8:
            {
                for (uint32_t faceId = 0; faceId < textureImage->getNumFaces(); faceId++)
                {
                    PixelBox pixelBox = textureImage->getPixelBox(faceId, 0);

                    uint8_t* pixels = (uint8_t*)pixelBox.data;
                    for (uint32_t y = 0; y < pixelBox.size().y; y++)
                    {
                        for (uint32_t x = 0; x < pixelBox.size().x; x++)
                        {
                            Ibl::Vector4f testValue(0,0,0,0);
                            for (uint32_t c = 0; c < pixelBox.getNumChannels(); c++)
                            {
                                testValue[c] = ((float)pixels[(((y * pixelBox.size().x) + x) * pixelBox.getNumChannels())]) / 255.0f;
                            }
                            // Rescale is color
                            float testDistance = testValue.distance(zero);
                            if (testDistance > maxValueDistance)
                            {
                                _maxValue = testValue;
                                maxValueDistance = testDistance;
                            }
                        }
                    }
                }
                break;
            }
            default:
                LOG ("Unhandled format for max value calcuation");
        }

        _maxValueCached = true;
    }
    return _maxValue;
}

Ibl::TextureImagePtr 
TextureD3D11::readImage(Ibl::PixelFormat format, int32_t mipId) const
{
    Ibl::TextureImagePtr textureImage(new Ibl::TextureImage());
    textureImage->create(Ibl::Vector2i(resource()->width(), resource()->height()), 
                         resource()->format(),
                         (uint32_t)resource()->mipLevels(),
                         resource()->dimension() == Ibl::CubeMap ? IF_CUBEMAP : 0);

    bool reverse = resource()->format() == PixelFormat::PF_A8R8G8B8;

    if (mapForRead())
    {
        size_t dstOffset = 0;
        size_t srcOffset = 0;

        uint8_t * dstData = (uint8_t*)textureImage->getData();
        uint32_t bytesPerPixel = (uint32_t)(bitsPerPixel(findFormat(this->format())) / 8);

        for(size_t face = 0; face < textureImage->getNumFaces(); face++)
        {
            for (size_t m = 0; m < resource()->mipLevels(); m++)
            {
                map((uint32_t)face, (uint32_t)m);
                uint8_t * srcData = (uint8_t*)_mappedResource.pData;

                size_t outNumBytes = 0;
                size_t outNumRows = 0;
                size_t outRowBytes = 0;
                Ibl::PixelBox box = textureImage->getPixelBox(face, m);

                size_t dstRowPitch = box.size().x * bytesPerPixel;

                GetSurfaceInfo( box.size().x,
                                box.size().y,
                                findFormat(this->format()),
                                &outNumBytes,
                                &outRowBytes,
                                &outNumRows);

                // Take into account row skip alignment.
                for (size_t y = 0; y < outNumRows; y++)
                {
                    // Copy each row for each mipmap
                    memcpy(dstData, srcData, dstRowPitch);

                    // If row stride == row bytestride.
                    // 16 is for d3d11 cards, if d3d10 support is ever added, this must be altered to 
                    // 4 for cards that support it.
                    srcData += _mappedResource.RowPitch;

                    if (reverse)
                    {
                        for (size_t x = 0; x < box.size().x; x++)
                        {
                            // RGBA8 only.
                            char b = dstData[(x*4)];
                            char g = dstData[(x*4)+1];
                            char r = dstData[(x*4)+2];
                            dstData[(x*4)] = r;
                            dstData[(x*4)+1] = g;
                            dstData[(x*4)+2] = b;
                        }
                    }
                    dstData += dstRowPitch;
                }
                unmap();
            }
         }
        unmapFromRead();
    }
    return textureImage;
}

bool
TextureD3D11::save(const std::string& filePathName,
                   bool fixSeams,
                   bool splitChannels,
                   bool rgbOnly,
                   int32_t mipLevel,
                   const Ibl::ITexture* mergeMap) const
{
    Ibl::TextureImagePtr textureImage(new Ibl::TextureImage());
    Ibl::TextureImagePtr textureImageRGB;
    Ibl::TextureImagePtr textureImageMMM;
    Ibl::TextureImagePtr mipImage;

    PixelFormat threeChannelFormat = PF_UNKNOWN;

    const Ibl::TextureParameters* parameters = resource();
    textureImage->create(Ibl::Vector2i(parameters->width(), parameters->height()), 
                         parameters->format(),
                         (uint32_t)parameters->mipLevels(),
                         parameters->dimension() == Ibl::CubeMap ? IF_CUBEMAP : 0);

    PixelFormat internalFormat = parameters->format();
    size_t extension = filePathName.rfind(".");

    if (internalFormat == PF_FLOAT32_RGBA)
    {
        threeChannelFormat = PF_FLOAT32_RGB;
    }
    else if (internalFormat == PF_FLOAT16_RGBA)
    {
        threeChannelFormat = PF_FLOAT16_RGB;
    }
    else if (internalFormat == PF_A8B8G8R8)
    {
        threeChannelFormat = PF_B8G8R8;
    }
    else if (internalFormat == PF_A8R8G8B8)
    {
        threeChannelFormat = PF_R8G8B8;
    }

    if (splitChannels || rgbOnly)
    {
        if (threeChannelFormat != PF_UNKNOWN)
        {
            const Ibl::TextureParameters* parameters = resource();
            textureImageRGB.reset(new Ibl::TextureImage());
            textureImageRGB->create(Ibl::Vector2i(parameters->width(), parameters->height()), 
                                    threeChannelFormat,
                                    (uint32_t)parameters->mipLevels(),
                                    parameters->dimension() == Ibl::CubeMap ? IF_CUBEMAP : 0);
            textureImageMMM.reset(new Ibl::TextureImage());
            textureImageMMM->create(Ibl::Vector2i(parameters->width(), parameters->height()), 
                                    threeChannelFormat,
                                    (uint32_t)parameters->mipLevels(),
                                    parameters->dimension() == Ibl::CubeMap ? IF_CUBEMAP : 0);
        }
        else
        {
            splitChannels = false;
            rgbOnly = false;
        }
    }

    if (mipLevel != -1)
    {
        Ibl::PixelBox sourceBox = textureImage->getPixelBox(0, mipLevel);
        Ibl::PixelFormat format = sourceBox.format;
        if (splitChannels || rgbOnly)
        {
            format = threeChannelFormat;
        }

        mipImage = TextureImagePtr(new Ibl::TextureImage());
        mipImage->create(Ibl::Vector2i((int32_t)sourceBox.size().x, 
                                       (int32_t)sourceBox.size().y),
                         format, 1, 
                         parameters->dimension() == Ibl::CubeMap ? IF_CUBEMAP : 0);
    }


    // Stupid bug when writing dds.
    bool reverse = parameters->format() == Ibl::PF_A8R8G8B8;

    if (mapForRead())
    {
        size_t dstOffset = 0;
        size_t srcOffset = 0;

        uint8_t * dstData = (uint8_t*)textureImage->getData();
        size_t bytesPerPixel = bitsPerPixel(findFormat(this->format())) / 8;

        for(size_t face = 0; face < textureImage->getNumFaces(); face++)
        {
            for (size_t m = 0; m < parameters->mipLevels(); m++)
            {
                map((uint32_t)face, (uint32_t)(m));
                uint8_t * srcData = (uint8_t*)_mappedResource.pData;

                size_t outNumBytes = 0;
                size_t outNumRows = 0;
                size_t outRowBytes = 0;
                Ibl::PixelBox box = textureImage->getPixelBox(face, m);

                size_t dstRowPitch = box.size().x * bytesPerPixel;

                GetSurfaceInfo( box.size().x,
                                box.size().y,
                                findFormat(this->format()),
                                &outNumBytes,
                                &outRowBytes,
                                &outNumRows);

                // Take into account row skip alignment.
                for (size_t y = 0; y < outNumRows; y++)
                {
                    // Copy each row for each mipmap
                    memcpy(dstData, srcData, dstRowPitch);

                    // If row stride == row bytestride.
                    // 16 is for d3d11 cards, if d3d10 support is ever added, this must be altered to 
                    // 4 for cards that support it.
                    srcData += _mappedResource.RowPitch;

                    if (reverse)
                    {
                        for (size_t x = 0; x < box.size().x; x++)
                        {
                            // RGBA8 only.
                            char b = dstData[(x*4)];
                            char g = dstData[(x*4)+1];
                            char r = dstData[(x*4)+2];
                            dstData[(x*4)] = r;
                            dstData[(x*4)+1] = g;
                            dstData[(x*4)+2] = b;
                        }
                    }

                    dstData += dstRowPitch;
                }
                unmap();
            }
         }
        unmapFromRead();

        // Merge the merge map into texture Image.
        if (const TextureD3D11* mergeMapD3D11 = dynamic_cast<const TextureD3D11*>(mergeMap))
        {
            // Create a temporary image based on the merge map
            TextureImagePtr mergeImage = TextureImagePtr(new Ibl::TextureImage());

            // Find the mip that corresponds to this image.
            Ibl::Vector2i mergeSize = Ibl::Vector2i(mergeMapD3D11->resource()->width(), mergeMapD3D11->resource()->height());
            int32_t dstMipId = -1;

            // Should assert on non power of 2 here.
            size_t sourceWidth = textureImage->getWidth();
            for (uint32_t mipId = 0; mipId < textureImage->getNumMipmaps(); mipId++)
            {
                if (sourceWidth == mergeSize.x)
                {
                    dstMipId = mipId;
                    break;
                }
                sourceWidth = sourceWidth >> 1;
            }

            if (dstMipId >= 0)
            {
                Ibl::PixelFormat format;
                {
                    Ibl::PixelBox dstBox = textureImage->getPixelBox(0, dstMipId);
                    format = dstBox.format;
                }
                // Create merge image
                // mergeImage->create(mergeSize, format, 1, parameters->dimension() == Ibl::CubeMap ? IF_CUBEMAP : 0);
                // Get merge image out of the d3d11 texture

                // MipId and format.
                mergeImage = mergeMapD3D11->readImage(format, -1);

                // Copy the faces from the merge image to the mip map for all faces.
                for (uint32_t mergeFaceId = 0; mergeFaceId < 6; mergeFaceId++)
                {
                    Ibl::PixelBox srcBox = mergeImage->getPixelBox(mergeFaceId, 0);
                    Ibl::PixelBox dstBox = textureImage->getPixelBox(mergeFaceId, dstMipId);
                    memcpy(dstBox.data, srcBox.data, mergeSize.x * mergeSize.y * 4); // Fix bytes per pixel code.
                }
            }
        }

        if (this->isCubeMap())
        {
            if (parameters->format() == Ibl::PF_A8R8G8B8)
            {
                for (uint32_t mipId = 0; mipId < textureImage->getNumMipmaps(); mipId++)
                {
                    PixelBox mip = textureImage->getPixelBox();
                    fixupCubeEdges <uint8_t> (textureImage, 
                                              mipId,
                                              CP_FIXUP_AVERAGE_HERMITE, 
                                              Ibl::maxValue(mip.size().x * 0.015f, 1.0f));
                }
            }
            else if (parameters->format() == PixelFormat::PF_FLOAT32_RGBA)
            {
                for (uint32_t mipId = 0; mipId < textureImage->getNumMipmaps(); mipId++)
                {
                    PixelBox mip = textureImage->getPixelBox();
                    fixupCubeEdges <float> (textureImage, 
                                            mipId,
                                            CP_FIXUP_AVERAGE_HERMITE, 
                                            Ibl::maxValue(mip.size().x * 0.015f, 1.0f));
                }
            }
        }

        // TODO: Filter for cubemap.
        if (splitChannels || rgbOnly)
        {
            // threeChannelFormat.
            // Split textures into RGB, MMM.
            switch (threeChannelFormat)
            {
                case PF_FLOAT32_RGB:
                    splitChannelsForFormat<typename float>(textureImage, textureImageRGB, textureImageMMM);
                    break;
                case PF_FLOAT16_RGB:
                    splitChannelsForFormat<typename uint16_t>(textureImage, textureImageRGB, textureImageMMM);
                    break;
                case PF_R8G8B8:
                case PF_B8G8R8:
                    splitChannelsForFormat<typename uint8_t>(textureImage, textureImageRGB, textureImageMMM);
                    break;
            }

            std::string mmmImageFilePathName = filePathName.substr(0, extension) + "MMM.dds";
            std::string rgbImageFilePathName = filePathName.substr(0, extension) + "RGB.dds";

            // Save images.
            if(rgbOnly)
            {
                if (mipLevel != -1)
                {
                    char mipImageFilePathName[512];
                    memset(mipImageFilePathName, 0, 512);
                    sprintf_s(mipImageFilePathName, "%s%d%s", filePathName.substr(0, extension).c_str(), mipLevel, "RGB.dds");
                    // Copy texture image into mip image.
                    copyMip (mipImage, textureImageRGB, mipLevel);
                    mipImage->save(std::string(mipImageFilePathName));
                }
                else
                {
                    textureImageRGB->save(filePathName);
                }
            }
            else if (splitChannels)
            {
                if (mipLevel != -1)
                {
                    char mipImageFilePathName[512];
                    memset(mipImageFilePathName, 0, 512);
                    sprintf_s(mipImageFilePathName, "%s%d%s", filePathName.substr(0, extension).c_str(), mipLevel, "RGB.dds");

                    // Copy texture image into mip image.
                    copyMip (mipImage, textureImageRGB, mipLevel);
                    mipImage->save(std::string(mipImageFilePathName));

                    memset(mipImageFilePathName, 0, 512);
                    sprintf_s(mipImageFilePathName, "%s%d%s", filePathName.substr(0, extension).c_str(), mipLevel, "MMM.dds");
                    // Copy texture image into mip image.
                    copyMip (mipImage, textureImageMMM, mipLevel);
                    mipImage->save(mipImageFilePathName);
                }
                else
                {
                    textureImageRGB->save(rgbImageFilePathName);
                    textureImageMMM->save(mmmImageFilePathName);
                }
            }
        }
        else
        {
            if (mipLevel != -1)
            {
                char mipImageFilePathName[512];
                memset(mipImageFilePathName, 0, 512);
                sprintf_s(mipImageFilePathName, "%s%d%s", filePathName.substr(0, extension).c_str(), mipLevel, ".dds");

                // Copy texture image into mip image.
                copyMip (mipImage, textureImage, mipLevel);
                mipImage->save(mipImageFilePathName);
            }
            else
            {
                textureImage->save(filePathName);
            }
        }
        return true;
    }
    else
    {
        LOG ("Failed to save texture " << filePathName);
        return false;
    }
}

bool 
TextureD3D11::create () 
{
    return true;
}
    
bool 
TextureD3D11::free() 
{
    saferelease (_texture);
    saferelease (_resourceView);
    return true;
}

ID3D11ShaderResourceView * 
TextureD3D11::resourceView(int32_t arrayId, int32_t mipId) const 
{
    return _resourceView;
}

bool
TextureD3D11::initialize(const Ibl::TextureParameters* data)
{
    if (!_resource)
    {
        safedelete (_resource);
        _resource = new Ibl::TextureParameters (*data);
        _width = _resource->width();
        _height = _resource->height();
        _format = _resource->format();
                
        //_channels = getChannelsForFormat(_format);

        _depth = _resource->depth();
        _textureCount = _resource->textureCount();
            
        _multiSampleCount = _resource->multiSampleCount();
        _multiSampleQuality = _resource->multiSampleQuality();
    }
    return create();
}


ID3D11Resource *
TextureD3D11::texture() const 
{ 
    return _texture; 
}

void
TextureD3D11::setTexture (ID3D11Resource * textureArg) 
{
    _texture = textureArg;
}

void
TextureD3D11::setResourceView (ID3D11ShaderResourceView * shaderResourceView)
{
    _resourceView = shaderResourceView;
}

bool
TextureD3D11::bindSurface(int renderTargetIndex) const
{
    return true;
}
    
bool
TextureD3D11::clearSurface (uint32_t layerId, float r, float g, float b, float a) 
{
    return true;
}

void
TextureD3D11::generateMipMaps() const 
{
    // Linear sampling. Use shader to resolve per face per mip.
    /*
    if (isCubeMap())
    {
        for (size_t faceId = 0; faceId < 6; faceId++)
        {
            size_t mipLevels = resource()->mipLevels();
            const ISurface * src = surface(faceId, 0);

            for (size_t mipId = 1; mipId < mipLevels; mipId++)
            {
                const ISurface * dst = surface(faceId, mipId);
                _deviceInterface->blitSurfaces(dst, src);
            }
        }
    }
    // Dicks, Dicks everywhere.
    else
    */
    {
        _immediateCtx->GenerateMips (resourceView());
    }
}

DXGI_FORMAT
TextureD3D11::dxFormat() const 
{ 
    return _dxFormat; 
}

bool
TextureD3D11::mapForWrite()
{
    if (SUCCEEDED(_immediateCtx->Map(texture(), 0, D3D11_MAP_WRITE_DISCARD, 0, &_mappedResource)))
    {
        _mapped = true;
        return true;
    }
    return false;
}

bool
TextureD3D11::mapForRead() const 
{
    throw (std::runtime_error("mapForRead() Texture read unimplemented at this level!"));
    return false;
}

bool
TextureD3D11::unmapFromRead() const
{
    throw (std::runtime_error("unmapFromRead() Texture read unimplemented at this level!"));
    return false;
}

bool
TextureD3D11::map(uint32_t level, uint32_t mipmap) const
{
    throw (std::runtime_error("map() Texture read unimplemented at this level!"));
    return false;
}

bool
TextureD3D11::unmap() const
{
    throw (std::runtime_error("umap() Texture read unimplemented at this level!"));
    return false;
}

bool
TextureD3D11::writeSubRegion(const Ibl::byte* srcPtr, uint32_t offsetX, uint32_t offsetY, uint32_t w, uint32_t h, uint32_t bytesPerPixel)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    uint32_t srcRowPitch = bytesPerPixel * width();
    uint32_t rowCopyPitch = width() * bytesPerPixel;

    if (SUCCEEDED(_immediateCtx->Map(texture(), 0,
        D3D11_MAP_WRITE, 0,
        &mappedResource)))
    {
        for (uint32_t rowId = offsetY; rowId < h; rowId++)
        {
            Ibl::byte* dstPtr = (Ibl::byte*)mappedResource.pData + ((rowId * srcRowPitch) + (offsetX * bytesPerPixel));
            memcpy(dstPtr, srcPtr, rowCopyPitch);
            srcPtr += w * bytesPerPixel;
        }

        _immediateCtx->Unmap(texture(), 0);

        LOG("Success in write procedural texture");
        return true;
    }
    else
    {
        LOG("Failed to write procedural texture");
        return false;
    }
}

bool
TextureD3D11::write (const Ibl::byte* ptr)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (SUCCEEDED(_immediateCtx->Map (texture(), 0, 
                                      D3D11_MAP_WRITE_DISCARD, 0, 
                                      &mappedResource)))
    {
        size_t bytesize = byteSize();
        BYTE* pSrc = (BYTE*)ptr;
        BYTE* pDest = (BYTE*)mappedResource.pData;
        
        // LOG("Bytesize is " << byteSize() << " mapped " << mappedResource.DepthPitch <<  " " << mappedResource.RowPitch);
        memcpy( pDest, pSrc, byteSize());

        _immediateCtx->Unmap(texture(), 0);

        LOG ("Success in write procedural texture");
        return true;
    }
    else
    {
        LOG ("Failed to write procedural texture");
    }
    
    return false;
}

bool
TextureD3D11::write (const Ibl::Vector4f&, const Ibl::Vector2f&) 
{

    // Todo, fill in.
    return false;
}

Ibl::Vector4f
TextureD3D11::read (const Ibl::Vector2i& pos)  const
{
#if 0 // mdavidson. Come back to [TODO]
    Ibl::Vector4f pixel;
    if (_mapped)
    {
        int bpp = _channels * sizeof(Ibl::byte); // Todo, alloc to correct size
        int y = pos.x; 
        int x = pos.y; 

        int index = (int)(bpp * ((width() * y) + x)); 
        if (index > 0 && index < (bpp * (int)(width() * height())))
        {
            // Read native
            char * srcPixel = new char[bytesPerPixel()];

            memcpy (&srcPixel[0], &((Ibl::byte*)_mappedResource.pData)[index], bpp);
            Ibl::convertPixels((const char*)&srcPixel[0], _pixelType, _channels, (char*)&pixel[0], FloatPixel, 4); 
            safedeletearray(srcPixel);
        }
        
        return pixel;
    }
    else
    {
        // Should throw
        std::ostringstream error;
        error << "You must map the texture before reading pixels! " << __FILE__ << " " << __LINE__;
        throw std::runtime_error (error.str());
    }
    return pixel;

#endif
    return Ibl::Vector4f(0,0,0,0);
}

Ibl::Vector4f
TextureD3D11::read (const Ibl::Vector2f& pos) const
{
/* mdavidson, TODO:
    Ibl::Vector4f pixel;
    if (_mapped)
    {
        int bpp = 4 * sizeof(Ibl::byte); // Todo, alloc to correct size
        int y = int(std::min(pos.y, 1.0f) * (float)(height()));
        int x = int(std::min(pos.x, 1.0f) * (float)(width()));

        int index = (int)(bpp * ((width() * y) + x)); 

        if (index > 0 && index < (bpp * (int)(width() * height())))
        {
            // Read native
            char * srcPixel = new char[bytesPerPixel()];

            memcpy (&srcPixel[0], &((Ibl::byte*)_mappedResource.pData)[index], bpp);
            Ibl::convertPixels((const char*)&srcPixel[0], _pixelType, _channels, (char*)&pixel[0], FloatPixel, 4); 
            safedeletearray(srcPixel);
        }
        
        return pixel;
    }
    else
    {
        // Should throw
        std::ostringstream error;
        error << "You must map the texture before reading pixels! " << __FILE__ << " " << __LINE__;
        throw std::runtime_error (error.str());
    }

    return pixel;
*/
    return Ibl::Vector4f(0,0,0,0);
}

Ibl::Vector4f
TextureD3D11::read (Ibl::byte* pos)  const
{
    Ibl::Vector4f pixel;
    // TODO, fill in.
    return pixel;
}

void
TextureD3D11::setFormat (DXGI_FORMAT format) 
{ 
    _dxFormat = format; 
    switch (_dxFormat)
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    {
        _channels = 4;
        _pixelChannelPitch = sizeof(float);
        break;
    }
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    {
        _channels = 4;
        _pixelChannelPitch = sizeof(float);
        break;
    }
    case DXGI_FORMAT_R32G32B32A32_UINT:
    {
        _channels = 4;
        _pixelChannelPitch = sizeof(unsigned);
        break;
    }
    case DXGI_FORMAT_R32G32B32A32_SINT:
    {
        _channels = 4;
        _pixelChannelPitch = sizeof(signed);
        break;
    }
    case DXGI_FORMAT_R32G32B32_TYPELESS:
    {
        _channels = 3;
        _pixelChannelPitch = sizeof(float);
        break;
    }
    case DXGI_FORMAT_R32G32B32_FLOAT:
    {
        _channels = 3;
        _pixelChannelPitch = sizeof(float);
        break;
    }
    case DXGI_FORMAT_R32G32B32_UINT:
    {
        _channels = 3;
        _pixelChannelPitch = sizeof(unsigned);
        break;
    }
    case DXGI_FORMAT_R32G32B32_SINT:
    {
        _channels = 3;
        _pixelChannelPitch = sizeof(signed);
        break;
    }
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    {
        _channels = 4;
        _pixelChannelPitch = sizeof(unsigned short);
        break;
    }
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    {
        _channels = 4;
        _pixelChannelPitch = sizeof(unsigned short);
        break;
    }
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    {
        _channels = 4;
        _pixelChannelPitch = sizeof(unsigned short);
        break;
    }
    case DXGI_FORMAT_R16G16B16A16_UINT:
    {
        _channels = 4;
        _pixelChannelPitch = sizeof(unsigned short);
        break;
    }
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    {
        _channels = 4;
        _pixelChannelPitch = sizeof(unsigned short);
        break;
    }
    case DXGI_FORMAT_R16G16B16A16_SINT:
    {
        _channels = 4;
        _pixelChannelPitch = sizeof(short);
        break;
    }
    case DXGI_FORMAT_R32G32_TYPELESS:
    {
        _channels = 2;
        _pixelChannelPitch = sizeof(float);
        break;
    }
    case DXGI_FORMAT_R32G32_FLOAT:
    {
        _channels = 2;
        _pixelChannelPitch = sizeof(float);
        break;
    }
    case DXGI_FORMAT_R32G32_UINT:
    {
        _channels = 2;
        _pixelChannelPitch = sizeof(unsigned);
        break;
    }
    case DXGI_FORMAT_R32G32_SINT:
    {
        _channels = 2;
        _pixelChannelPitch = sizeof(signed);
        break;
    }
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    {
        _channels = 2;
        _pixelChannelPitch = sizeof(unsigned);
        break;
    }
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    {
        _channels = 2;
        _pixelChannelPitch = sizeof(unsigned);
        break;
    }
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    {
        _channels = 2;
        _pixelChannelPitch = sizeof(unsigned);
        break;
    }
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned);
        break;
    }
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned);
        break;
    }
    case DXGI_FORMAT_R10G10B10A2_UINT:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned);
        break;
    }
    case DXGI_FORMAT_R11G11B10_FLOAT:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned);
        break;
    }
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    {
        _channels = 4;
        _pixelChannelPitch = sizeof(unsigned char);
        break;
    }
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    {
        _channels = 4;
        _pixelChannelPitch = sizeof(unsigned char);
        break;
    }
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    {
        _channels = 4;
        _pixelChannelPitch = sizeof(unsigned char);
        break;
    }
    case DXGI_FORMAT_R8G8B8A8_UINT:
    {
        _channels = 4;
        _pixelChannelPitch = sizeof(unsigned char);
         break;
    }
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    {
        _channels = 4;
        _pixelChannelPitch = sizeof(unsigned char);
        break;
    }
    case DXGI_FORMAT_R8G8B8A8_SINT:
    {
        _channels = 4;
        _pixelChannelPitch = sizeof(signed char);
        break;
    }
    case DXGI_FORMAT_R16G16_TYPELESS:
    {
        _channels = 2;
        _pixelChannelPitch = sizeof(short);
        break;
    }
    case DXGI_FORMAT_R16G16_FLOAT:
    {
        _channels = 2;
        _pixelChannelPitch = sizeof(short);
        break;
    }
    case DXGI_FORMAT_R16G16_UNORM:
    {
        _channels = 2;
        _pixelChannelPitch = sizeof(short);
        break;
    }
    case DXGI_FORMAT_R16G16_UINT:
    {
        _channels = 2;
        _pixelChannelPitch = sizeof(short);
        break;
    }
    case DXGI_FORMAT_R16G16_SNORM:
    {
        _channels = 2;
        _pixelChannelPitch = sizeof(short);
        break;
    }
    case DXGI_FORMAT_R16G16_SINT:
    {
        _channels = 2;
        _pixelChannelPitch = sizeof(short);
        break;
    }
    case DXGI_FORMAT_R32_TYPELESS:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(float);
        break;
    }
    case DXGI_FORMAT_D32_FLOAT:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(float);
        break;
    }
    case DXGI_FORMAT_R32_FLOAT:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(float);
        break;
    }
    case DXGI_FORMAT_R32_UINT:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned);
        break;
    }
    case DXGI_FORMAT_R32_SINT:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(signed);
        break;
    }
    case DXGI_FORMAT_R24G8_TYPELESS:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned);
        break;
    }
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned);
        break;
    }
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned);
        break;
    }
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned);
        break;
    }
    case DXGI_FORMAT_R8G8_TYPELESS:
    {
        _channels = 2;
        _pixelChannelPitch = sizeof(unsigned char);
        break;
    }
    case DXGI_FORMAT_R8G8_UNORM:
    {
        _channels = 2;
        _pixelChannelPitch = sizeof(unsigned char);
        break;
    }
    case DXGI_FORMAT_R8G8_UINT:
    {
        _channels = 2;
        _pixelChannelPitch = sizeof(unsigned char);
        break;
    }
    case DXGI_FORMAT_R8G8_SNORM:
    {
        _channels = 2;
        _pixelChannelPitch = sizeof(unsigned char);
        break;
    }
    case DXGI_FORMAT_R8G8_SINT:
    {
        _channels = 2;
        _pixelChannelPitch = sizeof(unsigned char);
        break;
    }
    case DXGI_FORMAT_R16_TYPELESS:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned short);
        break;
    }
    case DXGI_FORMAT_R16_FLOAT:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned short);
        break;
    }
    case DXGI_FORMAT_D16_UNORM:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned short);
        break;
    }
    case DXGI_FORMAT_R16_UNORM:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned short);
        break;
    }
    case DXGI_FORMAT_R16_UINT:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned short);
        break;
    }
    case DXGI_FORMAT_R16_SNORM:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned short);
        break;
    }
    case DXGI_FORMAT_R16_SINT:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(short);
        break;
    }
    case DXGI_FORMAT_R8_TYPELESS:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned char);
        break;
    }
    case DXGI_FORMAT_R8_UNORM:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned char);
        break;
    }
    case DXGI_FORMAT_R8_UINT:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned char);
        break;
    }
    case DXGI_FORMAT_R8_SNORM:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned char);
        break;
    }
    case DXGI_FORMAT_R8_SINT:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(signed char);
        break;
    }
    case DXGI_FORMAT_A8_UNORM:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(unsigned char);
        break;
    }
    case DXGI_FORMAT_R1_UNORM:
    {
        _channels = 1;
        _pixelChannelPitch = sizeof(bool);
        break;
    }
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    {
        _channels = 4;
        _pixelChannelPitch = sizeof(unsigned char);
        break;
    }
    default:
        LOG ("Unsupported or unknown format " << _dxFormat);
    }

    // LOG ("Channels = " << _channels << " pitch = " << _pixelChannelPitch << " bytes = " << byteSize() << "\n");
}

}
