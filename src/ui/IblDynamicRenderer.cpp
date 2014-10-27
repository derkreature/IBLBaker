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

#include <IblDynamicRenderer.h>
#include <IblIVertexDeclaration.h>
#include <IblVertexElement.h>
#include <IblVertexDeclarationMgr.h>
#include <IblMaterial.h>
#include <IblIShader.h>
#include <IblScene.h>

namespace Ibl
{
DynamicRenderer::DynamicRenderer(Ibl::IDevice* device) :
    Ibl::Mesh (device),
    _camera (nullptr),
    _scene(nullptr),
    _renderPass(nullptr)
{
    setDynamic (true);
    setPrimitiveType(Ibl::TriangleList);

    // Common multiple for triangles and vertices.
    _maxVertices = 600;
    _dynamicVertexCount = 0;
    _dynamicVertexStream = new DynamicVertexStream[600];
    memset(_dynamicVertexStream, 0, sizeof(DynamicVertexStream) * 600);
    _dynamicVertexStreamBackBuffer = 0;

    _color = Ibl::Vector4f(1, 0, 0, 1);

    initialize();

    rotationProperty()->set(Ibl::Vector3f(0,0,0));
    translationProperty()->set(Ibl::Vector3f(0,0,0));
    scaleProperty()->set(Ibl::Vector3f(1,1,1));

    setDynamicPrimitiveType(DynamicTriangleList);

}

DynamicRenderer::~DynamicRenderer()
{
    delete[] _dynamicVertexStream;
}

bool
DynamicRenderer::initialize ()
{
    bool result = false;
    std::vector<Ibl::VertexElement> vertexElements;
    vertexElements.push_back (Ibl::VertexElement (0,  0, Ibl::FLOAT3, Ibl::METHOD_DEFAULT,  Ibl::POSITION, 0));    
    vertexElements.push_back (Ibl::VertexElement (0, 12, Ibl::FLOAT2, Ibl::METHOD_DEFAULT,  Ibl::TEXCOORD, 0));
    vertexElements.push_back (Ibl::VertexElement (0, 20, Ibl::FLOAT3, Ibl::METHOD_DEFAULT,  Ibl::TEXCOORD, 1));
    vertexElements.push_back (Ibl::VertexElement (0xFF,0,Ibl::UNUSED,0,0,0));
    
    Ibl::IVertexDeclaration* vertexDeclaration = 0;
    if (vertexDeclaration = 
        Ibl::VertexDeclarationMgr::vertexDeclarationMgr()->createVertexDeclaration
        (&Ibl::VertexDeclarationParameters (vertexElements)))
    {
        setVertexDeclaration (vertexDeclaration);        
        setPrimitiveType (Ibl::TriangleList);
        setVertexCount (_maxVertices); //  * 2 * 3
        setPrimitiveCount (0);
        result = create();
    }

    if (!result)
    {
        safedelete (vertexDeclaration);
    }
    return result;
}

void
DynamicRenderer::setRenderState(Ibl::Material* material,
                                const Ibl::Camera* camera,
                                const Ibl::Scene* scene,
                                const Ibl::RenderPass * renderPass)
{
    setMaterial(material);
    _camera = camera;
    _scene = scene;
    _renderPass = renderPass;
}

void
DynamicRenderer::drawTriangle (const Vector3f& a, const Vector3f& b, const Vector3f& c,
                               const Vector2f& uvA, const Vector2f& uvB, const Vector2f& uvC)
{
    DynamicRenderer::begin();

    // A (left bottom)
    vertex3f(a, uvA);

    // B (Right Bottom)
    vertex3f(b, uvB);

    // C (Right, top)
    vertex3f(c, uvC);

    DynamicRenderer::end();
}


void
DynamicRenderer::drawQuad (const Vector3f& a, const Vector3f& b, const Vector3f& c, const Vector3f& d,
                           const Vector2f& uvA, const Vector2f& uvB, const Vector2f& uvC, const Vector2f& uvD)
{

    DynamicRenderer::begin();

    // A (left bottom)
    vertex3f(a, uvA);

    // B (Right Bottom)
    vertex3f(b, uvB);

    // C (Right, top)
    vertex3f(c, uvC);


    // A (left bottom)
    vertex3f(a, uvA);

    // C (right top)
    vertex3f(c, uvC);

    // D (Left top)
    vertex3f(d, uvD);

    DynamicRenderer::end();
}

void
DynamicRenderer::setDynamicPrimitiveType(DynamicPrimitiveType primitiveType)
{
    if (primitiveType != _dynamicPrimitiveType)
    {
        DynamicRenderer::end();
    }

    setPrimitiveType(_dynamicPrimitiveType == DynamicTriangleList ? Ibl::TriangleList : Ibl::LineList);
    _dynamicPrimitiveType = primitiveType;
}

void
DynamicRenderer::begin()
{
    _dynamicVertexStreamBackBuffer = _dynamicVertexStream;
    _dynamicVertexCount = 0;
    _subVertexCount = 0;
}

void
DynamicRenderer::begin (Ibl::Material* material,
                        const Ibl::Camera* camera)
{
    if (camera != nullptr)
        _camera = camera;
    setMaterial(material);

    _dynamicVertexStreamBackBuffer = _dynamicVertexStream;
    _dynamicVertexCount = 0;
    _subVertexCount = 0;
}

void
DynamicRenderer::vertex3f(const Ibl::Vector3f& point,
                          const Ibl::Vector2f& tex,
                          const Ibl::Vector4f& color)
{
    if (_dynamicVertexCount < _maxVertices-1)
    {
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].x = point.x;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].y = point.y;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].z = point.z;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].tu = tex.x;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].tv = tex.y;

        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].r = color.x;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].g = color.y;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].b = color.z;
    }
    else
    {
        end();
        begin (material());

        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].x = point.x;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].y = point.y;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].z = point.z;

        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].tu = tex.x;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].tv = tex.y;

        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].r = color.x;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].g = color.y;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].b = color.z;
    }
    _dynamicVertexCount++;
}

void
DynamicRenderer::vertex3f(const Ibl::Vector3f& point,
                          const Ibl::Vector2f& tex)
{
    if (_dynamicVertexCount < _maxVertices-1)
    {
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].x = point.x;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].y = point.y;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].z = point.z;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].tu = tex.x;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].tv = tex.y;
    }
    else
    {
        end();
        begin (material());
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].x = point.x;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].y = point.y;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].z = point.z;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].tu = tex.x;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].tv = tex.y;
    }
    _dynamicVertexCount++;
}

void
DynamicRenderer::vertex3f(const Ibl::Vector3f& point)
{
    if (_dynamicVertexCount < _maxVertices-1)
    {
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].x = point.x;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].y = point.y;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].z = point.z;
    }
    else
    {
        end();
        begin (material());
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].x = point.x;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].y = point.y;
        _dynamicVertexStreamBackBuffer[_dynamicVertexCount].z = point.z;
    }
    _dynamicVertexCount++;
}

void
DynamicRenderer::end()
{
    _dynamicVertexStreamBackBuffer = 0;

    if (_dynamicVertexCount > 0)
    {
        if (float * _internalBuffer = static_cast<float*>(lock()))
        {
            memcpy (&_internalBuffer[0], &_dynamicVertexStream[0], (sizeof (DynamicVertexStream)) * _dynamicVertexCount);
            unlock();

            uint32_t primitiveCount = _dynamicPrimitiveType == DynamicLineList ? (_dynamicVertexCount / 2)  : (_dynamicVertexCount / 3);
            setPrimitiveCount (primitiveCount);
            setPrimitiveType(_dynamicPrimitiveType == DynamicLineList ? Ibl::LineList : Ibl::TriangleList);
        }

        {
            // Draw
            //ASSERT (shader, "Shader is not valid!!!");
            const Ibl::IShader* shader = material()->shader();
            shader->renderMesh (Ibl::RenderRequest(material()->technique(), nullptr, _camera, this));
        }

        _dynamicVertexCount = 0;
    }
}

void DynamicRenderer::drawQuad(const Ibl::Vector3f& orig, 
                               float size, 
                               const Ibl::Vector4f& color,
                               const Ibl::Vector3f& axisU, 
                               const Ibl::Vector3f &axisV)
{

    material()->albedoColorProperty()->set(color);
    setDynamicPrimitiveType(DynamicTriangleList);

    Vector3f pts[4];
    pts[0] = orig;
    pts[1] = orig + (axisU * size);
    pts[2] = orig + (axisU + axisV)*size;
    pts[3] = orig + (axisV * size);

    // Draw filled.
    begin();
    vertex3f(pts[0]);
    vertex3f(pts[1]);
    vertex3f(pts[2]);
    
    vertex3f(pts[0]);
    vertex3f(pts[2]);
    vertex3f(pts[3]);
    end();

    // Draw Line
    material()->albedoColorProperty()->set(color);
    setDynamicPrimitiveType(DynamicLineList);
    begin();
    vertex3f(pts[0]);
    vertex3f(pts[1]);

    vertex3f(pts[1]);
    vertex3f(pts[2]);

    vertex3f(pts[2]);
    vertex3f(pts[3]);
    
    vertex3f(pts[3]);
    vertex3f(pts[0]);

    end();


}

void DynamicRenderer::drawTri(const Ibl::Vector3f& orig, 
                              float size, 
                              const Ibl::Vector4f& colour,
                              const Ibl::Vector3f& axisU, 
                              const Ibl::Vector3f& axisV)
{
    Vector3f pts[3];

    pts[0] = orig;
    pts[1] = (axisU );
    pts[2] = (axisV );

    pts[1]*=size;
    pts[2]*=size;
    pts[1]+=orig;
    pts[2]+=orig;

    material()->albedoColorProperty()->set(colour);
    setDynamicPrimitiveType(DynamicTriangleList);

    // Draw filled triangle.
    DynamicRenderer::begin();
    vertex3f(pts[0]);
    vertex3f(pts[1]);
    vertex3f(pts[2]);
    DynamicRenderer::end();

    // Draw edge outline
    setDynamicPrimitiveType(DynamicLineList);
    Ibl::Vector4f edgeColor = colour;
    edgeColor.w = 1.0f;
    material()->albedoColorProperty()->set(edgeColor);

    DynamicRenderer::begin();
    vertex3f(pts[0]);
    vertex3f(pts[1]);
    vertex3f(pts[2]);
    DynamicRenderer::end();
}

}
