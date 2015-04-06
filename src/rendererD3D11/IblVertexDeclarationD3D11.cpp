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


#include <IblVertexDeclarationD3D11.h>
#include <IblRenderDeviceD3D11.h>
#include <IblVertexElement.h>
#include <IblLog.h>

namespace Ibl
{
namespace 
{
static std::string positionSemantic = "POSITION";
static std::string normalSemantic = "NORMAL";
static std::string texCoordSemantic = "TEXCOORD";
static std::string tangentSemantic = "TANGENT";
static std::string binormalSemantic = "BINORMAL";
static std::string bonesSemantic = "BLENDINDICES";
static std::string weightsSemantic = "BLENDWEIGHT";
static std::string colorSemantic = "COLOR";
static std::string seedSemantic = "SEED";
static std::string speedSemantic = "SPEED";
static std::string randSemantic = "RAND";
static std::string typeSemantic = "TYPE";
static std::string unknownSemantic = "Unknown";
static std::string stateSemantic = "STATE";
static std::string oldStateSemantic = "OLDSTATE";

const std::string&
usageToString (BYTE usage)
{
    if (usage == Ibl::POSITION)
    {
        return positionSemantic;
    }
    if (usage == Ibl::NORMAL)
    {
        return normalSemantic;
    }
    if (usage == Ibl::TEXCOORD)
    {
        return texCoordSemantic;
    }
    if (usage == Ibl::TANGENT)
    {
        return tangentSemantic;
    }
    if (usage == Ibl::BINORMAL)
    {
        return binormalSemantic;
    }
    if (usage == Ibl::BLENDINDICES)
    {
        return bonesSemantic;
    }
    if (usage == Ibl::BLENDWEIGHT)
    {
        return weightsSemantic;
    }
    if (usage == Ibl::COLOR)
    {
        return colorSemantic;
    }
    if (usage == Ibl::SEED)
    {
        return seedSemantic;
    }
    if (usage == Ibl::SPEED)
    {
        return speedSemantic;
    }
    if (usage == Ibl::RAND)
    {
        return randSemantic;
    }
    if (usage == Ibl::TYPE)
    {
        return typeSemantic;
    }
    if (usage == Ibl::STATE)
    {
        return stateSemantic;
    }
    if (usage == Ibl::OLDSTATE)
    {
        return oldStateSemantic;
    }
    return unknownSemantic;
}
}

DXGI_FORMAT
typeToFormat (Ibl::DeclarationType type)
{
    switch (type)
    {
        case Ibl::FLOAT4: 
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case Ibl::FLOAT3:  
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case Ibl::FLOAT2: 
            return DXGI_FORMAT_R32G32_FLOAT;
        case Ibl::FLOAT1: 
            return DXGI_FORMAT_R32_FLOAT;
        case Ibl::UBYTE4:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case Ibl::UINT8:
            return DXGI_FORMAT_R8_UINT;
        case Ibl::UINT32:
            return DXGI_FORMAT_R32_UINT;
        default:
            break;
    };
    return DXGI_FORMAT_UNKNOWN;
}

VertexDeclarationD3D11::VertexDeclarationD3D11 (Ibl::DeviceD3D11* device) :
    Ibl::IVertexDeclaration (device),
    _vertexAttributes (nullptr),
    _elementCount (0),
    _vertexStride (0),
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

VertexDeclarationD3D11::~VertexDeclarationD3D11()
{
    _declaration = std::vector <Ibl::VertexElement>();
    safedeletearray(_vertexAttributes);
}

const std::vector <Ibl::VertexElement>      & 
VertexDeclarationD3D11::getDeclaration() const
{
    return _declaration;
}

bool
VertexDeclarationD3D11::free()
{
    return true;
}

bool
VertexDeclarationD3D11::create()
{
    return (_vertexStride > 0);
}

bool
VertexDeclarationD3D11::cache()
{
    return true;
}

bool
VertexDeclarationD3D11::initialize (const Ibl::VertexDeclarationParameters* resource)
{  
    safedelete(_vertexAttributes);
    const std::vector<Ibl::VertexElement> & elements = resource->elements();

    if (elements.size() == 0)
        return false;
    _vertexAttributes = new D3D11_INPUT_ELEMENT_DESC[elements.size()];

    uint32_t index = 0;
    for (uint32_t i = 0; i < elements.size() - 1; i++)
    {
        const Ibl::VertexElement& element = elements[i];
        _vertexAttributes[index] = 
            createVertexElement (element.stream(), element.offset(), 
                                 element.type(), element.method(), 
                                 element.usage(), element.usageIndex(),
                                 element.streamIndex());
        index++;
    }

    Ibl::VertexElement element = elements[elements.size()-2];
    _vertexStride = (element.offset() + VertexDeclarationD3D11::elementToSize(element.type()));    
    _elementCount = (uint32_t)(elements.size()) - 1;    


    index = 0;
    for (auto it = elements.begin();
         it != elements.end();
         it++)
    {
        _declaration.push_back (*it);
        index++;
    }

    return create();
}

bool
VertexDeclarationD3D11::bind() const
{
    return false;
}

D3D11_INPUT_ELEMENT_DESC
VertexDeclarationD3D11::createVertexElement (WORD stream, 
                                             WORD offset, 
                                             BYTE type, 
                                             BYTE method, 
                                             BYTE usage, 
                                             BYTE usageIndex,
                                             BYTE streamIndex)
{
    D3D11_INPUT_ELEMENT_DESC element = 
    { usageToString((Ibl::DeclarationType)usage).c_str(), 
      usageIndex, 
      typeToFormat((Ibl::DeclarationType)type), 
      streamIndex, 
      offset, 
      D3D11_INPUT_PER_VERTEX_DATA, 
      0 };

    return element;
}

bool 
VertexDeclarationD3D11::isTypeOf (const Ibl::DeclarationUsage& usage, const uint32_t& index)
{
    // mdavidson, casulatiy of integration
    LOG ("Is this really still used?");
    return false;
}

uint32_t
VertexDeclarationD3D11::vertexStride() const
{
    return _vertexStride;
}

uint32_t
VertexDeclarationD3D11::vertexStreamCount() const
{
    // Always float.
    return _vertexStride / sizeof(float);
}


}