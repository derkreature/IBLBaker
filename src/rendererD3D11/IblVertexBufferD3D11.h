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


#ifndef INCLUDED_BB_VERTEX_BUFFER
#define INCLUDED_BB_VERTEX_BUFFER

#include <IblPlatform.h>
#include <IblRenderDeviceD3D11.h>
#include <IblIVertexBuffer.h>

namespace Ibl
{
class VertexBufferD3D11 : public Ibl::IVertexBuffer
{
  public:
    VertexBufferD3D11(Ibl::DeviceD3D11* device);
    virtual ~VertexBufferD3D11();
    
    virtual bool                initialize (const Ibl::VertexBufferParameters* data);
    virtual bool                create();
    virtual bool                free();
    virtual bool                cache();

    virtual void*               lock();
    virtual bool                unlock();
    virtual bool                bind() const;

    virtual bool                bindToStreamOut() const;

    ID3D11ShaderResourceView *  shaderResourceView() const;
    ID3D11RenderTargetView *    renderTargetView() const;

    ID3D11UnorderedAccessView * unorderedResourceView() const;
    ID3D11Buffer*               resource() const;

    size_t                      size() const; 

  protected:
    void                        upload() const;
    bool                        _usingUploadBuffer;
    ID3D11Buffer*               _uploadBuffer;

  private:
    bool                                _locked;
    ID3D11Buffer*                       _vertexBuffer;
    Ibl::VertexBufferParameters          _resource;
    ID3D11ShaderResourceView *          _shaderResourceView;
    ID3D11RenderTargetView *            _renderTargetView;
    D3D11_UNORDERED_ACCESS_VIEW_DESC    _uavDesc;
    ID3D11UnorderedAccessView *         _unorderedResourceView;
    uint32_t                            _sizeInBytes;

  protected:
    ID3D11Device*               _direct3d;
    ID3D11DeviceContext *      _immediateCtx;
};
}

#endif 