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

#include <IblGpuVariableD3D11.h>
#include <IblShaderParameterValue.h>
#include <IblTextureMgr.h>
#include <IblTextureD3D11.h>
#include <IblGpuVariableD3D11.h>
#include <IblLog.h>
#include <IblIEffect.h>
#include <IblGpuTechnique.h>
#include <IblEffectD3D11.h>
#include <IblBufferD3D11.h>
#include <IblIndexBufferD3D11.h>
#include <IblVertexBufferD3D11.h>
#include <IblDepthSurfaceD3D11.h>

namespace Ibl
{
GpuVariableD3D11::GpuVariableD3D11(Ibl::IDevice* device) :
GpuVariable (device),
_handle(nullptr),
_name(""),
_semantic(""),
_parameterType(Ibl::UnknownParameter),
_texture(nullptr),
_effectType (nullptr)
{
}

GpuVariableD3D11::~GpuVariableD3D11() 
{
    free();
}

Ibl::ShaderParameter 
GpuVariableD3D11::parameterType() const
{
    return _parameterType;
}

Ibl::IEffect*
GpuVariableD3D11::effect() const 
{ 
    return _effect;
}

ID3DX11EffectVariable*
GpuVariableD3D11::handle() const
{
    return _handle;
}

void GpuVariableD3D11::setParameterType (Ibl::ShaderParameter type)
{
    _parameterType = type;
}

const std::string& 
GpuVariableD3D11::annotation (const std::string& name)
{
    for(auto annotations = _annotations.begin(); annotations != _annotations.end(); annotations++)
    {
        if((annotations)->first == name)
        {
            return (annotations)->second;
        }
    }
    static std::string noAnnotation = "";
    return noAnnotation;
}

bool 
GpuVariableD3D11::initialize (Ibl::IEffect* effectInterface, const int& index)
{
    bool result = false;
    free();
    
    ID3DX11Effect* effect;
    if (Ibl::EffectD3D11* ktrEffect = dynamic_cast<Ibl::EffectD3D11*>(effectInterface))
    {
        effect = ktrEffect->effect();
    }
    else
    {
        return false;
    }

    _handle = effect->GetVariableByIndex (index);
    _handle->GetDesc (&_parameterDescription);

    _effectType = _handle->GetType();
    _effectType->GetDesc (&_variableDesc);

    _name = std::string (_parameterDescription.Name);
    //LOG ("Variable name = " << _name);

    if (_parameterDescription.Semantic != 0)
    {
        std::string semantic (_parameterDescription.Semantic);

        if (Ibl::isIndexedSemantic (semantic))
        {
            // Trim off the semantic index
            _valueIndex = Ibl::semanticIndex (semantic);
            char trimmedSemantic [128];
            memset (trimmedSemantic, 0, sizeof(char) * 128);
            memcpy (trimmedSemantic, semantic.c_str(), semantic.size()-1);
            semantic = std::string (trimmedSemantic);

            //LOG ("Value index is " << _valueIndex << " for " << semantic);
        }
    
        _semantic = semantic;
    }

    for (uint32_t i = 0; i < _parameterDescription.Annotations; i++)
    {
        if (ID3DX11EffectVariable* annotationHandle = _handle->GetAnnotationByIndex (i))
        {
            D3DX11_EFFECT_VARIABLE_DESC desc;
            annotationHandle->GetDesc (&desc);

            ID3DX11EffectType * type = 
                annotationHandle->GetType();
            D3DX11_EFFECT_TYPE_DESC typeDesc;
            type->GetDesc(&typeDesc);
            if (typeDesc.Type != D3D10_SVT_STRING)
                continue;

            if (ID3DX11EffectStringVariable* string =
                annotationHandle->AsString())
            {
                if (string->IsValid())
                {
                    //char buffer[512];
                    //memset (buffer, 0, sizeof (char) * 512);
                    LPCSTR buffer = 0;
                    D3DX11_EFFECT_VARIABLE_DESC stringDesc;
                    string->GetDesc (&stringDesc);
                    string->GetString (&buffer);
                    if (buffer)
                    {
                        //LOG ("Annotation " << desc.Name << " " << buffer);
                    }
                    _annotations.insert(std::make_pair(desc.Name, buffer));
                }
            }
        }
    }

    //Load default textures if needed
    if(_variableDesc.Type == D3D10_SVT_TEXTURE2D || 
       _variableDesc.Type == D3D10_SVT_TEXTURE3D ||
       _variableDesc.Type == D3D10_SVT_TEXTURE)
    {
        //LOG ("We have a texture");
        if(semantic() == "ENVIRONMENTMAP")
        {
            std::string texturefile = annotation("defaultmap");
            if (texturefile.size() > 0)
            {
                _texture = dynamic_cast<const TextureD3D11*> 
                    (_deviceInterface->textureMgr()->loadCubeTexture (texturefile.c_str()));
            }
        }
        if (semantic() == "VOLUMEMAP")
        {
            std::string texturefile = annotation("defaultmap");
            if (texturefile.size() > 0)
            {
                _texture = dynamic_cast<const TextureD3D11*> 
                    (_deviceInterface->textureMgr()->loadThreeD(texturefile.c_str()));
            }
        }
        else
        {
            std::string texturefile = annotation("defaultmap");
            if (const TextureD3D11* texture = dynamic_cast<const TextureD3D11*>
                (_deviceInterface->textureMgr()->loadTexture(texturefile.c_str())))
            {
                _texture = texture;
            }
        }
    }

    _effect = effectInterface;
    return true;
}

bool
GpuVariableD3D11::free()
{
    memset ((void*)&_parameterDescription, 
            0, sizeof(D3DX11_EFFECT_VARIABLE_DESC));

    _handle = nullptr;
    _effectType = nullptr;
    return true;
}

const std::string& 
GpuVariableD3D11::semantic() const
{
    return _semantic;
} 

const std::string& 
GpuVariableD3D11::name() const
{
    return _name;
}

const Ibl::ITexture*
GpuVariableD3D11::texture() const
{
    return _texture;
}

void      
GpuVariableD3D11::unbind() const
{
}

void
GpuVariableD3D11::setFloatArray (const float* value, uint32_t size) const
{
    _handle->AsScalar()->SetFloatArray ((float*)value, 0, size);
}

void
GpuVariableD3D11::set (const void* value, uint32_t size) const
{
    if (ID3DX11EffectVariable* effectVariable = _handle)
    {
        effectVariable->SetRawValue ((void*)value, 0, size);
    }
}

void
GpuVariableD3D11::setMatrix(const float* value) const
{
    if (ID3DX11EffectMatrixVariable* effectVariable = _handle->AsMatrix())
    {
        effectVariable->SetMatrix ((float*)(void*)value);
    }
}

void
GpuVariableD3D11::setMatrixArray (const float* value, uint32_t count) const
{
    if (ID3DX11EffectMatrixVariable* matrixVariable = _handle->AsMatrix())
    {
        matrixVariable->SetMatrixArray ((float*)value, 0, count);
        return;
    }
}

void
GpuVariableD3D11::setVectorArray (const float* value, uint32_t count) const
{
    if (ID3DX11EffectVectorVariable* vectorVariable =
        _handle->AsVector())
    {
        vectorVariable->SetFloatVectorArray ((float*)value, 0, count);
        return;
    }
}

void
GpuVariableD3D11::setVector (const float* value) const
{
    //if (ID3DX11EffectVariable* effectVariable = _handle)
    //{
        if (ID3DX11EffectVectorVariable* vectorVariable =
            _handle->AsVector())
        {
            vectorVariable->SetFloatVector ((float*)(void*)value);
            return;
        }
    //}
}

void
GpuVariableD3D11::setTexture (const Ibl::ITexture* texture) const
{
    {
        if (ID3DX11EffectShaderResourceVariable* resourceVariable =
            _handle->AsShaderResource())
        {
            ID3D11ShaderResourceView * resourceView = nullptr;
            resourceView = ((const Ibl::TextureD3D11*)texture)->resourceView(texture->activeSlice(), texture->activeMipId());
            resourceVariable->SetResource (resourceView);
            return;
        }
    }
    LOG ("Bug in set texture...\n");
}

void
GpuVariableD3D11::setUnorderedResource(const Ibl::IGpuBuffer* buffer) const
{
    if (buffer)
    {
        if (ID3DX11EffectUnorderedAccessViewVariable* resourceVariable =
            _handle->AsUnorderedAccessView())
        {
            ID3D11UnorderedAccessView * resourceView = 
                ((const Ibl::BufferD3D11*)buffer)->unorderedView();
            if (FAILED(resourceVariable->SetUnorderedAccessView (resourceView)))
            {
                LOG ("Failed set buffer resource...");
            }
        }
    }
}

void
GpuVariableD3D11::setResource (const Ibl::IGpuBuffer* buffer) const
{
    if (buffer)
    {
        if (ID3DX11EffectShaderResourceVariable* resourceVariable =
            _handle->AsShaderResource())
        {
            ID3D11ShaderResourceView * resourceView = ((const Ibl::BufferD3D11*)buffer)->resourceView();
            if (SUCCEEDED(resourceVariable->SetResource (resourceView)))
            {
                return;
            }
            else
            {
                LOG ("Failed set buffer resource...");
            }
        }
    }
}

void
GpuVariableD3D11::setStream (const Ibl::IVertexBuffer* vertexBuffer) const
{
    if (ID3DX11EffectShaderResourceVariable* resourceVariable =
        _handle->AsShaderResource())
    {
        ID3D11ShaderResourceView * resourceView = ((const Ibl::VertexBufferD3D11*)vertexBuffer)->shaderResourceView();
        resourceVariable->SetResource(resourceView);
    }
}

void
GpuVariableD3D11::setStream (const Ibl::IIndexBuffer* indexBuffer) const
{
    if (ID3DX11EffectShaderResourceVariable* resourceVariable =
        _handle->AsShaderResource())
    {
        ID3D11ShaderResourceView * resourceView = ((const Ibl::IndexBufferD3D11*)indexBuffer)->resourceView();
        resourceVariable->SetResource(resourceView);
    }
}

void
GpuVariableD3D11::setDepthTexture (const Ibl::IDepthSurface* depthSurface) const
{
    if (ID3DX11EffectShaderResourceVariable* resourceVariable =
        _handle->AsShaderResource())
    {
        ID3D11ShaderResourceView * resourceView = ((const Ibl::DepthSurfaceD3D11*)depthSurface)->resourceView();
        resourceVariable->SetResource (resourceView);
    
        return;
    }
}

}