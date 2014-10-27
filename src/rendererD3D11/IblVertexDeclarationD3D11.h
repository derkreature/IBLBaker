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

#ifndef INCLUDED_BB_VERTEX_DECLARATION
#define INCLUDED_BB_VERTEX_DECLARATION

#include <IblPlatform.h>
#include <IblIVertexDeclaration.h>
#include <IblRenderDeviceD3D11.h>

namespace Ibl
{
class VertexDeclarationD3D11 : public Ibl::IVertexDeclaration
{
  public:
    VertexDeclarationD3D11(Ibl::DeviceD3D11* device);
    virtual ~VertexDeclarationD3D11();

    virtual bool                initialize (const Ibl::VertexDeclarationParameters*);
    virtual bool                create ();
    virtual bool                free ();
    virtual bool                cache();
    virtual bool                bind() const;
    
    virtual uint32_t            vertexStreamCount() const;
    virtual uint32_t            vertexStride() const;

    //-------------------------------------------------
    // Returns true if the specified element is present
    //-------------------------------------------------
    bool                        isTypeOf (const Ibl::DeclarationUsage& usage, 
                                          const uint32_t& index);

    //---------------------------
    // Allocates a vertex element
    //---------------------------
    static D3D11_INPUT_ELEMENT_DESC createVertexElement (WORD stream, 
                                                         WORD offset, 
                                                         BYTE type, 
                                                         BYTE method, 
                                                         BYTE usage, 
                                                         BYTE usageIndex,
                                                         BYTE streamIndex);

    D3D11_INPUT_ELEMENT_DESC*     layout() const { return _vertexAttributes; }
    uint32_t                      elementCount() const { return _elementCount; }

    virtual const std::vector <Ibl::VertexElement> & getDeclaration() const;

  protected:
    //ID3D11VertexDeclaration* _vertexDeclaration;
    D3D11_INPUT_ELEMENT_DESC*         _vertexAttributes;
    uint32_t                          _elementCount;
    uint32_t                          _vertexStride;
    std::vector <Ibl::VertexElement>   _declaration;

  protected:
    ID3D11Device*               _direct3d;
    ID3D11DeviceContext *      _immediateCtx;
};
}

#endif