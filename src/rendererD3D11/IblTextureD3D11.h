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

#ifndef INCLUDED_BB_TEXTURE
#define INCLUDED_BB_TEXTURE

#include <IblPlatform.h>
#include <IblRenderEnums.h>
#include <IblRenderDeviceD3D11.h>
#include <IblITexture.h>

namespace Ibl
{

void GetSurfaceInfo(size_t width,
                    size_t height,
                    DXGI_FORMAT fmt,
                    size_t* outNumBytes,
                    size_t* outRowBytes,
                    size_t* outNumRows );

//----------------------------------
// Base interface class for textures
//----------------------------------
class TextureD3D11 : public Ibl::ITexture
{
protected:
    TextureD3D11(Ibl::DeviceD3D11* device);

  public:
    virtual ~TextureD3D11();

    friend class TextureMgr;

    virtual bool cache();

    virtual bool create ();
    
    virtual bool free();

    virtual ID3D11ShaderResourceView * resourceView(int32_t arrayId = -1, int32_t mipId = -1) const;

    virtual bool               initialize(const Ibl::TextureParameters* data);
    ID3D11Resource *           texture() const;

    void                       setTexture (ID3D11Resource * textureArg);

    void                       setResourceView (ID3D11ShaderResourceView * shaderResourceView);

    virtual bool               bindSurface(int renderTargetIndex) const;

    virtual bool               unmap() const;
    virtual bool               map(uint32_t image, uint32_t mip) const;
    virtual bool               mapForRead() const;
    virtual bool               unmapFromRead() const;

    virtual bool               mapForWrite();
    
    // Dumb max value. Need to implement this on the gpu.
    virtual const Ibl::Vector4f& maxValue() const;

    virtual bool               clearSurface (uint32_t layerId, float r  = 1.0f, float g  = 1.0f, float b  = 1.0f, float a = 1.0f) ;

    virtual void               generateMipMaps() const;

    virtual Ibl::TextureImagePtr readImage(Ibl::PixelFormat format, int32_t mipId = -1) const;

    DXGI_FORMAT                dxFormat() const;

    // Write a pixel
    virtual bool               write (const Ibl::Vector4f&, 
                                      const Ibl::Vector2f& pos);

    // Write all pixels. Pixels is assumed to be the correct type and
    // allocated to byteSize()
    virtual bool               write (const Ibl::byte* pixels);

    // Read a texel
    virtual Ibl::Vector4f       read (const Ibl::Vector2f& pos) const;
    // read a pixel
    virtual Ibl::Vector4f       read (const Ibl::Vector2i& pos) const;

    // Read all pixels. Pixels should be preallocated to byteSize().
    virtual Ibl::Vector4f       read (Ibl::byte* pos) const;

    virtual bool               save(const std::string& filePathName,
                                    bool fixSeams = false,
                                    bool splitChannels = false,
                                    bool rgbOnly = false,
                                    int32_t mipLevel = -1,
                                    const Ibl::ITexture* mergeMap = nullptr) const;

  protected:
    virtual void                 setFormat (DXGI_FORMAT format);

    ID3D11Resource *             _texture;
    ID3D11ShaderResourceView *   _resourceView;
    mutable D3D11_MAPPED_SUBRESOURCE _mappedResource;
    mutable bool                 _mapped;
    DXGI_FORMAT                  _dxFormat;    
    ID3D11Device*                _direct3d;
    ID3D11DeviceContext *        _immediateCtx;

    mutable bool                 _maxValueCached;
    mutable Ibl::Vector4f         _maxValue;
};
}

#endif