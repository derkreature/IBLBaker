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

#include <IblInputLayoutCacheD3D11.h>
#include <IblVertexDeclarationD3D11.h>
#include <IblLog.h>


namespace Ibl
{
InputLayoutCacheD3D11::InputLayoutCacheD3D11(Ibl::IDevice* device) :
    _valid (false),
    _pass (nullptr),
    _direct3d(nullptr),
    _immediateCtx(nullptr)
{
    if (Ibl::DeviceD3D11* _device = 
        dynamic_cast <Ibl::DeviceD3D11*>(device))
    {
        _direct3d = *_device;
        _immediateCtx = _device->immediateCtx();
    }
}

InputLayoutCacheD3D11::~InputLayoutCacheD3D11(void)
{
    for (auto it = _layouts.begin(); it != _layouts.end(); it++)
    {
        ID3D11InputLayout* layout = it->second;
        saferelease(layout);
    }

    // Apparently pass is owned by _technique
    // safedelete(_pass);
}

void
InputLayoutCacheD3D11::initialize (ID3DX11EffectTechnique* technique, uint32_t passIndex)
{
    _technique = technique;
    _passIndex = passIndex;

    if (_pass = _technique->GetPassByIndex(passIndex))
    {
        _pass->GetDesc (&_passDesc);
        _valid = true;
    }
}

bool
InputLayoutCacheD3D11::bindLayout (const Ibl::VertexDeclarationD3D11* vertexDeclaration) const
{
    auto it = _layouts.find (vertexDeclaration);

    if (it == _layouts.end())
    {
        ID3D11InputLayout* inputLayout = 0;
        uint32_t elementCount = vertexDeclaration->elementCount();
        const D3D11_INPUT_ELEMENT_DESC* inputDesc = vertexDeclaration->layout();

        if (FAILED (_direct3d->CreateInputLayout (inputDesc, 
                                                  elementCount,
                                                  _passDesc.pIAInputSignature, 
                                                  _passDesc.IAInputSignatureSize, 
                                                  &inputLayout)))
        {
            LOG ("Failed to setup input layout");
            return false;
        }

        _layouts.insert (std::make_pair (vertexDeclaration, inputLayout));
        it = _layouts.find (vertexDeclaration);
    }

    if (ID3D11InputLayout* layout = it->second)
    {
        _immediateCtx->IASetInputLayout (layout);
        return true;
    }
    return false;
}

}
