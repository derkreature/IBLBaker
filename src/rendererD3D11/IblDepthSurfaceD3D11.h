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

#ifndef INCLUDED_BB_DEPTH_SURFACE
#define INCLUDED_BB_DEPTH_SURFACE

#include <IblPlatform.h>
#include <IblIDepthSurface.h>

namespace Ibl
{
class DeviceD3D11;

class DepthSurfaceD3D11 : public Ibl::IDepthSurface
{
  public:
    DepthSurfaceD3D11 (Ibl::DeviceD3D11* device);
    virtual ~DepthSurfaceD3D11();

    virtual bool               initialize (const Ibl::DepthSurfaceParameters*);
    virtual bool               create();
    virtual bool               free();
    virtual bool               cache();
    virtual bool               bind(uint32_t index = 0) const;

    virtual bool               clear();
    virtual void               setSize (const Ibl::Vector2i& size);

    virtual int                width() const;
    virtual int                height() const;

    ID3D11DepthStencilView *   surface () const;
    ID3D11Texture2D*           depthTexture() const;
    ID3D11ShaderResourceView*  resourceView() const;

    virtual bool               recreateOnResize() { return true; }

    virtual bool               clearStencil();

    virtual int                multiSampleCount() const;
    virtual int                multiSampleQuality() const;
    D3D11_TEXTURE2D_DESC       descriptor() const { return _textureDesc; }

  private:
    DepthSurfaceParameters*    _initializationData;
    ID3D11DepthStencilView*    _depthSurface;
    ID3D11ShaderResourceView*  _depthShaderResourceView;
    ID3D11Texture2D*           _depthTexture;
    D3D11_DEPTH_STENCIL_VIEW_DESC _desc;
    D3D11_TEXTURE2D_DESC       _textureDesc;

    ID3D11Device*             _direct3d;
    ID3D11DeviceContext *     _immediateCtx;
};
}

#endif