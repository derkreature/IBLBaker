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

#ifndef INCLUDED_BB_RENDER_TARGET_TEXTURE_D3D11
#define INCLUDED_BB_RENDER_TARGET_TEXTURE_D3D11

#include <IblTextureD3D11.h>
#include <IblPlatform.h>
#include <IblSurfaceD3D11.h>

namespace Ibl
{
class RenderTargetTextureD3D11 : public Ibl::TextureD3D11
{
  public:
    RenderTargetTextureD3D11(Ibl::DeviceD3D11 * device);
    virtual ~RenderTargetTextureD3D11();

    virtual bool               create();
    virtual bool               free();

    virtual bool               isCubeMap() const;

    virtual void               clearUnorderedAccessViewUint(uint32_t) const;
    ID3D11UnorderedAccessView* unorderedView() const;
    virtual bool               recreateOnResize();

    virtual bool               bindSurface (int renderTargetIndex) const;    
    virtual bool               clearSurface (uint32_t layerId, float r  = 1.0f, float g  = 1.0f, float b  = 1.0f, float a = 1.0f) ;

    virtual const Ibl::ISurface*  surface(int32_t arrayId = -1, int32_t mipId = -1) const;
    virtual ID3D11ShaderResourceView * resourceView(int32_t arrayId = -1, int32_t mipId = -1) const;

    virtual bool                mapForRead() const;
    virtual bool                unmapFromRead() const;

    virtual bool                map(uint32_t imageLevel = 0, uint32_t mipLevel = 0) const;
    virtual bool                unmap() const;

    const D3D11_TEXTURE2D_DESC desc() const { return _desc; }

  private:
    D3D11_TEXTURE2D_DESC        _desc;
    mutable ID3D11Texture2D *   _stagingTexture;

  private:
    ID3D11Texture2D*                _texture;
    ID3D11UnorderedAccessView*      _uav;

    // Surface for all arrayIds, 0th mip.
    mutable SurfaceD3D11*           _surface;

    // Resources for all mips per Array ID
    std::vector<ID3D11ShaderResourceView *>       _sliceResources;
    // Resources for all mips per array Id
    std::vector<SurfaceD3D11*>        _sliceSurfaces;

    mutable int32_t                   _mappedSubresourceId;

    // These still need cleanup.
    std::vector<ID3D11ShaderResourceView*>       _resourcesAllImagesPerMip;
    std::vector<Ibl::SurfaceD3D11*>       _surfacesAllImagesPerMip;

    std::map<size_t, std::vector<ID3D11ShaderResourceView*> > _resourcesPerImagePerMip;
    std::map<size_t, std::vector<Ibl::SurfaceD3D11*> >         _surfacesPerImagePerMip;
};
}

#endif