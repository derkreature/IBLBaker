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

#include <IblBufferD3D11.h>
#include <IblIRenderResourceParameters.h>
#include <IblIVertexDeclaration.h>
#include <IblFormatConversionD3D11.h>
#include <IblLog.h>

namespace Ibl
{
BufferD3D11::BufferD3D11(Ibl::DeviceD3D11* device) : 
IGpuBuffer (device),
    _locked (false),
    _buffer(0),
    _resourceView (0),
    _unorderedResourceView(0),
    _size(0),
    _direct3d(nullptr),
    _immediateCtx(nullptr)
{
    if (Ibl::DeviceD3D11* _device = 
        dynamic_cast <Ibl::DeviceD3D11*>(device))
    {
        _direct3d = *_device;
        _immediateCtx = _device->immediateCtx();
    }
    _isOwner = true;
    memset (&_srvDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
}

BufferD3D11::BufferD3D11 (Ibl::DeviceD3D11* device,
                          ID3D11ShaderResourceView * resourceView) : 
    IGpuBuffer (device),
        _locked (false),
        _buffer(0),
        _resourceView (0),
        _unorderedResourceView(0),
        _size(0),
        _direct3d(nullptr),
        _immediateCtx(nullptr)
{
    // This really should be 
    _resourceView  = resourceView;
    _isOwner = false;

    if (Ibl::DeviceD3D11* _device = 
        dynamic_cast <Ibl::DeviceD3D11*>(device))
    {
        _direct3d = *_device;
        _immediateCtx = _device->immediateCtx();
    }

    memset (&_srvDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
}

BufferD3D11::~BufferD3D11()
{
    free();
}

bool
BufferD3D11::initialize (const Ibl::GpuBufferParameters* data)
{
    _resource = Ibl::GpuBufferParameters (*data);
    return create();
}

ID3D11ShaderResourceView * 
BufferD3D11::resourceView() const
{
    return _resourceView;
}

ID3D11Buffer *
BufferD3D11::buffer() const
{
    return _buffer;
}

ID3D11UnorderedAccessView*  
BufferD3D11::unorderedView() const
{
    return _unorderedResourceView;
}

bool
BufferD3D11::create()
{
    D3D11_BUFFER_DESC bd;
    memset (&bd, 0, sizeof(D3D11_BUFFER_DESC));

    if (_resource.defaultMemoryUsage())
    {
        bd.Usage = D3D11_USAGE_DEFAULT;
    }
    else
    {
        bd.Usage = _resource.dynamic() ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
    }

    size_t byteWidth = 0;
    size_t elementCount = 0;
    size_t byteStride = 0; 

    if (_resource.sizeBasedOnBackBuffer())
    {
        elementCount = _deviceInterface->backbuffer()->width() *
            _deviceInterface->backbuffer()->height() * _resource.elementSizeMultiplier();
    }
    else
    {
        elementCount = _resource.elementCount();
    }

    if (_resource.format() == Ibl::PF_UNKNOWN &&
        _resource.formatWidth() == 0)
    {
        LOG ("Cannot deduce format width from UNKNOWN");
        return false;
    }

    if (_resource.format() == Ibl::PF_UNKNOWN)
    {
        byteStride = _resource.formatWidth();
    }
    else
    {
        byteStride = formatByteStride(_resource.format());
    }

    byteWidth = byteStride * elementCount;
    //LOG ("Bytewidth is " << byteWidth);

    if (_resource.unorderedAccess())
    {
        bd.StructureByteStride = (uint32_t)(byteStride);
        bd.ByteWidth = (uint32_t)(elementCount * bd.StructureByteStride);

        if (_resource.constructStructured())
        {
            bd.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        }
        if (_resource.allowRawView())
        {
            bd.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
        }
    }
    else if (_resource.drawIndirect())
    {
        bd.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
        bd.ByteWidth = uint32_t(byteWidth);
    }
    else
    {
        if (_resource.format() != Ibl::PF_UNKNOWN)
        {
            bd.ByteWidth = uint32_t(elementCount *  byteStride);
        }
        else
        {
            // potential bug, bytewidth probably should be implicitly calculated here.
            bd.ByteWidth = _resource.byteWidth();
        }
    }

    _size = bd.ByteWidth;
    bd.BindFlags = 0;
    
    if (_resource.bindShaderResource())
    {
        bd.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
    }
    if (_resource.bindVertexBuffer())
    {
        bd.BindFlags |= D3D11_BIND_VERTEX_BUFFER;
    }
    if (_resource.bindRenderTarget())
    {
        bd.BindFlags |= D3D11_BIND_RENDER_TARGET;
    }
    if (_resource.bindStreamOutput())
    {
        bd.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
    }
    if (_resource.bindConstantBuffer())
    {
        bd.BindFlags |= D3D11_BIND_CONSTANT_BUFFER;
    }
    if (_resource.bindUnorderedAccessView())
    {
        bd.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
    }

    bd.CPUAccessFlags = _resource.dynamic() ? D3D11_CPU_ACCESS_WRITE : 0;

    D3D11_SUBRESOURCE_DATA resource;
    D3D11_SUBRESOURCE_DATA * ResourcePtr = nullptr;
    
    if (!_resource.dynamic() && _resource.streamPtr())
    {
        memset (&resource, 0,  sizeof(D3D11_SUBRESOURCE_DATA) );
        resource.pSysMem = _resource.streamPtr();
        resource.SysMemPitch = 0;
        resource.SysMemSlicePitch = 0;
        ResourcePtr = &resource;
    }

    bool success = false;

    if (SUCCEEDED (_direct3d->CreateBuffer( &bd, ResourcePtr, &_buffer)))
    {
        if (_resource.bindUnorderedAccessView())
        {
            memset(&_uavDesc, 0, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

            _uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
            _uavDesc.Buffer.FirstElement = 0;

            // Suspect, probably buggy.
            _uavDesc.Buffer.NumElements  = bd.ByteWidth / bd.StructureByteStride;

            _uavDesc.Format = findTypelessFormat(_resource.format());

            if (_resource.uavCounter())
            {
                _uavDesc.Buffer.Flags |= D3D11_BUFFER_UAV_FLAG_COUNTER;
            }
            if (_resource.uavFlagRaw())
            {
                _uavDesc.Buffer.Flags |= D3D11_BUFFER_UAV_FLAG_RAW;
            }
            if (_resource.uavAppend())
            {
                _uavDesc.Buffer.Flags |= D3D11_BUFFER_UAV_FLAG_APPEND;
            }

            if (FAILED(_direct3d->CreateUnorderedAccessView (_buffer, &_uavDesc, &_unorderedResourceView)))
            {
                LOG ("Failed to create unordered access view");
                return false;
            }
        }

        if (_resource.bindShaderResource())
        {
            //LOG ("Successfully created SRV buffer resource");
            memset (&_srvDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
            _srvDesc.Format =  findFormat(_resource.format());
            _srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;

            if (_resource.unorderedAccess())
            {
                _srvDesc.Buffer.FirstElement = 0;
                _srvDesc.Buffer.NumElements  = bd.ByteWidth / bd.StructureByteStride;
            }
            else
            {
                _srvDesc.Buffer.ElementOffset = 0;
                _srvDesc.Buffer.ElementWidth = uint32_t(elementCount / _resource.elementWidth());
            }

            if (FAILED (_direct3d->CreateShaderResourceView(_buffer,
                                                               &_srvDesc,
                                                               &_resourceView)))
            {
                LOG ("FAILED to create IResourceBuffer SRV ... \n");
                return false;
            }
        }
        success = true;
    }
    else
    {
        LOG ("Failed to create a buffer");
    }

    if (!success)
    {
        LOG ("Failed to create buffer resource");    
    }
    return success;
}

size_t
BufferD3D11::size() const
{
    return _size;
}

bool
BufferD3D11::free()
{
    if (_isOwner)
    {
        saferelease (_buffer);
        saferelease (_unorderedResourceView);
        saferelease (_resourceView);
    }
    return true;
}

bool
BufferD3D11::cache()
{
    return true;
}

void*
BufferD3D11::lock()
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    if (SUCCEEDED(_immediateCtx->Map (_buffer, 0, 
                                      D3D11_MAP_WRITE_DISCARD, 0, 
                                      &mappedResource)))
    {
        _locked = true;
        return mappedResource.pData;
    }

    return 0;
}

bool
BufferD3D11::unlock()
{
    _immediateCtx->Unmap(_buffer, 0);
    _locked = false;
    return true;
}

bool
BufferD3D11::bind() const
{
//    uint32_t offset = 0;
//    uint32_t stride = _vertexDeclaration->vertexStride();
//    _immediateCtx->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
    return false;
}

bool
BufferD3D11::bindToStreamOut() const
{
    // Set stream out to null
    ID3D11Buffer *buffer[1];
    buffer [0] = _buffer;
    UINT offset[1] = {0};

    _immediateCtx->SOSetTargets (1, buffer, offset);

    return true;
}

void
BufferD3D11::clearUnorderedAccessViewUint(uint32_t clearValue)
{
    const UINT clearValueUINT[4] = { clearValue, clearValue, clearValue, clearValue };
    _immediateCtx->ClearUnorderedAccessViewUint (_unorderedResourceView, &clearValueUINT[0]);
}

void
BufferD3D11::clearUnorderedAccessViewFloat(float clearValue)
{
    const FLOAT clearArray [4] = {(FLOAT)clearValue, (FLOAT)clearValue, (FLOAT)clearValue, (FLOAT)clearValue};
    _immediateCtx->ClearUnorderedAccessViewFloat (_unorderedResourceView, &clearArray[0]);
}

}
