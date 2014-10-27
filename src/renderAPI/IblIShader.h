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
#ifndef INCLUDED_BB_SHADER_INTERFACE
#define INCLUDED_BB_SHADER_INTERFACE

#include <IblIRenderResource.h>
#include <IblRenderRequest.h>
#include <IblHash.h>

namespace Ibl
{
class Mesh;
class Camera;
class PostEffect;
class GpuTechnique;
class GpuVariable;
class GpuConstantBuffer;
class IGpuBuffer;
class IEffect;

class
IShader : public IRenderResource
{
  public:
    IShader (Ibl::IDevice* device);
    virtual ~IShader();

    virtual bool                initialize (const std::string& file,
                                            const std::string& stream,
                                            bool verbose = false,
                                            bool allowDeprecated = true) = 0;

    virtual bool                initialize (const std::string& file,
                                            const std::string& stream,
                                            bool verbose,
                                            bool allowDeprecated,
                                            const std::map<std::string, std::string>& defines) = 0;

    //-----------------------------
    // Gets the specified technique
    //-----------------------------
    virtual bool                getTechniqueByName(const std::string& name, 
                                                   const GpuTechnique*& technique) const = 0;

    //-----------------------------
    // Gets the specified parameter
    //-----------------------------
    virtual bool                getParameterByName (const std::string& parameterName,
                                                    const GpuVariable*& coreVariable) const = 0;

    //-----------------------------------
    // Gets the specified constant buffer
    //-----------------------------------
    virtual bool                getConstantBufferByName(const std::string& constantBufferName,
                                                        const GpuConstantBuffer*&) const = 0;

    //---------------
    // Renders a mesh
    //---------------
    virtual bool                renderMesh (const RenderRequest& request) const = 0;

    virtual bool                renderMeshes (const RenderRequest& request,
                                              const std::set<const Mesh*>&) const = 0;

    virtual bool                renderInstancedBuffer (const RenderRequest& request,
                                                       const Ibl::IGpuBuffer* instanceBuffer) const = 0;

    virtual bool                renderMeshSubset (Ibl::PrimitiveType primitiveType,
                                                  size_t startIndex,
                                                  size_t numIndices,
                                                  const RenderRequest& request) const = 0;

    //-------------------------------------------
    // Passes variables to find shader parameters
    //-------------------------------------------
    virtual bool                passVariables() = 0;

    //--------------------------------------------
    // Sets up parameters for rendering for a mesh
    //--------------------------------------------
    virtual bool                setParameters (const RenderRequest& request) const = 0;

    //---------------------------------------------------
    // Gets the parameter type for the specified variable
    //---------------------------------------------------    
    virtual void                getParameterType (GpuVariable* param) = 0;

    //-----------------------------------------------------
    // Sets up parameters for rendering for a render target
    //-----------------------------------------------------
    virtual bool                setParameters (const PostEffect* target) const = 0;

    //-------------------------------------------
    // Gets the count of techniques in the shader
    //-------------------------------------------
    virtual uint32_t            techniqueCount() const = 0;

    //------------------------------------------
    // Gets the technique at the specified index
    //------------------------------------------
    virtual const GpuTechnique* getTechnique (uint32_t index) const = 0;
    
    virtual const IEffect*      effect () const = 0;

    const std::string&          shaderStream() const;
    const std::string&          filePathName() const;
    const std::string&          includeFilePathName() const;
    const std::string&          name() const;
    const std::string&          lastError() const;

    void                        setName(const char* shaderName);
    void                        setIncludeFilename(const char* shaderName);
    void                        setFilename (const char* shaderFileName); 
    void                        setShaderStream (const char* shaderStream);

    const Hash&                 hash() const;

  protected:
    Hash                        _hash;
  private:
    std::string                _shaderStream;
    std::string                _fileName;
    std::string                _includeFileName;
    std::string                _name;
};

}

#endif