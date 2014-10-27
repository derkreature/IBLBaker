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

#include <IblGpuTechniqueD3D11.h>
#include <IblVertexDeclarationD3D11.h>
#include <IblInputLayoutCacheD3D11.h>
#include <IblMesh.h>

namespace Ibl
{
GpuTechniqueD3D11::GpuTechniqueD3D11(Ibl::IDevice * device) :
GpuTechnique (device),
_handle(0),
_name(""),
_description(""),
_effect (0),
_hasTessellationStage (false)
{
    if (Ibl::DeviceD3D11* _device = 
        dynamic_cast <Ibl::DeviceD3D11*>(device))
    {
        _direct3d = *_device;
        _immediateCtx = _device->immediateCtx();
    }

    memset (&_desc, 0, sizeof (D3DX11_TECHNIQUE_DESC));
}

GpuTechniqueD3D11::~GpuTechniqueD3D11()
{
    free();
}

bool
GpuTechniqueD3D11::hasTessellationStage() const
{
    return _hasTessellationStage;
}

Ibl::IEffect* 
GpuTechniqueD3D11::effect () const
{
    return _effect;
}

const D3DX11_TECHNIQUE_DESC& 
GpuTechniqueD3D11::description() const
{
    return _desc;
}

// Yes the fact that this function is const is retarded.
bool
GpuTechniqueD3D11::setupInputLayout (const Ibl::Mesh* mesh,
                                     uint32_t passIndex) const
{
    if (const Ibl::VertexDeclarationD3D11* declaration = 
        dynamic_cast<const Ibl::VertexDeclarationD3D11*>(mesh->vertexDeclaration()))
    {
        if (passIndex < _layoutCache.size())
        {
            return _layoutCache[passIndex]->bindLayout (declaration);
        }
        return false;
    }
    else
    {
        // TODO: Debug. Imagine. Dream.
        // _immediateCtx->IASetInputLayout (nullptr); Removing, temprarily. MATTD. UGGH>.
        // NULL VERTEX LAYOUT. ASSERT FOR NOW. I have no idea why this is here.
        // I guess something that I was working on for someone else crept in here.
        //
        assert(0);
    }
    return true;
}
bool 
GpuTechniqueD3D11::initialize (Ibl::IEffect* effect, 
                               ID3DX11EffectTechnique* techniqueHandle)
{
    if (_effect = dynamic_cast<EffectD3D11*>(effect))
    {
        techniqueHandle->GetDesc (&_desc);

        _name = _desc.Name;
        _handle = techniqueHandle;
    
        if (ID3DX11EffectPass * _passHandle =
            techniqueHandle->GetPassByIndex(0))
        {
            D3DX11_PASS_SHADER_DESC hullDesc;
            if (SUCCEEDED(_passHandle->GetHullShaderDesc(&hullDesc)))
            {
                if (ID3DX11EffectShaderVariable * variable = hullDesc.pShaderVariable)
                {
                    _hasTessellationStage = (variable->IsValid() == TRUE);
                    // if (_hasTessellationStage)
                    //    LOG ("Found tessellation stage in " << _name);                    
                }
            }
        }

        char buffer [512];
        for(uint32_t annotation = 0; annotation < _desc.Annotations; annotation++)
        {
            memset (buffer, 0, sizeof(char) * 512);
            ID3DX11EffectVariable* effectVariable =
                techniqueHandle->GetAnnotationByIndex (annotation);
        }
    }

    // For each pass push back an inputlayout cache.
    for (uint32_t passIndex = 0; passIndex < _desc.Passes; passIndex++)
    {
        Ibl::InputLayoutCacheD3D11* layoutCache = new Ibl::InputLayoutCacheD3D11(_deviceInterface);
        layoutCache->initialize (_handle, passIndex);
        _layoutCache.push_back (layoutCache);
    }

    return true;
}

bool GpuTechniqueD3D11::free()
{
    _name = "";
    _description = "";
    _handle = 0;

    for (auto it = _layoutCache.begin(); it != _layoutCache.end(); it++)
    {
        safedelete(*it);
    }
    _layoutCache.clear();

    return true;
}

const std::string& GpuTechniqueD3D11::name() const
{
    return _name;
}

void
GpuTechniqueD3D11::setHandle(ID3DX11EffectTechnique* handle)
{
    _handle = handle;
}

ID3DX11EffectTechnique*
GpuTechniqueD3D11::handle() const
{
    return _handle;
}


}