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

#ifndef INCLUDED_TEXTURE_IMAGE
#define INCLUDED_TEXTURE_IMAGE

#include <IblPlatform.h>
#include <IblPixelFormat.h>
#include <IblDataStream.h>

namespace Ibl
{
enum TextureImageFlags
{
    IF_COMPRESSED = 0x00000001,
    IF_CUBEMAP    = 0x00000002,
    IF_3D_TEXTURE = 0x00000004
};

class TextureImage
{
  public:
    typedef Region3ui Box;

  public:
    TextureImage();
    TextureImage( const TextureImage &img );
    virtual ~TextureImage();

    void           copyFrom(intptr_t dstPtr, bool reverse = true);

    TextureImage & operator = ( const TextureImage & img );
    TextureImage & flipAroundY();
    TextureImage & flipAroundX();
    TextureImage& loadDynamicTextureImage(uint8_t* data, size_t width, size_t height, 
                                          size_t depth,
                                          PixelFormat format, bool autoDelete = false, 
                                          size_t numFaces = 1, size_t numMipMaps = 0);
    
    TextureImage& loadDynamicTextureImage( uint8_t* data, size_t width,
                             size_t height, PixelFormat format)
    {
        return loadDynamicTextureImage(data, width, height, 1, format);
    }

    TextureImage & loadRawData( 
        DataStreamPtr& stream, 
        size_t width, size_t height, size_t depth,
        PixelFormat format,
        size_t numFaces = 1, size_t numMipMaps = 0);
   
    TextureImage & loadRawData( 
        DataStreamPtr& stream, 
        size_t width, size_t height, 
        PixelFormat format )
    {
        return loadRawData(stream, width, height, 1, format);
    }


    TextureImage &  create(const Ibl::Vector2i& size, PixelFormat format, uint32_t numMipMaps = 1, uint32_t flags = 0);

    TextureImage & load( const std::string& filename, const std::string& groupName);

    
    TextureImage & load(DataStreamPtr& stream, const std::string& type);

    
    TextureImage & loadTwoTextureImagesAsRGBA(const std::string& rgbFilename, const std::string& alphaFilename,
        const std::string& groupName, PixelFormat format = PF_BYTE_RGBA);

    
    TextureImage & loadTwoTextureImagesAsRGBA(DataStreamPtr& rgbStream, DataStreamPtr& alphaStream, PixelFormat pf,
        const std::string& rgbType, const std::string& alphaType);

    
    TextureImage & combineTwoTextureImagesAsRGBA(const TextureImage& rgb, const TextureImage& alpha, PixelFormat format);

    void save(const std::string& filename);

    DataStreamPtr encode(const std::string& formatextension);
    
    uint8_t* getData(void);

    const uint8_t * getData() const;       

    size_t getSize() const;

    size_t getNumMipmaps() const;

    bool hasFlag(const TextureImageFlags imgFlag) const;

    size_t getWidth(void) const;

    size_t getHeight(void) const;

    size_t getDepth(void) const;
    
    size_t getNumFaces(void) const;

    size_t getRowSpan(void) const;

    PixelFormat getFormat() const;

    uint8_t getBPP() const;

    bool getHasAlpha() const;
    
    static void applyGamma( uint8_t *buffer, float  gamma, size_t size, uint8_t bpp );

    ColorValue getColorAt(size_t x, size_t y, size_t z) const;
    
    void setColorAt(ColorValue const &cv, size_t x, size_t y, size_t z);

    PixelBox getPixelBox(size_t face = 0, size_t mipmap = 0) const;

    void freeMemory();

    enum Filter
    {
        FILTER_NEAREST,
        FILTER_LINEAR,
        FILTER_BILINEAR,
        FILTER_BOX,
        FILTER_TRIANGLE,
        FILTER_BICUBIC
    };

    static void scale(const PixelBox &src, const PixelBox &dst, Filter filter = FILTER_BILINEAR);
    void   resize(size_t width, size_t height, Filter filter = FILTER_BILINEAR);
    static size_t calculateSize(size_t mipmaps, size_t faces, size_t width, size_t height, size_t depth, PixelFormat format);
    static std::string getFileExtFromMagic(DataStreamPtr stream);

    friend std::ostream& 
    operator << (std::ostream &o, const Ibl::TextureImage& image);

  protected:
    size_t mWidth;
    size_t mHeight;
    size_t mDepth;
    size_t mBufSize;
    size_t mNumMipmaps;
    int    mFlags;

    PixelFormat mFormat;

    uint8_t  mPixelSize;
    uint8_t* mBuffer;

    bool mAutoDelete;
};

typedef std::shared_ptr<TextureImage> TextureImagePtr;
typedef std::vector<TextureImagePtr> TextureImagePtrList;
typedef std::vector<const TextureImagePtr> ConstTextureImagePtrList;

}

#endif
