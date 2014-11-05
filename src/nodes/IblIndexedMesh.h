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

#ifndef INCLUDED_BB_INDEXED_MESH
#define INCLUDED_BB_INDEXED_MESH

#include <IblPlatform.h>
#include <IblStreamedMesh.h>

#if IBL_USE_ASS_IMP_AND_FREEIMAGE
// Assimp includes
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>
struct aiMesh;
#else
#include <tiny_obj_loader.h>
#endif

namespace Ibl
{
class IDevice;
class IIndexBuffer;


class IndexedMesh : public Ibl::StreamedMesh
{
  public:
    IndexedMesh(Ibl::IDevice*);
    virtual ~IndexedMesh();
    
    virtual bool               cache();
    virtual bool               create();
    virtual bool               free();

    virtual bool               render(const Ibl::RenderRequest* request,
                                      const Ibl::GpuTechnique* technique) const;
    bool                       initialize();

    void                       setIndices (const uint32_t* faces,
                                           uint32_t indexCout,
                                           uint32_t faceCount);    


    uint32_t*                  indices() const;
    uint32_t                   indexCount() const;

#if IBL_USE_ASS_IMP_AND_FREEIMAGE
    bool                       load(const aiMesh* mesh);
#else
    bool                       load(const tinyobj::shape_t* shape);
#endif

  protected:
    const IIndexBuffer*        indexBuffer() const;
    bool                       fillIndexBuffer();
    bool                       createIndexBuffer();
    uint32_t*                  lockIndexBuffer();
    void                       unlockIndexBuffer();

  protected:
    uint32_t*                  _indices;

  private:

    Ibl::IIndexBuffer*          _indexBuffer;
    bool                       _indexBufferLocked;
    IntProperty *              _indexCount;
    UIntPtrProperty *          _indicesBufferAttr;
};
}

#endif