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


#include <IblIndexedMesh.h>
#include <IblIDevice.h>
#include <IblIVertexDeclaration.h>
#include <IblIIndexBuffer.h>
#include <IblTransformProperty.h>
#include <IblVertexStream.h>
#include <IblLog.h>
#include <IblVertexDeclarationMgr.h>

#if IBL_USE_ASS_IMP_AND_FREEIMAGE
// Assimp includes
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>
#endif

namespace Ibl
{
IndexedMesh::IndexedMesh(Ibl::IDevice* device) : 
    StreamedMesh  (device),
    _indices (0),
    _indexBuffer (0),
    _indexBufferLocked (false)
{
    _indexCount = new IntProperty(this, std::string("intbuffer"));
    _indicesBufferAttr = new UIntPtrProperty(this, std::string ("indbuffer"));

    setPrimitiveType (Ibl::TriangleList);
    _indexCount->set (0);
    _indicesBufferAttr->set (nullptr);
}

IndexedMesh::~IndexedMesh()
{
    _device->destroyResource(_indexBuffer);
    
    if (_indices)
    {
        ::free(_indices);
    }
}

#if IBL_USE_ASS_IMP_AND_FREEIMAGE
bool
IndexedMesh::load(const aiMesh* inputMesh)
{
    size_t inputIndexCount = inputMesh->mNumFaces * 3;
    size_t inputVertexCount = inputMesh->mNumVertices;

    Vector3f* verticesPtr = new Vector3f[inputVertexCount];
    Vector3f* normalsPtr = new Vector3f[inputVertexCount];
    Vector2f* uvsPtr = new Vector2f[inputVertexCount];
    uint32_t* indiciesPtr = new uint32_t[inputIndexCount];

    // Copy the vertex buffer
    {
        Vector3f* vertexPtr = verticesPtr;
        Vector3f* normalPtr = normalsPtr;
        Vector2f* uvPtr = uvsPtr;

        memset(&vertexPtr[0], 0, sizeof(float)* 3 * inputVertexCount);
        memset(&normalPtr[0], 0, sizeof(float)* 3 * inputVertexCount);
        memset(&uvPtr[0], 0, sizeof(float)* 2 * inputVertexCount);

        if (inputMesh->HasPositions())
        {
            memcpy(&vertexPtr[0], inputMesh->mVertices, sizeof(float)* 3 * inputVertexCount);
        }
        if (inputMesh->HasNormals())
        {
            memcpy(&normalPtr[0], inputMesh->mNormals, sizeof(float)* 3 * inputVertexCount);
        }
        if (inputMesh->HasTextureCoords(0))
        {
            for (uint32_t uvId = 0; uvId < inputVertexCount; uvId++)
            {
                uvPtr[uvId].x = inputMesh->mTextureCoords[0][uvId].x;
                uvPtr[uvId].y = inputMesh->mTextureCoords[0][uvId].y;
            }
        }
    }

    // Copy the index buffer
    size_t triangleCount = inputMesh->mNumFaces;
    for (size_t triangleId = 0; triangleId < triangleCount; triangleId++)
    {
        for (size_t indexId = 0; indexId < 3; indexId++)
        {
            indiciesPtr[(triangleId * 3) + indexId] = inputMesh->mFaces[triangleId].mIndices[indexId];
        }
    }

    // Create mesh representation.
    uint32_t newPrimitiveCount = inputMesh->mNumFaces;

    // Initialize topology information
    setIndices(indiciesPtr,
        (uint32_t)(inputIndexCount),
        newPrimitiveCount);

    setVertexCount((uint32_t)(inputVertexCount));

    setPrimitiveType(Ibl::TriangleList);

    uint32_t offset = 0;
    // Setup Elements
    std::vector<Ibl::VertexElement> vertexElements;
    vertexElements.push_back(Ibl::VertexElement(0, 0, Ibl::FLOAT3, Ibl::METHOD_DEFAULT, Ibl::POSITION, 0));
    vertexElements.push_back(Ibl::VertexElement(0, 12, Ibl::FLOAT3, Ibl::METHOD_DEFAULT, Ibl::NORMAL, 0));
    vertexElements.push_back(Ibl::VertexElement(0, 24, Ibl::FLOAT2, Ibl::METHOD_DEFAULT, Ibl::TEXCOORD, 0));
    vertexElements.push_back(Ibl::VertexElement(0xFF, 0, Ibl::UNUSED, 0, 0, 0));

    Ibl::VertexDeclarationParameters resource = Ibl::VertexDeclarationParameters(vertexElements);

    if (Ibl::IVertexDeclaration* vertexDeclaration =
        Ibl::VertexDeclarationMgr::vertexDeclarationMgr()->createVertexDeclaration(&resource))
    {
        setVertexDeclaration(vertexDeclaration);

        Ibl::VertexStream* vertexStream =
            new Ibl::VertexStream(Ibl::POSITION, 0, 3,
            vertexCount(), (float*)verticesPtr);
        Ibl::VertexStream* normalStream =
            new Ibl::VertexStream(Ibl::NORMAL, 0, 3,
            vertexCount(), (float*)normalsPtr);
        Ibl::VertexStream* texCoordStream =
            new Ibl::VertexStream(Ibl::TEXCOORD, 0, 2,
            vertexCount(), (float*)uvsPtr);
        addStream(vertexStream);
        addStream(normalStream);
        addStream(texCoordStream);
    }

    // Nuke pointers
    delete[] verticesPtr;
    delete[] normalsPtr;
    delete[] uvsPtr;
    delete[] indiciesPtr;

    if (create())
    {
        if (cache())
        {
            return true;
        }
    }
    return false;
}
#else 
bool
IndexedMesh::load(const tinyobj::shape_t* shape)
{

    const tinyobj::mesh_t* inputMesh = &shape->mesh;

    size_t inputIndexCount = inputMesh->indices.size();
    size_t inputVertexCount = inputMesh->positions.size();



    if (inputIndexCount == 0)
        return false;
    if (inputVertexCount == 0)
        return false;

    float * verticesPtr = new float[inputVertexCount * 3];
    float * normalsPtr = new float[inputVertexCount * 3];
    float * uvsPtr = new float[inputVertexCount * 2];
    uint32_t* indiciesPtr = new uint32_t[inputIndexCount];

    // Copy the vertex buffer

    {
        memset(&verticesPtr[0], 0, sizeof(float) * 3 * inputVertexCount);
        memset(&normalsPtr[0], 0, sizeof(float)* 3 * inputVertexCount);
        memset(&uvsPtr[0], 0, sizeof(float)* 2 * inputVertexCount);

        {
            for (size_t positionId = 0; positionId < inputMesh->positions.size(); positionId++)
            {
                verticesPtr[positionId] = inputMesh->positions[positionId];
            }
        }
        {
            for (size_t normalsId = 0; normalsId < inputMesh->normals.size(); normalsId++)
            {
                normalsPtr[normalsId] = inputMesh->normals[normalsId];
            }
        }
        {
            for (uint32_t uvId = 0; uvId < inputMesh->texcoords.size(); uvId++)
            {
                uvsPtr[uvId] = inputMesh->texcoords[uvId];
                if (uvId%2)
                    uvsPtr[uvId] = 1.0f-uvsPtr[uvId];
            }
        }
    }

    // Copy the index buffer
    size_t newPrimitiveCount = inputMesh->indices.size() * 3;
    for (size_t indexId = 0; indexId < inputMesh->indices.size(); indexId++)
    {
        indiciesPtr[indexId] = (uint32_t)(inputMesh->indices[indexId]);
    }

    // Initialize topology information
    setIndices(indiciesPtr,
        (uint32_t)(inputIndexCount),
        (uint32_t)(newPrimitiveCount));

    setVertexCount((uint32_t)(inputVertexCount));

    setPrimitiveType(Ibl::TriangleList);

    uint32_t offset = 0;
    // Setup Elements
    std::vector<Ibl::VertexElement> vertexElements;
    vertexElements.push_back(Ibl::VertexElement(0, 0, Ibl::FLOAT3, Ibl::METHOD_DEFAULT, Ibl::POSITION, 0));
    vertexElements.push_back(Ibl::VertexElement(0, 12, Ibl::FLOAT3, Ibl::METHOD_DEFAULT, Ibl::NORMAL, 0));
    vertexElements.push_back(Ibl::VertexElement(0, 24, Ibl::FLOAT2, Ibl::METHOD_DEFAULT, Ibl::TEXCOORD, 0));
    vertexElements.push_back(Ibl::VertexElement(0xFF, 0, Ibl::UNUSED, 0, 0, 0));

    Ibl::VertexDeclarationParameters resource = Ibl::VertexDeclarationParameters(vertexElements);

    if (Ibl::IVertexDeclaration* vertexDeclaration =
        Ibl::VertexDeclarationMgr::vertexDeclarationMgr()->createVertexDeclaration(&resource))
    {
        setVertexDeclaration(vertexDeclaration);

        Ibl::VertexStream* vertexStream =
            new Ibl::VertexStream(Ibl::POSITION, 0, 3,
            vertexCount(), (float*)verticesPtr);
        Ibl::VertexStream* normalStream =
            new Ibl::VertexStream(Ibl::NORMAL, 0, 3,
            vertexCount(), (float*)normalsPtr);
        Ibl::VertexStream* texCoordStream =
            new Ibl::VertexStream(Ibl::TEXCOORD, 0, 2,
            vertexCount(), (float*)uvsPtr);
        addStream(vertexStream);
        addStream(normalStream);
        addStream(texCoordStream);
    }

    bool result = false;
    if (create())
    {
        if (cache())
        {
            result = true;;
        }
    }

    safedeletearray(verticesPtr);
    safedeletearray(normalsPtr);
    safedeletearray(uvsPtr);
    safedeletearray(indiciesPtr);

    return result;
}
#endif


uint32_t
IndexedMesh::indexCount() const
{
    return _indexCount->get();
}

bool
IndexedMesh::cache()
{
    if (StreamedMesh::cache())
    {
        return fillIndexBuffer();
    }
    return false;
}

bool
IndexedMesh::create()
{
    if (createIndexBuffer())
    {
        return StreamedMesh::create();
    }
    return false;
}

bool
IndexedMesh::free()
{
    if (_indices)
    {
        ::free(_indices);
    }

    safedelete(_indexBuffer);
    return StreamedMesh::free();
}

const IIndexBuffer*
IndexedMesh::indexBuffer() const
{
    return _indexBuffer;
}

void
IndexedMesh::setIndices (const uint32_t* indexBuffer, uint32_t indexCount, uint32_t faceCount)
{
    uint32_t lastIndexCount = _indexCount->get();
    _indexCount->set (indexCount);

    setPrimitiveCount (faceCount);
    indexCount = _indexCount->get();

    if (indexCount > lastIndexCount)
    {
        if (_indices)
            ::free(_indices);
        if (_indexBuffer)
            safedelete (_indexBuffer);

        _indices = (uint32_t*)malloc(sizeof(uint32_t) * indexCount);

        createIndexBuffer();
    }

    memcpy (_indices, indexBuffer, indexCount * sizeof(uint32_t));
    _indicesBufferAttr->set (_indices);
    
    if (_indexBuffer)
    {
        fillIndexBuffer();
    }
}

uint32_t*
IndexedMesh::indices() const
{
    return _indices;
}

bool  
IndexedMesh::fillIndexBuffer()
{ 
    
    if (uint32_t* indices =  static_cast<uint32_t*>(lockIndexBuffer()))
    {
        memcpy(indices, _indices, sizeof(uint32_t)*_indexCount->get());
        unlockIndexBuffer();
        return true;
    }
    return false;
}

bool
IndexedMesh::createIndexBuffer()
{
    if (!_indexBuffer)
    {
        IndexBufferParameters ibResource= IndexBufferParameters(sizeof(uint32_t)*_indexCount->get());
        if (_indexBuffer = _device->createIndexBuffer(&ibResource))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    return true;
}

uint32_t* 
IndexedMesh::lockIndexBuffer()
{ 
    return static_cast<uint32_t*>(_indexBuffer->lock());
};

void 
IndexedMesh::unlockIndexBuffer()
{ 
    if (_indexBuffer->unlock())
    {
        _indexBufferLocked = false;
    }
    else
    {
        THROW("Failed to unlock index buffer");
    }
};

bool 
IndexedMesh::render(const Ibl::RenderRequest* request,
                    const Ibl::GpuTechnique* technique) const
{
    {
        return _device->drawIndexedPrimitive (_vertexDeclaration, _indexBuffer, 
                                              vertexBuffer(), technique, (PrimitiveType)primitiveType(), 
                                              primitiveDrawCount(), vertexCount());
    }
    return true;
}

}

