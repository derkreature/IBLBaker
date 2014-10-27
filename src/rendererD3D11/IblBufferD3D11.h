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

#ifndef INCLUDED_BB_BUFFER_RESOURCE_D3D11
#define INCLUDED_BB_BUFFER_RESOURCE_D3D11

#include <IblPlatform.h>
#include <IblRenderDeviceD3D11.h>
#include <IblIGpuBuffer.h>

namespace Ibl
{
class BufferD3D11: public Ibl::IGpuBuffer
{
  public:
    BufferD3D11 (Ibl::DeviceD3D11* device);

    // Non owner bind point for shader resource.
    // Adds reference.
    BufferD3D11 (Ibl::DeviceD3D11* device, 
                 ID3D11ShaderResourceView * resourceView);

    virtual ~BufferD3D11();
    
    virtual bool               initialize (const Ibl::GpuBufferParameters* data);
    virtual bool               create();
    virtual bool               free();
    virtual bool               cache();

    virtual void*              lock();
    virtual bool               unlock();
    virtual bool               bind() const;
    virtual bool               bindToStreamOut() const;

    virtual size_t             size() const;

    ID3D11Buffer *             buffer() const;
    ID3D11ShaderResourceView * resourceView() const;
    ID3D11UnorderedAccessView* unorderedView() const;

    virtual bool               recreateOnResize() { return _resource.sizeBasedOnBackBuffer(); }
  
    virtual void               clearUnorderedAccessViewFloat(float clearValue);
    virtual void               clearUnorderedAccessViewUint(uint32_t clearValue);

  private:
    bool                               _locked;

    ID3D11Buffer*                      _buffer;
    ID3D11UnorderedAccessView*         _unorderedResourceView;
    ID3D11ShaderResourceView *         _resourceView;

    Ibl::GpuBufferParameters            _resource;
    D3D11_UNORDERED_ACCESS_VIEW_DESC   _uavDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC    _srvDesc;
    size_t                             _size;
    bool                               _isOwner;
  protected:
    ID3D11Device*                      _direct3d;
    ID3D11DeviceContext *              _immediateCtx;
};
}

#endif