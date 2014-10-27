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

#ifndef INCLUDED_BB_DYNAMIC_RENDERER
#define INCLUDED_BB_DYNAMIC_RENDERER

#include <IblPlatform.h>
#include <IblMesh.h>
#include <IblGpuTechnique.h>
#include <IblColor.h>

namespace Ibl
{
class IShader;
class RenderPass;

enum DynamicPrimitiveType
{
    DynamicTriangleList,
    DynamicLineList
};

struct DynamicVertexStream
{
    float x, y, z;
    float tu, tv;
    float r, g, b;
};

class DynamicRenderer :
    public Ibl::Mesh
{
  public:
    DynamicRenderer(Ibl::IDevice* device);
    virtual ~DynamicRenderer();

    void                       setDynamicPrimitiveType(DynamicPrimitiveType);

    bool                       initialize ();

    void                       begin (Ibl::Material* material,
                                      const Ibl::Camera* camera = nullptr);

    void                       begin ();

    void                       vertex3f(const Ibl::Vector3f& point,
                                        const Ibl::Vector2f& tex,
                                        const Ibl::Vector4f& color);

    void                       vertex3f(const Ibl::Vector3f& point,
                                        const Ibl::Vector2f& tex);
    void                       vertex3f(const Ibl::Vector3f& point);

    void                       end();

    void                       setRenderState(Ibl::Material* material, 
                                              const Ibl::Camera* camera,
                                              const Ibl::Scene* scene,
                                              const Ibl::RenderPass * renderPass);

    void                       drawQuad (const Vector3f& a, const Vector3f& b, const Vector3f& c, const Vector3f& d,
                                         const Vector2f& uvA, const Vector2f& uvB, const Vector2f& uvC, const Vector2f& uvD);

    void                       drawTriangle (const Vector3f& a, const Vector3f& b, const Vector3f& c,
                                             const Vector2f& uvA, const Vector2f& uvB, const Vector2f& uvC);

    void                       drawQuad(const Ibl::Vector3f& orig, 
                                        float size, 
                                        const Ibl::Vector4f& colour,
                                        const Ibl::Vector3f& axisU, 
                                        const Ibl::Vector3f &axisV);

    void                       drawTri(const Ibl::Vector3f& orig, 
                                       float size,
                                       const Ibl::Vector4f& colour,
                                       const Ibl::Vector3f& axisU, 
                                       const Ibl::Vector3f& axisV);

  protected:
    DynamicPrimitiveType         _dynamicPrimitiveType;
    uint32_t                     _maxVertices;
    uint32_t                     _dynamicVertexCount;
    DynamicVertexStream *        _dynamicVertexStream;
    DynamicVertexStream *        _dynamicVertexStreamBackBuffer;
    uint32_t                     _subVertexCount;
    Ibl::Vector4f                 _color;
    const Ibl::Camera*            _camera;
    const Ibl::Scene*             _scene;
    const Ibl::RenderPass*        _renderPass;
};
}

#endif