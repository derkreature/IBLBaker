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

#ifndef INCLUDED_IBL_UI_RENDERER
#define INCLUDED_IBL_UI_RENDERER

#include <IblPlatform.h>
#include <IblMesh.h>
#include <IblGpuTechnique.h>
#include <IblColor.h>

namespace Ibl
{
class IShader;
class RenderPass;
class IIndexBuffer;
class IVertexBuffer;

class UIRenderer :
    public Ibl::Mesh
{
  public:
    UIRenderer(Ibl::IDevice* device);
    virtual ~UIRenderer();

    static void                create(Ibl::IDevice* device);
    static UIRenderer*         renderer();
    Ibl::IDevice*              device();

    void                       setDrawIndexed(bool drawIndexed);
    void                       setVertexBuffer(IVertexBuffer* vertexBuffer);
    void                       setShader(const Ibl::IShader* vertexBuffer);

    void                       render(uint32_t count, uint32_t offset);

    virtual bool               render(const Ibl::RenderRequest* request,
                                      const Ibl::GpuTechnique* technique) const;

    // Get a vertex buffer to satisfy a given declaration.
    IVertexBuffer*             vertexBuffer(IVertexDeclaration* declaration);
    IIndexBuffer*              indexBuffer();

    void                       setViewProj(const Ibl::Matrix44f& ortho);

  protected:
    // Pipeline State.
    Ibl::IShader*              _currentShader;
    Ibl::IVertexBuffer*        _currentVertexBuffer;

    // Buffer offset state.
    uint32_t                   _vertexOffset;
    bool                       _drawIndexed;
    uint32_t                   _primitiveCount;

    // Buffers
    Ibl::IIndexBuffer*         _indexBuffer;
    std::map<IVertexDeclaration*, IVertexBuffer*> _vertexBuffers;

    Ibl::Matrix44f             _viewProj;

    static UIRenderer*         _uiRenderer;
};
}

#endif