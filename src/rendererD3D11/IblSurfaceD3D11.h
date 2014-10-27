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

#ifndef INCLUDED_BB_BACKBUFFER_D3D11
#define INCLUDED_BB_BACKBUFFER_D3D11

#include <IblPlatform.h>
#include <IblISurface.h>

namespace Ibl
{
class ITexture;

class SurfaceD3D11 : public Ibl::ISurface
{
  public:
    SurfaceD3D11(Ibl::IDevice*);
    virtual ~SurfaceD3D11();

    virtual bool               create();
    virtual bool               free();
    virtual bool               cache();

    // Initialize from a texture surface.
    virtual bool               initialize (int firstLevel = 0, 
                                           int numberOfLevels = 1,
                                           Ibl::ITexture* texture = 0,
                                           int mipLevel = -1);

    // Bind to the specified surface
    virtual bool               bind(uint32_t level = 0) const;
    virtual bool               bindAndClear (uint32_t level = 0) const;

    bool                       clearAll(float r = 0, float g= 0, float b= 0, float a= 0);

    virtual unsigned int       width() const;
    virtual unsigned int       height() const;

    ID3D11RenderTargetView*    surface() const;
    virtual bool               writeToFile (const std::string& filename) const;
    const ITexture*            texture() const {return _texture;}
    ID3D11Texture2D*           resource() const;
    virtual bool               recreateOnResize();
    DXGI_FORMAT                dxFormat() const { return _desc.Format; }

  protected:
    ID3D11RenderTargetView*     _surface;
    D3D11_RENDER_TARGET_VIEW_DESC _desc;
    ID3D11Texture2D*            _resource;
    D3D11_TEXTURE2D_DESC        _textureDesc;
    int                         _firstLevel;
    int                         _numberOfLevels;
    ITexture*             _texture;
    std::vector<ID3D11RenderTargetView*> _mipViews;
    ID3D11Device*               _direct3d;
    ID3D11DeviceContext *       _immediateCtx;
    int                         _mipLevel;
};
}

#endif