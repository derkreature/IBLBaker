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

#include <IblIndexBufferD3D11.h>
#include <IblIRenderResourceParameters.h>
#include <IblFormatConversionD3D11.h>

namespace Ibl
{
IndexBufferD3D11::IndexBufferD3D11(Ibl::DeviceD3D11* device) :
    IIndexBuffer(device),
    _locked(false),
    _resource(0, false),
    _indexBuffer(0),
    _resourceView(0),
    _direct3d(nullptr),
    _immediateCtx(nullptr),
    _sizeInBytes(0),
    _bufferCursor(0),
    _lastCopySize(0),
    _needsDiscard(true),
    _isRingBuffer(false),
    _stagingBuffer(nullptr)
{
    if (Ibl::DeviceD3D11* _device =
        dynamic_cast <Ibl::DeviceD3D11*>(device))
    {
        _direct3d = *_device;
        _immediateCtx = _device->immediateCtx();
    }
}

IndexBufferD3D11::~IndexBufferD3D11()
{
    free();
}

ID3D11ShaderResourceView * IndexBufferD3D11::resourceView() const
{
    return _resourceView;
}

bool IndexBufferD3D11::initialize(const Ibl::IndexBufferParameters* data)
{
    _resource = Ibl::IndexBufferParameters(*data);
    return IndexBufferD3D11::create();
}

bool IndexBufferD3D11::bind(uint32_t offset) const
{
    _immediateCtx->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, (uint32_t)(_bufferCursor + offset));
    return true;
}

bool IndexBufferD3D11::create()
{
    D3D11_BUFFER_DESC bd;
    memset(&bd, 0, sizeof(D3D11_BUFFER_DESC));

    _sizeInBytes = _resource.sizeInBytes();
    _dynamic = _resource.dynamic();
    _isRingBuffer = _resource.ringBuffered();

    // No overwrite buffer for 5 frames.
    if (_isRingBuffer)
        _sizeInBytes *= 5;


    bd.Usage = _dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
    bd.ByteWidth = (uint32_t)(_sizeInBytes);
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = _dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
    bd.MiscFlags = 0;


    return SUCCEEDED(_direct3d->CreateBuffer(&bd, 0, &_indexBuffer));
}

bool IndexBufferD3D11::free()
{
    saferelease(_indexBuffer);
    saferelease(_resourceView);
    saferelease(_stagingBuffer)
        return true;
}

bool IndexBufferD3D11::cache()
{
    return true;
}

void* IndexBufferD3D11::lock(size_t byteSize)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    if (_dynamic)
    {
        D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD;
        if (_isRingBuffer)
        {
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

        }
        mapType = _needsDiscard ? (D3D11_MAP)(D3D11_MAP_WRITE_DISCARD | D3D11_MAP_WRITE_NO_OVERWRITE) : D3D11_MAP_WRITE_NO_OVERWRITE;

        // CPU Access must be mapped for write.
        if (SUCCEEDED(_immediateCtx->Map(_indexBuffer,
            0,
            mapType,
            0,
            &mappedResource)))
        {
            _locked = true;
            return ((uint8_t*)mappedResource.pData) + _bufferCursor;
        }
    }
    else
    {
        D3D11_BUFFER_DESC bd;
        memset(&bd, 0, sizeof(D3D11_BUFFER_DESC));
        bd.Usage = D3D11_USAGE_STAGING;
        bd.ByteWidth = (uint32_t)(_sizeInBytes);
        bd.BindFlags = 0;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

        if (SUCCEEDED(_direct3d->CreateBuffer(&bd, 0, &_stagingBuffer)))
        {
            if (SUCCEEDED(_immediateCtx->Map(_stagingBuffer,
                0,
                D3D11_MAP_READ_WRITE,
                0,
                &mappedResource)))
            {
                _locked = true;
                return mappedResource.pData;
            }
            else
            {
                LOG("FAILED TO MAP STAGING BUFFER FOR INDEX BUFFER");
            }
        }
        else
        {
            LOG("FAILED TO CREATE STAGING BUFFER FOR INDEX BUFFER");
        }
    }
    return 0;
}

bool IndexBufferD3D11::unlock()
{
    if (_dynamic)
    {
        _immediateCtx->Unmap(_indexBuffer, 0);
    }
    else
    {
        _immediateCtx->CopyResource(_indexBuffer, _stagingBuffer);
        saferelease(_stagingBuffer);
        _stagingBuffer = nullptr;
    }

    _locked = false;
    return true;
}
}