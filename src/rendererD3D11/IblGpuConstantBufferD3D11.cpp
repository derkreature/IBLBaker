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

#include <IblGpuConstantBufferD3D11.h>
#include <IblShaderParameterValue.h>
#include <IblTextureMgr.h>
#include <IblTextureD3D11.h>
#include <IblEffectD3D11.h>
#include <IblLog.h>
#include <IblIEffect.h>
#include <IblGpuTechnique.h>
#include <IblBufferD3D11.h>

namespace Ibl
{
ConstantBufferD3D11::ConstantBufferD3D11() :
GpuConstantBuffer (0),
_handle(nullptr),
_name(""),
_semantic(""),
_parameterType(Ibl::UnknownParameter),
_effectType (nullptr)
{
}

ConstantBufferD3D11::~ConstantBufferD3D11() 
{
    free();
}


Ibl::ShaderParameter 
ConstantBufferD3D11::parameterType() const
{
    return _parameterType;
}

Ibl::IEffect*
ConstantBufferD3D11::effect() const 
{ 
    return _effect;
}

ID3DX11EffectConstantBuffer*
ConstantBufferD3D11::handle() const
{
    return _handle;
}

void ConstantBufferD3D11::setParameterType (Ibl::ShaderParameter type)
{
    _parameterType = type;
}

const std::string& 
ConstantBufferD3D11::annotation (const std::string& name)
{
    for(auto annotations = _annotations.begin(); annotations != _annotations.end(); annotations++)
    {
        if((annotations)->first == name)
        {
            return (annotations)->second;
        }
    }
    static const std::string noAnnotation = "";
    return noAnnotation;
}

bool 
ConstantBufferD3D11::initialize (Ibl::IEffect* effectInterface, const int& index)
{
    bool result = false;
    free();
    
    ID3DX11Effect* effect;
    if (EffectD3D11* effectPtr = dynamic_cast<Ibl::EffectD3D11*>(effectInterface))
    {
        effect = effectPtr->effect();
    }
    else
    {
        return false;
    }

    _handle = effect->GetConstantBufferByIndex (index);
    _handle->GetDesc (&_parameterDescription);


    _effectType = _handle->GetType();
    _effectType->GetDesc (&_variableDesc);

    _name = std::string (_parameterDescription.Name);
    _effect = effectInterface;

    return true;
}

bool
ConstantBufferD3D11::free()
{
    memset ((void*)&_parameterDescription, 
            0, sizeof(D3DX11_EFFECT_VARIABLE_DESC));

    // Effect and handles are owned by the d3dx effects framework.
    _effectType = nullptr;
    _handle = nullptr;
    return true;
}

const std::string& 
ConstantBufferD3D11::semantic() const
{
    return _semantic;
} 

const std::string& 
ConstantBufferD3D11::name() const
{
    return _name;
}

void
ConstantBufferD3D11::setConstantBuffer (const Ibl::IGpuBuffer* buffer) const
{
    if (buffer)
    {
       if (ID3DX11EffectConstantBuffer* resourceVariable = _handle)
       {
           ID3D11Buffer * constantBuffer = ((const Ibl::BufferD3D11*)buffer)->buffer();
    
           if (SUCCEEDED(resourceVariable->SetConstantBuffer(constantBuffer)))
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


}