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

#include <IblUIRenderer.h>
#include <IblIVertexDeclaration.h>
#include <IblVertexElement.h>
#include <IblVertexDeclarationMgr.h>
#include <IblMaterial.h>
#include <IblIVertexBuffer.h>
#include <IblIDevice.h>
#include <IblIIndexBuffer.h>
#include <IblGpuTechnique.h>
#include <IblGpuVariable.h>
#include <IblIShader.h>
#include <IblScene.h>

namespace Ibl
{
UIRenderer::UIRenderer(Ibl::IDevice* device) :
    Ibl::Mesh (device),
    _vertexOffset(0)
{
    setDynamic (true);
    setPrimitiveType(Ibl::TriangleList);

    // Setup a Default Material.
}

UIRenderer::~UIRenderer()
{
}


void
UIRenderer::setDrawIndexed(bool drawIndexed)
{

}

void
UIRenderer::setVertexBuffer(IVertexBuffer* vertexBuffer)
{
    _currentVertexBuffer = vertexBuffer;
}

void
UIRenderer::setShader(const Ibl::IShader* shader)
{
    _material->setShader(shader);
    _material->setTechnique(shader->getTechnique(0));
}

IVertexBuffer*
UIRenderer::vertexBuffer(IVertexDeclaration* declaration)
{
    return nullptr;
}

IIndexBuffer*
UIRenderer::indexBuffer()
{
    return _indexBuffer;
}

bool
UIRenderer::render(const Ibl::RenderRequest* request,
                   const Ibl::GpuTechnique* technique) const
{
    if (_drawIndexed)
    {
        return _device->drawIndexedPrimitive(_currentVertexBuffer->vertexDeclaration(), 
                                             _indexBuffer,
                                             _currentVertexBuffer, technique, (PrimitiveType)primitiveType(),
                                             _primitiveCount, 0, _vertexOffset);

    }
    else
    {

        return _device->drawPrimitive(_vertexDeclaration, _currentVertexBuffer, technique,
            (PrimitiveType)primitiveType(), _primitiveCount, _vertexOffset);
    }

    return true;
}

void
UIRenderer::render(uint32_t count, uint32_t vertexOffset)
{
    _vertexOffset = vertexOffset;
    // Count is index or vertex count depending on indexed or not. Convert.
    const Ibl::IShader* shader = material()->shader();

    if (primitiveType() == Ibl::TriangleList)
    {
        _primitiveCount = count / 3;
    }
    else if (primitiveType() == Ibl::TriangleStrip)
    {
        _primitiveCount = (count-1) / 2;
    }


    // Try to setup ortho if it is available.
    shader->renderMesh (Ibl::RenderRequest(material()->technique(), nullptr, nullptr, this));
}

}
