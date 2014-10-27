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
/* Based on code from the OGRE engine:
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2012 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include <IblTextureImage.h>
#include <IblImageCodec.h>
#include <IblColorValue.h>
#include <IblMath.h>
#include <IblAssetManager.h>
#include <IblLog.h>

namespace Ibl
{

ImageCodec::~ImageCodec() 
{
}

std::ostream& 
operator << (std::ostream &o, const Ibl::TextureImage& image)
{
    return (o << " w: " << image.getWidth() << " h:" << image.getHeight() << " bpp " << (int)(image.getBPP()));
}

TextureImage::TextureImage()
    : mWidth(0),
    mHeight(0),
    mDepth(0),
    mBufSize(0),
    mNumMipmaps(0),
    mFlags(0),
    mFormat(PF_UNKNOWN),
    mBuffer( nullptr ),
    mAutoDelete( true )
{
}

TextureImage::TextureImage( const TextureImage &img )
    : mBuffer( nullptr ),
    mAutoDelete( true )
{
    // call assignment operator
    *this = img;
}

TextureImage::~TextureImage()
{
    freeMemory();
}

void TextureImage::freeMemory()
{
    //Only delete if this was not a dynamic image (meaning app holds & destroys buffer)
    if( mBuffer && mAutoDelete )
    {
        free(mBuffer);
        mBuffer = nullptr;
    }

}

TextureImage & TextureImage::operator = ( const TextureImage &img )
{
    freeMemory();
    mWidth = img.mWidth;
    mHeight = img.mHeight;
    mDepth = img.mDepth;
    mFormat = img.mFormat;
    mBufSize = img.mBufSize;
    mFlags = img.mFlags;
    mPixelSize = img.mPixelSize;
    mNumMipmaps = img.mNumMipmaps;
    mAutoDelete = img.mAutoDelete;
    //Only create/copy when previous data was not dynamic data
    if( mAutoDelete )
    {
        mBuffer = (uint8_t*)malloc(mBufSize * sizeof(uint8_t));
        memcpy( mBuffer, img.mBuffer, mBufSize );
    }
    else
    {
        mBuffer = img.mBuffer;
    }

    return *this;
}

TextureImage & TextureImage::flipAroundY()
{
    if( !mBuffer )
    {
        throw(std::exception("Can not flip an unitialized texture TextureImage::flipAroundY"));
    }
    
     mNumMipmaps = 0; // TextureImage operations lose precomputed mipmaps

    uint8_t    *pTempBuffer1 = nullptr;
    uint16_t    *pTempBuffer2 = nullptr;
    uint8_t    *pTempBuffer3 = nullptr;
    uint32_t    *pTempBuffer4 = nullptr;

    uint8_t    *src1 = mBuffer, *dst1 = nullptr;
    uint16_t    *src2 = (uint16_t *)mBuffer, *dst2 = nullptr;
    uint8_t    *src3 = mBuffer, *dst3 = nullptr;
    uint32_t *src4 = (uint32_t *)mBuffer, *dst4 = nullptr;

    uint16_t y;
    switch (mPixelSize)
    {
    case 1:
        pTempBuffer1 = (uint8_t*)malloc(mWidth * mHeight * sizeof(uint8_t));
        for (y = 0; y < mHeight; y++)
        {
            dst1 = (pTempBuffer1 + ((y * mWidth) + mWidth - 1));
            for (uint16_t x = 0; x < mWidth; x++)
                memcpy(dst1--, src1++, sizeof(uint8_t));
        }

        memcpy(mBuffer, pTempBuffer1, mWidth * mHeight * sizeof(uint8_t));
        free(pTempBuffer1);
        break;

    case 2:
        pTempBuffer2 = (uint16_t*)malloc(sizeof(uint16_t) * mWidth * mHeight);
        for (y = 0; y < mHeight; y++)
        {
            dst2 = (pTempBuffer2 + ((y * mWidth) + mWidth - 1));
            for (uint16_t x = 0; x < mWidth; x++)
                memcpy(dst2--, src2++, sizeof(uint16_t));
        }

        memcpy(mBuffer, pTempBuffer2, mWidth * mHeight * sizeof(uint16_t));
        free(pTempBuffer2);
        break;

    case 3:
        pTempBuffer3 = (uint8_t*)malloc(mWidth * mHeight * 3 * sizeof(uint8_t));
        for (y = 0; y < mHeight; y++)
        {
            size_t offset = ((y * mWidth) + (mWidth - 1)) * 3;
            dst3 = pTempBuffer3;
            dst3 += offset;
            for (size_t x = 0; x < mWidth; x++)
            {
                memcpy(dst3, src3, sizeof(uint8_t) * 3);
                dst3 -= 3; src3 += 3;
            }
        }

        memcpy(mBuffer, pTempBuffer3, mWidth * mHeight * sizeof(uint8_t) * 3);
        free(pTempBuffer3);
        break;

    case 4:
        pTempBuffer4 = (uint32_t*)malloc(sizeof(uint32_t) *  mWidth * mHeight);
        for (y = 0; y < mHeight; y++)
        {
            dst4 = (pTempBuffer4 + ((y * mWidth) + mWidth - 1));
            for (uint16_t x = 0; x < mWidth; x++)
                memcpy(dst4--, src4++, sizeof(uint32_t));
        }

        memcpy(mBuffer, pTempBuffer4, mWidth * mHeight * sizeof(uint32_t));
        free(pTempBuffer4);
        break;

    default:
        throw( std::exception("Unknown pixel depth TextureImage::flipAroundY" ));
        break;
    }

    return *this;

}

TextureImage & TextureImage::flipAroundX()
{
    if( !mBuffer )
    {
        throw(std::exception( "Can not flip an unitialized texture TextureImage::flipAroundX" ));
    }
    
    mNumMipmaps = 0; // TextureImage operations lose precomputed mipmaps

    size_t rowSpan = mWidth * mPixelSize;

    uint8_t *pTempBuffer = (uint8_t*)malloc(rowSpan * mHeight * sizeof(uint8_t));
    uint8_t *ptr1 = mBuffer, *ptr2 = pTempBuffer + ( ( mHeight - 1 ) * rowSpan );

    for( uint16_t i = 0; i < mHeight; i++ )
    {
        memcpy( ptr2, ptr1, rowSpan );
        ptr1 += rowSpan; ptr2 -= rowSpan;
    }

    memcpy( mBuffer, pTempBuffer, rowSpan * mHeight);

    free(pTempBuffer);

    return *this;
}

TextureImage& TextureImage::loadDynamicTextureImage(uint8_t* pData, size_t uWidth, size_t uHeight, 
                                                    size_t depth,
                                                    PixelFormat eFormat, 
                                                    bool autoDelete, 
                                                    size_t numFaces, size_t numMipMaps)
{

    freeMemory();
    // Set image metadata
    mWidth = uWidth;
    mHeight = uHeight;
    mDepth = depth;
    mFormat = eFormat;
    mPixelSize = static_cast<uint8_t>(PixelUtil::getNumElemBytes( mFormat ));
    mNumMipmaps = numMipMaps;
    mFlags = 0;
    // Set flags
    if (PixelUtil::isCompressed(eFormat))
        mFlags |= IF_COMPRESSED;
    if (mDepth != 1)
        mFlags |= IF_3D_TEXTURE;
    if(numFaces == 6)
        mFlags |= IF_CUBEMAP;
    if(numFaces != 6 && numFaces != 1)
        throw(std::exception("Number of faces currently must be 6 or 1. TextureImage::loadDynamicTextureImage"));

    mBufSize = calculateSize(numMipMaps, numFaces, uWidth, uHeight, depth, eFormat);
    mBuffer = pData;
    mAutoDelete = autoDelete;

    return *this;

}

TextureImage & TextureImage::loadRawData(
    DataStreamPtr& stream, 
    size_t uWidth, size_t uHeight, size_t uDepth,
    PixelFormat eFormat,
    size_t numFaces, size_t numMipMaps)
{
    size_t size = calculateSize(numMipMaps, numFaces, uWidth, uHeight, uDepth, eFormat);
    if (size != stream->size())
    {
        throw(std::exception("Stream size does not match calculated image size TextureImage::loadRawData"));
    }

    uint8_t *buffer = (uint8_t*)malloc(sizeof(uint8_t) * size);
    stream->read(buffer, size);

    return loadDynamicTextureImage(buffer,
        uWidth, uHeight, uDepth,
        eFormat, true, numFaces, numMipMaps);

}

TextureImage & TextureImage::load(const std::string& strFileName, const std::string& group)
{

    std::string strExt;

    size_t pos = strFileName.rfind(".");
    if( pos != std::string::npos && pos < (strFileName.length() - 1))
    {
        strExt = strFileName.substr(pos+1, strFileName.size()-(pos+1));
    }

    LOG ("TextureImage Load " << strExt );

    std::unique_ptr<typename DataStream> dataStream =
        std::unique_ptr<typename DataStream>(Ibl::AssetManager::assetManager()->openStream(strFileName));

    if (dataStream)
    {
        return load (dataStream, strExt);
    }
    else
    {
        LOG ("Failed to load image " + strFileName);
        throw (std::exception("Failed to load image "));
    }

    THROW (__LINE__ << " " << __FILE__ << " " << " is not implemented!!!");
}

void TextureImage::save(const std::string& filename)
{
    if( !mBuffer )
    {
        throw(std::exception("No image data loaded - TextureImage::save"));
    }

    std::string strExt;
    size_t pos = filename.rfind(".");
    if( pos == std::string::npos )
        throw(std::exception("Unable to save image file invalid extension. TextureImage::save" ));

    while( pos != filename.length() - 1 )
        strExt += filename.c_str()[++pos];

    Codec * pCodec = Codec::getCodec(strExt);
    if( !pCodec )
        throw(std::exception("Unable to save image file  - invalid extension. TextureImage::save" ));

    ImageCodec::ImageData* imgData = new ImageCodec::ImageData();
    imgData->format = mFormat;
    imgData->height = mHeight;
    imgData->width = mWidth;
    imgData->depth = mDepth;
    imgData->size = mBufSize;
    imgData->num_images = mFlags&IF_CUBEMAP?6:1;
    imgData->num_mipmaps = (uint16_t)(mNumMipmaps);

    // Wrap in CodecDataPtr, this will delete
    Codec::CodecDataPtr codeDataPtr(imgData);
    // Wrap memory, be sure not to delete when stream destroyed
    MemoryDataStreamPtr wrapper(new MemoryDataStream(mBuffer, mBufSize, false));

    pCodec->codeToFile(wrapper, filename, codeDataPtr);
}

DataStreamPtr TextureImage::encode(const std::string& formatextension)
{
    if( !mBuffer )
    {
        throw(std::exception("No image data loaded TextureImage::encode"));
    }

    Codec * pCodec = Codec::getCodec(formatextension);
    if( !pCodec )
        throw(std::exception("Unable to encode image data as - invalid extension. TextureImage::encode" ));

    ImageCodec::ImageData* imgData = new ImageCodec::ImageData();
    imgData->format = mFormat;
    imgData->height = mHeight;
    imgData->width = mWidth;
    imgData->depth = mDepth;
    // Wrap in CodecDataPtr, this will delete
    Codec::CodecDataPtr codeDataPtr(imgData);
    // Wrap memory, be sure not to delete when stream destroyed
    MemoryDataStreamPtr wrapper(new MemoryDataStream(mBuffer, mBufSize, false));

    return pCodec->code(wrapper, codeDataPtr);
}

TextureImage & TextureImage::load(DataStreamPtr& stream, const std::string& type )
{
    freeMemory();

    Codec * pCodec = 0;
    if (!type.empty())
    {
        // use named codec
        pCodec = Codec::getCodec(type);
    }
    else
    {
        // derive from magic number
        // read the first 32 bytes or file size, if less
        size_t magicLen = std::min(stream->size(), (size_t)32);
        char magicBuf[32];
        stream->read(magicBuf, magicLen);
        // return to start
        stream->seek(0);
        pCodec = Codec::getCodec(magicBuf, magicLen);

        if( !pCodec )
            throw(std::exception("Unable to load image: TextureImage format is unknown. Unable to identify codec. "));
    }

    Codec::DecodeResult res = pCodec->decode(stream);

    ImageCodec::ImageData* pData = 
        static_cast<ImageCodec::ImageData*>(res.second.get());

    mWidth = pData->width;
    mHeight = pData->height;
    mDepth = pData->depth;
    mBufSize = pData->size;
    mNumMipmaps = pData->num_mipmaps;
    mFlags = pData->flags;

    // Get the format and compute the pixel size
    mFormat = pData->format;
    mPixelSize = static_cast<uint8_t>(PixelUtil::getNumElemBytes( mFormat ));
    // Just use internal buffer of returned memory stream
    mBuffer = res.first->getPtr();
    // Make sure stream does not delete
    res.first->setFreeOnClose(false);
    // make sure we delete
    mAutoDelete = true;

    return *this;
}
std::string TextureImage::getFileExtFromMagic(DataStreamPtr stream)
{
    // read the first 32 bytes or file size, if less
    size_t magicLen = std::min(stream->size(), (size_t)32);
    char magicBuf[32];
    stream->read(magicBuf, magicLen);
    // return to start
    stream->seek(0);
    Codec* pCodec = Codec::getCodec(magicBuf, magicLen);

    if(pCodec)
        return pCodec->getType();
    else
        return std::string();

}

uint8_t* TextureImage::getData()
{
    return mBuffer;
}

const uint8_t* TextureImage::getData() const
{
    assert( mBuffer );
    return mBuffer;
}

size_t TextureImage::getSize() const
{
    return mBufSize;
}

size_t TextureImage::getNumMipmaps() const
{
    return mNumMipmaps;
}

bool TextureImage::hasFlag(const TextureImageFlags imgFlag) const
{
    if(mFlags & imgFlag)
    {
        return true;
    }
    else
    {
        return false;
    }
}

size_t TextureImage::getDepth() const
{
    return mDepth;
}

size_t TextureImage::getWidth() const
{
    return mWidth;
}

size_t TextureImage::getHeight() const
{
    return mHeight;
}

size_t TextureImage::getNumFaces(void) const
{
    if(hasFlag(IF_CUBEMAP))
        return 6;
    return 1;
}

size_t TextureImage::getRowSpan() const
{
    return mWidth * mPixelSize;
}

PixelFormat TextureImage::getFormat() const
{
    return mFormat;
}

uint8_t TextureImage::getBPP() const
{
    return mPixelSize * 8;
}

bool TextureImage::getHasAlpha(void) const
{
    return PixelUtil::getFlags(mFormat) & PFF_HASALPHA;
}

void TextureImage::applyGamma( uint8_t *buffer, float gamma, size_t size, uint8_t bpp )
{
    if( gamma == 1.0f )
        return;

    //NB only 24/32-bit supported
    if( bpp != 24 && bpp != 32 ) return;

    uint32_t stride = bpp >> 3;

    for( size_t i = 0, j = size / stride; i < j; i++, buffer += stride )
    {
        float r, g, b;

        const float rangeMult = 255.0f;
        const float rangeMultInv = 1.0f / rangeMult;
        const float gammaValue = 1.0f / gamma;

        r = rangeMultInv * buffer[0];
        g = rangeMultInv * buffer[1];
        b = rangeMultInv * buffer[2];

        r = powf(r, gammaValue);
        g = powf(g, gammaValue);
        b = powf(b, gammaValue);

        buffer[0] = (uint8_t)(r * rangeMult);
        buffer[1] = (uint8_t)(g * rangeMult);
        buffer[2] = (uint8_t)(b * rangeMult);
    }
}

void TextureImage::resize(size_t width, size_t height, Filter filter)
{
    // resizing dynamic images is not supported
    assert(mAutoDelete);
    assert(mDepth == 1);

    // reassign buffer to temp image, make sure auto-delete is true
    TextureImage temp;
    temp.loadDynamicTextureImage(mBuffer, mWidth, mHeight, 1, mFormat, true);
    // do not delete[] mBuffer!  temp will destroy it

    // set new dimensions, allocate new buffer
    mWidth = width;
    mHeight = height;
    mBufSize = PixelUtil::getMemorySize(mWidth, mHeight, 1, mFormat);
    mBuffer = (uint8_t*)malloc(sizeof(uint8_t) * mBufSize);
    mNumMipmaps = 0; // Loses precomputed mipmaps

    // scale the image from temp into our resized buffer
    TextureImage::scale(temp.getPixelBox(), getPixelBox(), filter);
}

void
TextureImage::copyFrom(intptr_t dstPtr, bool reverse)
{   
    const PixelBox& src = getPixelBox();

    if (!reverse)
    {
        memcpy(reinterpret_cast<void*>(dstPtr), (void*)src.data, src.getConsecutiveSize());
    }
    else
    {
        unsigned char* dst = (unsigned char*)(dstPtr); // Should be based on Template Type

        for (unsigned int i = 0;i < src.size().y; i++)
        {
            for (unsigned int j = 0; j < src.size().x; j++)
            {
                    size_t x = ((i * src.size().x) + j) * 4;
                    dst[x  ]   = ((unsigned char*)(src.data))[x];
                    dst[x+1]   = ((unsigned char*)(src.data))[x+1];
                    dst[x+2]   = ((unsigned char*)(src.data))[x+2];
                    dst[x+3]   = ((unsigned char*)(src.data))[x+3];
            }
        }
    }
}

void TextureImage::scale(const PixelBox &src, const PixelBox &scaled, Filter filter) 
{
#if 0 // TODO. Critical.
    assert(PixelUtil::isAccessible(src.format));
    assert(PixelUtil::isAccessible(scaled.format));
    MemoryDataStreamPtr buf; // For auto-delete

    PixelBox temp;
    switch (filter) 
    {
    default:
    case FILTER_NEAREST:
        if(src.format == scaled.format) 
        {
            // No intermediate buffer needed
            temp = scaled;
        }
        else
        {
            // Allocate temporary buffer of destination size in source format 
            temp = PixelBox(scaled.size().x, scaled.size().y, scaled.size().z, src.format);
            buf.reset(new MemoryDataStream(temp.getConsecutiveSize()));
            temp.data = buf->getPtr();
        }
        // super-optimized: no conversion
        switch (PixelUtil::getNumElemBytes(src.format)) 
        {
        case 1: NearestResampler<1>::scale(src, temp); break;
        case 2: NearestResampler<2>::scale(src, temp); break;
        case 3: NearestResampler<3>::scale(src, temp); break;
        case 4: NearestResampler<4>::scale(src, temp); break;
        case 6: NearestResampler<6>::scale(src, temp); break;
        case 8: NearestResampler<8>::scale(src, temp); break;
        case 12: NearestResampler<12>::scale(src, temp); break;
        case 16: NearestResampler<16>::scale(src, temp); break;
        default:
            // never reached
            assert(false);
        }
        if(temp.data != scaled.data)
        {
            // Blit temp buffer
            PixelUtil::bulkPixelConversion(temp, scaled);
        }
        break;

    case FILTER_LINEAR:
    case FILTER_BILINEAR:
        switch (src.format) 
        {
        case PF_L8: case PF_A8: case PF_BYTE_LA:
        case PF_R8G8B8: case PF_B8G8R8:
        case PF_R8G8B8A8: case PF_B8G8R8A8:
        case PF_A8B8G8R8: case PF_A8R8G8B8:
        case PF_X8B8G8R8: case PF_X8R8G8B8:
            if(src.format == scaled.format) 
            {
                // No intermediate buffer needed
                temp = scaled;
            }
            else
            {
                // Allocate temp buffer of destination size in source format 
                temp = PixelBox(scaled.size().x, scaled.size().y, scaled.size().z, src.format);
                buf.reset(new MemoryDataStream(temp.getConsecutiveSize()));
                temp.data = buf->getPtr();
            }
            // super-optimized: byte-oriented math, no conversion
            switch (PixelUtil::getNumElemBytes(src.format)) 
            {
            case 1: LinearResampler_Byte<1>::scale(src, temp); break;
            case 2: LinearResampler_Byte<2>::scale(src, temp); break;
            case 3: LinearResampler_Byte<3>::scale(src, temp); break;
            case 4: LinearResampler_Byte<4>::scale(src, temp); break;
            default:
                // never reached
                assert(false);
            }
            if(temp.data != scaled.data)
            {
                // Blit temp buffer
                PixelUtil::bulkPixelConversion(temp, scaled);
            }
            break;
        case PF_FLOAT32_RGB:
        case PF_FLOAT32_RGBA:
            if (scaled.format == PF_FLOAT32_RGB || scaled.format == PF_FLOAT32_RGBA)
            {
                // float32 to float32, avoid unpack/repack overhead
                LinearResampler_Float32::scale(src, scaled);
                break;
            }
            // else, fall through
        default:
            // non-optimized: floating-point math, performs conversion but always works
            LinearResampler::scale(src, scaled);
        }
        break;
    }
#endif
}

ColorValue TextureImage::getColorAt(size_t x, size_t y, size_t z) const
{
    ColorValue rval;
    PixelUtil::unpackColor(&rval, mFormat, &mBuffer[mPixelSize * (z * mWidth * mHeight + mWidth * y + x)]);
    return rval;
}

void TextureImage::setColorAt(ColorValue const &cv, size_t x, size_t y, size_t z)
{
    size_t pixelSize = PixelUtil::getNumElemBytes(getFormat());
    PixelUtil::packColor(cv, getFormat(), &((unsigned char *)getData())[pixelSize * (z * getWidth() * getHeight() + y * getWidth() + x)]);
}  

PixelBox TextureImage::getPixelBox(size_t face, size_t mipmap) const
{
    // TextureImage data is arranged as:
    // face 0, top level (mip 0)
    // face 0, mip 1
    // face 0, mip 2
    // face 1, top level (mip 0)
    // face 1, mip 1
    // face 1, mip 2
    // etc
    if(mipmap > getNumMipmaps())
        throw(std::exception("Mipmap index out of range TextureImage::getPixelBox" )) ;
    if(face >= getNumFaces())
        throw(std::exception("Face index out of range TextureImage::getPixelBox"));
    // Calculate mipmap offset and size
    // Not mine. [MattD].
    uint8_t *offset = const_cast<uint8_t*>(getData());
    // Base offset is number of full faces
    size_t width = getWidth(), height=getHeight(), depth=getDepth();
    size_t numMips = getNumMipmaps();

    // Figure out the offsets 
    size_t fullFaceSize = 0;
    size_t finalFaceSize = 0;
    size_t finalWidth = 0, finalHeight = 0, finalDepth = 0;

    size_t mipRange = Ibl::Limits<int32_t>::maxVal(int32_t(0), int32_t(numMips - 1));
    for(size_t mip = 0; mip <= mipRange; ++mip)
    {
        if (mip == mipmap)
        {
            finalFaceSize = fullFaceSize;
            finalWidth = width;
            finalHeight = height;
            finalDepth = depth;
        }
        fullFaceSize += PixelUtil::getMemorySize(width, height, depth, getFormat());

        /// Half size in each dimension
        if(width!=1) width /= 2;
        if(height!=1) height /= 2;
        if(depth!=1) depth /= 2;
    }
    // Advance pointer by number of full faces, plus mip offset into
    offset += face * fullFaceSize;
    offset += finalFaceSize;
    // Return subface as pixelbox
    PixelBox src(finalWidth, finalHeight, finalDepth, getFormat(), offset);
    return src;
}

size_t TextureImage::calculateSize(size_t mipmaps, size_t faces, size_t width, size_t height, size_t depth, 
    PixelFormat format)
{
    size_t size = 0;
    for(size_t mip=0; mip<=mipmaps; ++mip)
    {
        size += PixelUtil::getMemorySize(width, height, depth, format)*faces; 
        if(width!=1) width /= 2;
        if(height!=1) height /= 2;
        if(depth!=1) depth /= 2;
    }
    return size;
}

TextureImage & TextureImage::loadTwoTextureImagesAsRGBA(const std::string& rgbFilename, const std::string& alphaFilename,
    const std::string& groupName, PixelFormat fmt)
{
    TextureImage rgb, alpha;

    rgb.load(rgbFilename, groupName);
    alpha.load(alphaFilename, groupName);

    return combineTwoTextureImagesAsRGBA(rgb, alpha, fmt);

}

TextureImage & TextureImage::loadTwoTextureImagesAsRGBA(DataStreamPtr& rgbStream, DataStreamPtr& alphaStream,
    PixelFormat fmt, const std::string& rgbType, const std::string& alphaType)
{
    TextureImage rgb, alpha;

    rgb.load(rgbStream, rgbType);
    alpha.load(alphaStream, alphaType);

    return combineTwoTextureImagesAsRGBA(rgb, alpha, fmt);

}

TextureImage & 
TextureImage::create(const Ibl::Vector2i& size, PixelFormat format, uint32_t numMipMaps, uint32_t flags)
{
    freeMemory();

    mWidth  = size.x;
    mHeight  = size.y;
    mDepth   = 1;

    mNumMipmaps = numMipMaps;
    mFlags = flags;

    // Get the format and compute the pixel size
    mFormat = format;
    mPixelSize = static_cast<uint8_t>(PixelUtil::getNumElemBytes( mFormat ));

    // Just use internal buffer of returned memory stream
    mBufSize = calculateSize(mNumMipmaps, mFlags&IF_CUBEMAP?6:1, mWidth, mHeight, mDepth, mFormat);
    mPixelSize = static_cast<uint8_t>(PixelUtil::getNumElemBytes( mFormat ));

    // error check here.
    // PF_UNKNOWN, mBufSize etc

    mBuffer = static_cast<uint8_t*>(malloc(mBufSize * sizeof(uint8_t)));
    memset (mBuffer, 0, sizeof(uint8_t) * mBufSize);

    // make sure we delete
    mAutoDelete = true;

    return *this;
}

TextureImage & TextureImage::combineTwoTextureImagesAsRGBA(const TextureImage& rgb, const TextureImage& alpha, PixelFormat fmt)
{
    // the images should be the same size, have the same number of mipmaps
    if (rgb.getWidth() != alpha.getWidth() ||
        rgb.getHeight() != alpha.getHeight() ||
        rgb.getDepth() != alpha.getDepth())
    {
        throw(std::exception("TextureImages must be the same dimensions - TextureImage::combineTwoTextureImagesAsRGBA"));
    }
    if (rgb.getNumMipmaps() != alpha.getNumMipmaps() ||
        rgb.getNumFaces() != alpha.getNumFaces())
    {
        throw(std::exception("TextureImages must have the same number of surfaces (faces & mipmaps) - TextureImage::combineTwoTextureImages"));
    }
    // Format check
    if (PixelUtil::getComponentCount(fmt) != 4)
    {
        throw(std::exception("Target format must have 4 components - TextureImage::combineTwoTextureImagesAsRGBA"));
    }
    if (PixelUtil::isCompressed(fmt) || PixelUtil::isCompressed(rgb.getFormat()) 
        || PixelUtil::isCompressed(alpha.getFormat()))
    {
        throw(std::exception("Compressed formats are not supported in this method TextureImage::combineTwoTextureImagesAsRGBA"));
    }

    freeMemory();

    mWidth = rgb.getWidth();
    mHeight = rgb.getHeight();
    mDepth = rgb.getDepth();
    mFormat = fmt;
    mNumMipmaps = rgb.getNumMipmaps();
    size_t numFaces = rgb.getNumFaces();

    // Set flags
    mFlags = 0;
    if (mDepth != 1)
        mFlags |= IF_3D_TEXTURE;
    if(numFaces == 6)
        mFlags |= IF_CUBEMAP;

    mBufSize = calculateSize(mNumMipmaps, numFaces, mWidth, mHeight, mDepth, mFormat);

    mPixelSize = static_cast<uint8_t>(PixelUtil::getNumElemBytes( mFormat ));

    mBuffer = static_cast<uint8_t*>(malloc(mBufSize * sizeof(uint8_t)));

    // make sure we delete
    mAutoDelete = true;


    for (size_t face = 0; face < numFaces; ++face)
    {
        for (size_t mip = 0; mip <= mNumMipmaps; ++mip)
        {
            // convert the RGB first
            PixelBox srcRGB = rgb.getPixelBox(face, mip);
            PixelBox dst = getPixelBox(face, mip);
            PixelUtil::bulkPixelConversion(srcRGB, dst);

            // now selectively add the alpha
            PixelBox srcAlpha = alpha.getPixelBox(face, mip);
            uint8_t* psrcAlpha = static_cast<uint8_t*>(srcAlpha.data);
            uint8_t* pdst = static_cast<uint8_t*>(dst.data);
            for (size_t d = 0; d < mDepth; ++d)
            {
                for (size_t y = 0; y < mHeight; ++y)
                {
                    for (size_t x = 0; x < mWidth; ++x)
                    {
                        ColorValue colRGBA, colA;
                        // read RGB back from dest to save having another pointer
                        PixelUtil::unpackColor(&colRGBA, mFormat, pdst);
                        PixelUtil::unpackColor(&colA, alpha.getFormat(), psrcAlpha);

                        // combine RGB from alpha source texture
                        colRGBA.a = (colA.r + colA.g + colA.b) / 3.0f;

                        PixelUtil::packColor(colRGBA, mFormat, pdst);
                        
                        psrcAlpha += PixelUtil::getNumElemBytes(alpha.getFormat());
                        pdst += PixelUtil::getNumElemBytes(mFormat);
                    }
                }
            }
        }
    }
    return *this;
}
}
