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
#ifndef INCLUDED_BB_BASE_MESH
#define INCLUDED_BB_BASE_MESH

#include <IblPlatform.h>
#include <IblTransformNode.h>
#include <IblIDevice.h>
#include <IblTextureImage.h>
#include <IblRegion.h>

namespace Ibl
{
class Entity;
class IDevice;
class IVertexDeclaration;
class IVertexBuffer;
class GpuTechnique;
class VertexStream;
class Mesh;
class Camera;
class IGpuBuffer;
class Scene;
class RenderRequest;
class Material;

enum TopologySubtype
{
    Tri
};

//--------------------------------------------------------------------
//
// Mesh
//
// Create a mesh by first instantiating a VertexDelcaration.
// Attach each index and size that you want to assign elements for.
// Set streams for each stream that you want to be set on the mesh.
// Call createVertexBuffer.
// The size and stride is determined from the VertexDeclaration.
// Each buffer is filled using the definition of the VertexDeclaration
//
//--------------------------------------------------------------------

class Mesh : public Ibl::TransformNode
{
  public:
    Mesh (Ibl::IDevice*);
    virtual ~Mesh();

    virtual bool                    create();
    virtual bool                    free();

    virtual bool                    render(const Ibl::RenderRequest* request,
                                           const Ibl::GpuTechnique* technique) const;


    void                            setStreamOutCapable (bool streamOutCapable);
    bool                            streamOutCapable () const;

    void                            setVertexDeclaration (const IVertexDeclaration*);
    virtual const IVertexDeclaration* vertexDeclaration() const;

    void                            setVertexCount (uint32_t vertexCount);
    virtual uint32_t                vertexCount() const;

    void                            setPrimitiveCount (uint32_t primitiveCount);
    virtual uint32_t                primitiveCount() const;

    void                            setPrimitiveDrawCount (uint32_t primitiveDrawCount);
    uint32_t                        primitiveDrawCount() const;

    void*                           lock();
    bool                            unlock();

    void                            setPrimitiveType (Ibl::PrimitiveType);
    virtual Ibl::PrimitiveType      primitiveType () const;

    virtual const Ibl::Material*    material () const;
    virtual Ibl::Material*          material ();
    void                            setMaterial (Ibl::Material* material);

    const Ibl::Entity*               entity() const;
    Ibl::Entity*                     entity();

    void                            setEntity (Ibl::Entity* entity);

    uint32_t                        groupId() const;
    void                            setGroupId(uint32_t groupIdIn);

    bool                            locked() const;


    bool                            visible() const;
    void                            setVisible (bool visible);

    bool                            bindVBToStreamOut() const;

    virtual void*                   internalStreamPtr();
    long                            vertexBufferSize();

    bool                            dynamic() const;
    void                            setDynamic (bool value);


    virtual float                   patchCount () const { return 0;}

    TopologySubtype                 topologySubType() const { return _topologySubtype; }
    void                            setTopologySubType(TopologySubtype value)  { _topologySubtype = value; }

    virtual float                   patchPrimitiveSize() const {return 0;}; 
        
    void                            setShadowMask(uint32_t);
    uint32_t                        shadowMask() const;

  protected:
    const IVertexBuffer*            vertexBuffer() const;

    TopologySubtype                 _topologySubtype;
    const IVertexDeclaration*       _vertexDeclaration;

    bool                            _useResource;
    Ibl::Material*                   _material;

  private:
    Ibl::PrimitiveType               _primitiveType;
    IVertexBuffer*                  _vertexBuffer;
    uint32_t                        _vertexCount;
    uint32_t                        _primitiveCount;
    uint32_t                        _primitiveDrawCount;
    bool                            _locked;
    Ibl::Entity*                     _entity;
    Ibl::BoolProperty*               _visible;
    bool                            _dynamic;
    uint32_t                        _groupId;
};
}
#endif
