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

#ifndef INCLUDED_BB_SHADER_MGR
#define INCLUDED_BB_SHADER_MGR

#include <IblPlatform.h>
#include <IblHash.h>

namespace Ibl
{
class Entity;
class Material;
class IShader;
class IComputeShader;
class GpuTechnique;
class IDevice;
class FileChangeWatcher;
class Mesh;

class ShaderMgr
{
  public:
    typedef std::map <Ibl::Hash, IShader*>  ShaderList;
    typedef const ShaderList                ConstShaderList;

    typedef std::map <Ibl::Hash, IComputeShader*> ComputeShaderList;
    typedef const ComputeShaderList         ConstComputeShaderList;

    ShaderMgr(Ibl::IDevice* device);
    virtual ~ShaderMgr();

    virtual bool                free();
    virtual bool                create();

    void                        update();

    //--------------------------------
    // Create a shader from a filename
    //--------------------------------
    bool                        addShader (const std::string& filename, 
                                           bool allowDeprecated = true,
                                           bool verbose = false);

    bool                        addComputeShaderFromFile  (const std::string& filename,
                                                           const std::string& includeFileName,
                                                           const std::string& functionName,
                                                           const Ibl::IComputeShader*& shaderOut,
                                                           const std::map<std::string, std::string>& defines);

    //----------------------------------------------------
    // Creates a shader and returns the shader 
    // Checks that the shader does not already exist first
    //----------------------------------------------------
    bool                        addShader(const std::string& filename, 
                                          const Ibl::IShader*& shader,  
                                          bool allowDeprecated = true,
                                          bool verbose = false);

    bool                        addShader(const std::string& filename, 
                                          const Ibl::IShader*& shader,  
                                          bool allowDeprecated,
                                          bool verbose,
                                          const std::map<std::string, std::string>& defines);

    bool                        addShader(const std::string& filename,
                                          const std::string& includeFileName,
                                          const IShader*& shader,
                                          bool allowDeprecated,
                                          bool verbose);


    //-------------------------------------
    // Get a shader with the specified hash
    //-------------------------------------
    const IShader*              shader (const Ibl::Hash& shadername);

    //--------------------------------------------------
    // Resolves shader bindings for the specified entity
    //--------------------------------------------------
    bool                        resolveShaders (Ibl::Entity* entity);

    //------------------------------------------------
    // Resolves shader bindings for the specified mesh
    //------------------------------------------------
    bool                        resolveShaders(Ibl::Mesh* mesh);

    //------------------------------------------------------------------
    // Gets the shader and technique for the specified material
    //------------------------------------------------------------------
    bool                        getShaderAndTechnique (Material* material, 
                                                       const IShader*& shader, 
                                                       const GpuTechnique*& technique);

    //---------------------------------------
    // Finds a shader with the specified hash
    //---------------------------------------
    bool                        findShader(const Ibl::Hash& hash,
                                           const Ibl::IShader*& shader);


    //---------------------------------------
    // Gets the current shaders in the system
    //---------------------------------------
    ConstShaderList&           shaderList();

    //-----------------------------------------
    // Gets the number of shaders in the system
    //-----------------------------------------
    uint32_t                   shaderCount();

    //---------------------------------------
    // Gets the shader at the specified index
    //---------------------------------------
    const IShader*             getShader (const uint32_t& index);



    void                       remove(Entity* entity);

  protected:

    bool                       addShaderFromManifest(const std::string& shaderName);

  private:
    Ibl::IDevice*              _deviceInterface;
    ComputeShaderList          _computeShaderList;
    ShaderList                 _shaderList;

    std::map<std::string, std::string> _shaderNameManifest;
    std::unique_ptr<FileChangeWatcher> _fileChangeWatcher;
    std::set<Mesh*>                    _trackedMeshes;
};
}

#endif