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

#include <IblComputeShaderD3D11.h>
#include <IblRenderTargetTextureD3D11.h>
#include <IblBufferD3D11.h>
#include <IblVertexBufferD3D11.h>
#include <IblTexture2DD3D11.h>
#include <IblAssetManager.h>
#include <IblLog.h>
#include <D3Dcompiler.h>

namespace Ibl
{
namespace
{
ID3D10Blob* compileShaderFromStream(const char* stream,
                                    const char* functionName,
                                    const char* profile,
                                    CONST D3D_SHADER_MACRO* defines)
{
    UINT flags = D3D10_SHADER_SKIP_OPTIMIZATION;
    ID3D10Blob* compiledShader = nullptr;
    ID3D10Blob * errors = nullptr;

    if (FAILED(D3DCompile(stream, strlen(stream), nullptr, defines, nullptr, functionName, profile,
                                      flags, 0,  &compiledShader, &errors)))
    {
        if (errors)
        {
            LOG ("Failed to compile ... " << stream << " for " << functionName << "\n");
            if(errors) 
            { 
                char* buffer = new char[errors->GetBufferSize()+1];
                memset(&buffer[0], 0, errors->GetBufferSize()+1);
                memcpy(&buffer[0], errors->GetBufferPointer(), errors->GetBufferSize());

                LOG ("Error Compiling ShaderD3D11 from stream" << buffer);
                safedeletearray(buffer);
             } 
        }
        return nullptr;
    }
    else
        return compiledShader;
}
}

ComputeShaderD3D11::ComputeShaderD3D11 (Ibl::IDevice* device) :
    Ibl::IComputeShader(device),
    _computeShader(nullptr),
    _constantBuffer(nullptr),
    _direct3d(nullptr),
    _immediateCtx(nullptr)
{
    if (Ibl::DeviceD3D11* _device = 
        dynamic_cast <Ibl::DeviceD3D11*>(device))
    {
        _direct3d = *_device;
        _immediateCtx = _device->immediateCtx();
    }

    // Setup sa default sampler state for inputs.
    D3D11_SAMPLER_DESC sampDesc;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.BorderColor[0] = 0;
    sampDesc.BorderColor[1] = 0;
    sampDesc.BorderColor[2] = 0;
    sampDesc.BorderColor[3] = 0;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.MaxAnisotropy = 1;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    sampDesc.MinLOD = 0;
    sampDesc.MipLODBias = 0;
    _direct3d->CreateSamplerState(&sampDesc, &_pointSampler);

}

ComputeShaderD3D11::~ComputeShaderD3D11()
{
    free();

    saferelease(_pointSampler);
    safedelete(_constantBuffer);
}

bool
ComputeShaderD3D11::create() 
{ 
    D3D10_SHADER_MACRO* definesPtr = nullptr;
    if (_defines.size() > 0)
        &_defines[0];


    std::string includeStream;
    if (_includeFilePathName.length() > 0)
    {
        LOG("Loading compute include " << _includeFilePathName);
        if (std::unique_ptr<typename DataStream> fileStream =
            std::unique_ptr<typename DataStream>(AssetManager::assetManager()->openStream(_includeFilePathName)))
        {
            size_t bufferSize = fileStream->size();
            char* stringBuffer = new char[bufferSize + 1];
            memset(stringBuffer, 0, bufferSize + 1);
            fileStream->read(stringBuffer, bufferSize);
            includeStream = std::string(stringBuffer);
            safedeletearray(stringBuffer);
        }
    }

    std::string shaderStream;
    if (filePathName().length() > 0)
    {
        LOG("Loading compute shader " << filePathName());

        if (std::unique_ptr<typename DataStream> fileStream =
            std::unique_ptr<typename DataStream>(AssetManager::assetManager()->openStream(filePathName())))
        {
            size_t bufferSize = fileStream->size();
            char* stringBuffer = new char[bufferSize + 1];
            memset(stringBuffer, 0, bufferSize + 1);
            fileStream->read(stringBuffer, bufferSize);
            shaderStream = std::string(stringBuffer);
            safedeletearray(stringBuffer);
        }
    }

    _stream = includeStream + "\n" + shaderStream;

    _hash.build(_stream);
    if (ID3DBlob* compiledShader = compileShaderFromStream(_stream.c_str(), _functionName.c_str(), "cs_5_0", definesPtr))
    {
        if (SUCCEEDED(_direct3d->CreateComputeShader(compiledShader->GetBufferPointer(),
            compiledShader->GetBufferSize(), nullptr, &_computeShader)))
        {
            return true;
        }
    }
    return false;
}

bool
ComputeShaderD3D11::free() 
{ 
    saferelease (_computeShader);
    return true;
}

bool
ComputeShaderD3D11::cache() 
{
    return true;
}

const std::string&
ComputeShaderD3D11::filePathName() const
{
    return _filePathName;
}


const std::string&
ComputeShaderD3D11::includePathName() const
{
    return _includeFilePathName;
}

D3D10_SHADER_MACRO*
ComputeShaderD3D11::setupDefines(const std::map<std::string, std::string> & defines)
{
    if (defines.size() > 0)
    {
        for (auto it = defines.begin(); it != defines.end(); it++)
        {
            D3D10_SHADER_MACRO macro = { (*it).first.c_str(), (*it).second.c_str() };
            _defines.push_back(macro);
        }

        D3D10_SHADER_MACRO nullMacro = { 0, 0 };
        _defines.push_back(nullMacro);
        return &_defines[0];
    }
    return nullptr;
}

bool
ComputeShaderD3D11::initializeFromFile (const std::string& shaderFilePathName,
                                        const std::string& includeFilePathName,
                                        const std::string& functionName,
                                        const std::map<std::string, std::string> & defines)
{
    _filePathName = shaderFilePathName;
    _includeFilePathName = includeFilePathName;
    _stream = "";
    _functionName = functionName;
    setupDefines(defines);

    return create();
}

bool
ComputeShaderD3D11::initializeFromStream (const std::string& stream,
                                          const std::string& functionName,
                                          const std::map<std::string, std::string> & defines)
{
    _filePathName = "";
    _stream = stream;
    _functionName = functionName;
    setupDefines(defines);

    return create();
}

bool
ComputeShaderD3D11::createConstantBuffer(size_t size)
{
    if (!_constantBuffer)
    {
        Ibl::GpuBufferParameters constantBufferResourceData =
            Ibl::GpuBufferParameters::setupConstantBuffer
            (static_cast<size_t>(size + (16 - (size % 16))));

        _constantBuffer = _deviceInterface->createBufferResource(&constantBufferResourceData);
    }

    return _constantBuffer != nullptr;
}

bool
ComputeShaderD3D11::updateConstantBuffer(void* src, size_t bytes)
{
    if (_constantBuffer)
    {
        if (void * data = _constantBuffer->lock())
        {
            memcpy(data, src, bytes);
            _constantBuffer->unlock();
            return true;
        }
    }
    return false;
}

bool
ComputeShaderD3D11::bind() const
{
    ID3D11SamplerState* samplers[4] = { _pointSampler, nullptr, nullptr, nullptr }; 
    _immediateCtx->CSSetSamplers(0, 4, samplers);

    _immediateCtx->CSSetShader(_computeShader, nullptr, 0);

    return true;
}

bool
ComputeShaderD3D11::unbind() const
{
    ID3D11ShaderResourceView * srViews[8] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    _immediateCtx->CSSetShaderResources(0, 8, srViews);

    ID3D11UnorderedAccessView * uaViews[8] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    _immediateCtx->CSSetUnorderedAccessViews(0, 8, uaViews, nullptr);

    _immediateCtx->CSSetShader(nullptr, nullptr, 0);
    return true;
}

bool
ComputeShaderD3D11::dispatch(const Ibl::Vector3i& groupBounds) const
{
    _immediateCtx->Dispatch(groupBounds.x, groupBounds.y, groupBounds.z);
    return true;
}

bool
ComputeShaderD3D11::setResources(const std::vector<const Ibl::IRenderResource*>      & resources) const
{
    ID3D11ShaderResourceView* shaderViews[8] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    uint32_t count = 0;
    for (size_t i = 0; i < resources.size(); i++)
    {
        if (const Ibl::BufferD3D11* buffer = 
            dynamic_cast<const Ibl::BufferD3D11*>(resources[i]))
        {
            shaderViews[count++] = buffer->resourceView();
        }
        else if (const Ibl::RenderTargetTextureD3D11* texture = 
            dynamic_cast<const Ibl::RenderTargetTextureD3D11*>(resources[i]))
        {
            shaderViews[count++] = texture->resourceView();
        }
        else if (const Ibl::Texture2DD3D11* texture = 
                 dynamic_cast<const Ibl::Texture2DD3D11*>(resources[i]))
        {
            shaderViews[count++] = texture->resourceView();
        }
        else if (const Ibl::VertexBufferD3D11* buffer =
                 dynamic_cast<const Ibl::VertexBufferD3D11*>(resources[i]))
        {
            shaderViews[count++] = buffer->shaderResourceView();
        }
        else
        {
            LOG ("Logic error when attempting to set compute shader resources " << 
                 __FILE__ << " " << __LINE__); 
        }
    }

    _immediateCtx->CSSetShaderResources(0, 8, shaderViews);

    if (_constantBuffer)
    {
        ID3D11Buffer* buffer = dynamic_cast<Ibl::BufferD3D11*>(_constantBuffer)->buffer();
        _immediateCtx->CSSetConstantBuffers(count, 1, &buffer);
    }


    return true;
}

bool
ComputeShaderD3D11::setViews(const std::vector<const Ibl::IRenderResource*>      & resources) const
{
    ID3D11UnorderedAccessView* uaViews[8] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    uint32_t count = 0;
    for (size_t i = 0; i < resources.size(); i++)
    {
        if (const Ibl::BufferD3D11* buffer = 
            dynamic_cast<const Ibl::BufferD3D11*>(resources[i]))
        {
            uaViews[count++] = buffer->unorderedView();
        }
        else if (const Ibl::RenderTargetTextureD3D11* texture = 
                 dynamic_cast<const Ibl::RenderTargetTextureD3D11*>(resources[i]))
        {       
            uaViews[count++] = texture->unorderedView();
        }
        else if (const Ibl::VertexBufferD3D11* vb = 
                 dynamic_cast<const Ibl::VertexBufferD3D11*>(resources[i]))
        {
            uaViews[count++] = vb->unorderedResourceView();
        }
        else
        {
            LOG ("Logic error when attempting to set compute shader views " << __FILE__ << " " << __LINE__);
        }
    }

    _immediateCtx->CSSetUnorderedAccessViews(0, 4, uaViews, nullptr);

    return true;
}

}
