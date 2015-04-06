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

#include <IblVertexBufferD3D11.h>
#include <IblIRenderResourceParameters.h>
#include <IblIVertexDeclaration.h>
#include <IblFormatConversionD3D11.h>
#include <IblLog.h>

namespace Ibl
{
VertexBufferD3D11::VertexBufferD3D11(Ibl::DeviceD3D11* device) :
    IVertexBuffer(device),
    _locked(false),
    _resource(0),
    _vertexBuffer(0),
    _shaderResourceView(0),
    _unorderedResourceView(nullptr),
    _renderTargetView(nullptr),
    _usingUploadBuffer(false),
    _uploadBuffer(nullptr),
    _direct3d(nullptr),
    _immediateCtx(nullptr),
    _sizeInBytes(0),
    _bufferCursor(0),
    _needsDiscard(true),
    _isRingBuffer(false),
    _lastCopySize(0)
{
    if (Ibl::DeviceD3D11* _device =
        dynamic_cast <Ibl::DeviceD3D11*>(device))
    {
        _direct3d = *_device;
        _immediateCtx = _device->immediateCtx();
    }
}

size_t VertexBufferD3D11::size() const
{
    return _sizeInBytes;
}

VertexBufferD3D11::~VertexBufferD3D11()
{
    free();
}

bool VertexBufferD3D11::initialize(const Ibl::VertexBufferParameters* data)
{
    _resource = Ibl::VertexBufferParameters(*data);
    return VertexBufferD3D11::create();
}

ID3D11ShaderResourceView * VertexBufferD3D11::shaderResourceView() const
{
    return _shaderResourceView;
}

ID3D11UnorderedAccessView * VertexBufferD3D11::unorderedResourceView() const
{
    return _unorderedResourceView;
}

ID3D11Buffer* VertexBufferD3D11::resource() const
{
    return _vertexBuffer;
}

void VertexBufferD3D11::upload() const
{
    if (_uploadBuffer)
    {
        D3D11_BOX srcBox = { 0, 0, 0, _resource.sizeInBytes(), 1, 1 };
        _immediateCtx->CopySubresourceRegion(_vertexBuffer, 0, 0, 0, 0,
            _uploadBuffer, 0, &srcBox);
    }
    else
    {
        LOG("Attempting to upload from vertex buffer with no upload resource");
    }
}

bool VertexBufferD3D11::create()
{

    _sizeInBytes = _resource.sizeInBytes();
    _isRingBuffer = _resource.ringBuffered();
    if (_isRingBuffer)
    {
        // Align to 16 bytes.
        if (_sizeInBytes % 16 != 0)
            _sizeInBytes += 16 - (_sizeInBytes % 16);
        _sizeInBytes *= 4;
    }

    D3D11_BUFFER_DESC bd;
    memset(&bd, 0, sizeof(D3D11_BUFFER_DESC));
    bd.Usage = _resource.dynamic() ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
    bd.ByteWidth = _sizeInBytes;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.MiscFlags = 0;
    bd.CPUAccessFlags = 0;
    bd.StructureByteStride = sizeof(float);

    if (_resource.bindStreamOut())
    {
        bd.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
    }
    if (_resource.bindShaderResource())
    {
        bd.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
    }
    if (_resource.bindRenderTarget())
    {
        bd.BindFlags |= D3D11_BIND_RENDER_TARGET;
    }
    if (_resource.bindUAV())
    {
        bd.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        bd.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        //bd.MiscFlags |=  D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    }

    if (!_resource.dynamic())
    {
        D3D11_SUBRESOURCE_DATA resource;
        memset(&resource, 0, sizeof(D3D11_SUBRESOURCE_DATA));
        resource.pSysMem = _resource.vertexPtr();
        resource.SysMemPitch = _resource.vertexStride();

        if (_resource.useResource())
        {
            // UAV and binding?
            if (FAILED(_direct3d->CreateBuffer(&bd, &resource, &_vertexBuffer)))
            {
                return false;
            }
        }
        else
        {
            if (FAILED(_direct3d->CreateBuffer(&bd, 0, &_vertexBuffer)))
            {
                return false;
            }
        }
    }
    else
    {
        bd.CPUAccessFlags = _usingUploadBuffer ? 0 : D3D11_CPU_ACCESS_WRITE;
        if (FAILED(_direct3d->CreateBuffer(&bd, 0, &_vertexBuffer)))
        {
            return false;
        }
    }

    int elementWidth = _resource.sizeInBytes() / _resource.vertexStride();

    // Create rendertarget view if needed
    if (_resource.bindShaderResource() || _resource.bindUAV())
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
        srDesc.Format = DXGI_FORMAT_R32_FLOAT;
        srDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srDesc.Buffer.ElementOffset = 0;
        srDesc.Buffer.ElementWidth = sizeof(float); // was element width
        srDesc.Buffer.NumElements = _resource.sizeInBytes() / sizeof(float);

        if (FAILED(_direct3d->CreateShaderResourceView(_vertexBuffer, &srDesc, &_shaderResourceView)))
        {
            LOG("FAILED to create shader resource view for vertex buffer");
            return false;
        }
    }

    if (_resource.bindUAV())
    {
        memset(&_uavDesc, 0, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

        _uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        _uavDesc.Buffer.FirstElement = 0;
        _uavDesc.Buffer.NumElements = _resource.sizeInBytes() / sizeof(float);
        _uavDesc.Format = DXGI_FORMAT_R32_FLOAT;

        //_uavDesc.Buffer.Flags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        if (FAILED(_direct3d->CreateUnorderedAccessView(_vertexBuffer, &_uavDesc, &_unorderedResourceView)))
        {
            LOG("Failed to create unordered access view for vertex buffer");
            return false;
        }
    }

    // Create shader view if needed.
    if (_resource.bindRenderTarget())
    {
        D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
        rtDesc.Format = findFormat(_resource.format());
        rtDesc.ViewDimension = D3D11_RTV_DIMENSION_BUFFER;
        rtDesc.Buffer.ElementOffset = 0;
        rtDesc.Buffer.ElementWidth = elementWidth;
        if (FAILED(_direct3d->CreateRenderTargetView(_vertexBuffer, &rtDesc, &_renderTargetView)))
        {
            LOG("FAILED to create render target view for vertex buffer");
            return false;
        }
    }

    // Upload data if required.
    if (_usingUploadBuffer && _resource.useResource())
    {
        upload();
    }

    return true;
}

bool VertexBufferD3D11::free()
{
    saferelease(_vertexBuffer);
    saferelease(_shaderResourceView);
    saferelease(_renderTargetView);
    saferelease(_unorderedResourceView);
    saferelease(_uploadBuffer);

    return true;
}

bool VertexBufferD3D11::cache()
{
    return true;
}

void* VertexBufferD3D11::lock(size_t byteSize)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    if (_isRingBuffer)
    {
        if (byteSize <= 0)
        {
            LOG("Massive mapping failure while attempting to map ring buffer");
        }

        //Align to 16 bytes.
        if (byteSize % 16 != 0)
            byteSize += 16 - (byteSize % 16);


        if ((_bufferCursor + _lastCopySize + byteSize) > _sizeInBytes)
        {
            //_needsDiscard = true;
            _bufferCursor = 0;
        }
        else
        {
            _bufferCursor += _lastCopySize;
        }

        // Stash the last copy size.
        _lastCopySize = byteSize;

        if (SUCCEEDED(_immediateCtx->Map(_vertexBuffer, 0,
            _needsDiscard ? D3D11_MAP(D3D11_MAP_WRITE_DISCARD | D3D11_MAP_WRITE_NO_OVERWRITE) : D3D11_MAP_WRITE_NO_OVERWRITE, 0,
            &mappedResource)))
        {
            _locked = true;
            _needsDiscard = false;
            return ((uint8_t*)mappedResource.pData) + _bufferCursor;
        }
    }
    else
    {
        if (_usingUploadBuffer)
        {
            if (SUCCEEDED(_immediateCtx->Map(_uploadBuffer, 0,
                D3D11_MAP_WRITE_DISCARD, 0,
                &mappedResource)))
            {
                _locked = true;
                return mappedResource.pData;
            }
        }
        else
        {
            if (SUCCEEDED(_immediateCtx->Map(_vertexBuffer, 0,
                D3D11_MAP_WRITE_DISCARD, 0,
                &mappedResource)))
            {
                _locked = true;
                return mappedResource.pData;
            }
        }
    }
    return 0;
}

bool VertexBufferD3D11::unlock()
{
    if (_usingUploadBuffer)
    {
        _immediateCtx->Unmap(_uploadBuffer, 0);
        _locked = false;
        upload();
    }
    else
    {
        _immediateCtx->Unmap(_vertexBuffer, 0);
        _locked = false;
    }

    return true;
}

bool VertexBufferD3D11::bind(uint32_t bufferOffset) const
{
    uint32_t offset = uint32_t(_bufferCursor+bufferOffset);
    uint32_t stride = _vertexDeclaration->vertexStride();
    _immediateCtx->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
    return true;
}

bool VertexBufferD3D11::bindToStreamOut() const
{
    // Set stream out to null
    ID3D11Buffer *buffer[1];
    buffer[0] = _vertexBuffer;
    UINT offset[1] = { 0 };
    _immediateCtx->SOSetTargets(1, buffer, offset);
    return true;
}
}