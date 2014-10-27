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
#include <IblShaderMgr.h>
#include <IblMaterial.h>
#include <IblMesh.h>
#include <IblIShader.h>
#include <IblIComputeShader.h>
#include <IblPostEffect.h>
#include <IblLog.h>
#include <IblAssetManager.h>
#include <IblEntity.h>
#include <IblFileChangeWatcher.h>
#include <direct.h>

namespace Ibl
{

ShaderMgr::ShaderMgr(Ibl::IDevice* device) : 
    _deviceInterface(device)
{
    _fileChangeWatcher.reset(new FileChangeWatcher());

    // Load the user shader manifest
    std::string filename = "data/ShaderManifest.xml";
    std::unique_ptr<typename pugi::xml_document> doc = 
        std::unique_ptr<typename pugi::xml_document>(Ibl::AssetManager::assetManager()->openXmlDocument(filename));

    if (!doc)
    {
        LOG ("Could not locate application shader manifest");
    }
    else
    {
        LOG ("Loaded manifest file " << filename);
        pugi::xpath_node_set shaderSet = doc->select_nodes("/Shaders/Shader");

        for (auto nodeIt = shaderSet.begin(); nodeIt != shaderSet.end(); ++nodeIt)
        {
            auto xpathNode = *nodeIt;
            if (xpathNode)
            {
                // Load default user shaders.
                std::string name;
                std::string shaderFilePathName;
                if (const char* xpathValue = xpathNode.node().attribute("ShaderName").value())
                {
                    name = std::string(xpathValue);
                }
                if (const char* xpathValue = xpathNode.node().attribute("ShaderPath").value())
                {
                    shaderFilePathName = std::string(xpathValue);
                }

                // Initialize the shader name cache.
                _shaderNameManifest.insert (std::make_pair(name, shaderFilePathName));
                addShaderFromManifest(shaderFilePathName);
            }
        }
    }
}

ShaderMgr::~ShaderMgr()
{
    for(auto iterator = _shaderList.begin(); 
         iterator != _shaderList.end(); ++iterator)
    {
       Ibl::IShader * shader = iterator->second;
       delete shader;
    }

    for(auto iterator = _computeShaderList.begin(); 
        iterator != _computeShaderList.end(); ++iterator)
    {
       Ibl::IComputeShader * shader = iterator->second;
       delete shader;
    }

    _shaderList.clear();
    _computeShaderList.clear();
}


bool 
ShaderMgr::free()
{
    for (auto iterator = _shaderList.begin();
         iterator != _shaderList.end(); ++iterator)
    {
        (*iterator).second->free();
    }

    for (auto iterator = _computeShaderList.begin();
         iterator != _computeShaderList.end(); ++iterator)
    {
        iterator->second->free();
    }

    return true;
}

bool 
ShaderMgr::create()
{
    for (auto iterator = _shaderList.begin(); iterator != _shaderList.end(); ++iterator)
    {
        (*iterator).second->create();
    }
    return true;
}

bool 
ShaderMgr::addShader(const std::string& filename, 
                     bool allowDeprecated,
                     bool verbose)
{
    const std::string filePathName = "data/shadersD3D11/" + filename;
    LOG ("Creating shader " << filePathName.c_str());
    
    if (IShader* shader = _deviceInterface->createShader())
    {
        if (shader->initialize (filePathName, "", verbose, true))
        {
            LOG ("Created shader " << filePathName.c_str() << " and adding to shader mgr");
            _shaderList.insert(std::make_pair(Ibl::Hash(filePathName), shader));
            return true;
        }
        else
        {
            safedelete (shader);
        }
    }
    return false;
}

void
ShaderMgr::update()
{
    if (_fileChangeWatcher->hasChanges())
    {
        std::set<std::string> changed = _fileChangeWatcher->changeList();
        for (auto it = changed.begin(); it != changed.end(); it++)
        {
            LOG("Detected change: " << *it);
        }

        for (auto it = _shaderList.begin(); it != _shaderList.end(); it++)
        {
            LOG("Have shader " << (*it).second->filePathName());
            // Shader stores a relative path. We just want to rfind the path
            // For the moment to detect a modified shader.
            for (auto cit = changed.begin(); cit != changed.end(); cit++)
            {
                // Shader name
                if ((*cit).rfind((*it).second->filePathName()) != std::string::npos)
                {
                    LOG("Reloading " << (*it).second->filePathName());
                    (*it).second->free();
                    (*it).second->create();
                }
                // Include name
                else if ((*it).second->includeFilePathName().length() > 0)
                { 
                    if ((*cit).rfind((*it).second->includeFilePathName()))
                    {
                        (*it).second->free();
                        (*it).second->create();
                    }
                }
            }
        }

        for (auto it = _computeShaderList.begin(); it != _computeShaderList.end(); it++)
        {
            LOG("Have compute shader " << it->second->filePathName());
            // ComputeShader stores a relative path. We just want to rfind the path
            // For the moment to detect a modified shader.
            for (auto cit = changed.begin(); cit != changed.end(); cit++)
            {
                if ((*cit).rfind(it->second->filePathName()) != std::string::npos ||
                    (*cit).rfind((it)->second->includePathName()) != std::string::npos)
                {
                    LOG("Reloading " << it->second->filePathName());
                    it->second->free();
                    it->second->create();
                }
                else if ((it)->second->includePathName().length() > 0)
                {
                    if ((*cit).rfind((it)->second->includePathName()) != std::string::npos)
                    {
                        LOG("Reloading " << it->second->filePathName());
                        it->second->free();
                        it->second->create();
                    }
                }
            }
        }

        for (auto it = _trackedMeshes.begin(); it != _trackedMeshes.end(); it++)
        {
            resolveShaders(*it);
        }
    }
}

bool
ShaderMgr::addComputeShaderFromFile (const std::string& filename,
                                     const std::string& includeFilename,
                                     const std::string& functionName,
                                     const Ibl::IComputeShader*& shaderOut,
                                     const std::map<std::string, std::string>& defines)
{
    shaderOut = nullptr;

    const std::string filePathName = "data/shadersD3D11/" + filename;
    LOG ("Creating compute shader " << filePathName.c_str());
    std::string includeFilePathName;
    if (includeFilename.length() > 0)
    {
        includeFilePathName = "data/shadersD3D11/" + includeFilename;
    }

    std::ostringstream stream;
    stream << filename << " " << includeFilename << functionName;

    auto it = _computeShaderList.find(stream.str());

    if (it != _computeShaderList.end())
    {
        shaderOut = it->second;
        return true;
    }
        
    if (IComputeShader* shader = _deviceInterface->createComputeShader())
    {
        if (shader->initializeFromFile (filePathName, includeFilePathName, functionName, defines))
        {
            LOG ("Created compute shader " << filePathName.c_str() << " and adding to shader mgr");
            
            _computeShaderList.insert(std::make_pair(stream.str(), shader));            
            shaderOut = shader;
            return true;
        }
        else
        {
            safedelete (shader);
        }
    }
    return false;
}

ShaderMgr::ConstShaderList& 
ShaderMgr::shaderList()
{
    return _shaderList;
}

uint32_t 
ShaderMgr::shaderCount()
{
    return (uint32_t)_shaderList.size();
}

const IShader* 
ShaderMgr::getShader(const uint32_t& i)
{
    uint32_t index = 0;
    for (auto shaderit = _shaderList.begin(); 
        shaderit != _shaderList.end(); 
        shaderit++)
    {
        if(index == i)
        {
            return (*shaderit).second;
        }
        index++;
    }
    return 0;
}

bool 
ShaderMgr::findShader (const Ibl::Hash& hash, 
                       const Ibl::IShader*& shader)
{

    auto shaderIt = _shaderList.find(hash);
    if (shaderIt != _shaderList.end())
    {
        shader = shaderIt->second; 
        return true;
    }
    shader = nullptr;
    return false;
}

bool
ShaderMgr::addShader(const std::string& filename,
                     const std::string& includeFileName,
                     const IShader*& shader,
                     bool allowDeprecated,
                     bool verbose)
{
    shader = 0;
    const std::string filePathName = "data/shadersD3D11/" + filename;
    std::string includePathName;
    if (includeFileName.length() > 0)
    {
        includePathName = "data/shadersD3D11/" + includeFileName;
    }

    if (!findShader(Ibl::Hash(filePathName + includePathName), shader))
    {
        if (IShader* tshader = _deviceInterface->createShader())
        {
            if (tshader->initialize(filePathName, includePathName, verbose, true))
            {
                // LOG ("Shader created successfully from " << filePathName);
                // TODO: fix hash!                
                _shaderList.insert(std::make_pair(Ibl::Hash(filePathName+includeFileName), tshader));
                shader = tshader;
                return true;
            }
            else
            {
                LOG("Failed to create shader from " << filePathName);
                safedelete(tshader);
                return false;
            }
        }
    }
    return true;
}

bool 
ShaderMgr::addShader(const std::string& filename, 
                     const IShader*& shader, 
                     bool allowDeprecated,
                     bool verbose)
{
    shader = 0;
    const std::string filePathName = "data/shadersD3D11/"+ filename;

    if (!findShader(Ibl::Hash(filePathName), shader))
    {
        if (IShader* tshader = _deviceInterface->createShader())
        {
            if (tshader->initialize (filePathName, "", verbose, true))
            {
                // LOG ("Shader created successfully from " << filePathName);
                // TODO: Fix hash!
                _shaderList.insert(std::make_pair(Ibl::Hash(filePathName), tshader));

                shader = tshader;
                return true;
            }
            else
            {
                LOG ("Failed to create shader from " << filePathName);
                safedelete(tshader);
                return false;
            }
        }
    }
    return true;
}


bool 
ShaderMgr::addShader(const std::string& filename, 
                     const IShader*& shader, 
                     bool allowDeprecated,
                     bool verbose,
                     const std::map<std::string, std::string>& defines)
{
    shader = 0;
    
    std::ostringstream stream;

    stream << "data/shadersD3D11/" + filename;

    const std::string filePathName = "data/shadersD3D11/" + filename;

    for (auto it = defines.begin(); it != defines.end(); it++)
    {
        stream << (*it).first << (*it).second;
    }

    const std::string filePathKeyName = std::string(stream.str().c_str());

    if (!findShader(Ibl::Hash(filePathKeyName), shader))
    {
        if (IShader* tshader = _deviceInterface->createShader())
        {
            if (tshader->initialize (filePathName, "", verbose, true, defines))
            {
                // LOG ("Shader created successfully from " << filePathName);
                // TODO: Fix hash!

                _shaderList.insert(std::make_pair(Ibl::Hash(filePathKeyName), tshader));
                shader = tshader;
                return true;
            }
            else
            {
                LOG ("Failed to create shader from " << filePathName);
                safedelete(tshader);
                return false;
            }
        }
    }
    return true;
}

const IShader* 
ShaderMgr::shader(const Ibl::Hash& hash)
{
    auto shaderIt = _shaderList.find(hash);
    if (shaderIt != _shaderList.end())
    {
        return shaderIt->second;
    }
    return nullptr;
}

bool ShaderMgr::resolveShaders(Ibl::Entity* entity)
{
    size_t numMeshes = entity->numMeshes();
    for (size_t i = 0; i < numMeshes; i++)
    {
        if (Mesh* mesh = entity->mesh(i))
        {
            resolveShaders(mesh);
        }
    }
    return true;
}

bool ShaderMgr::resolveShaders(Ibl::Mesh* mesh)
{
    if (Material* material = mesh->material())
    {
        const IShader* shader = 0;
        const GpuTechnique* technique = 0;
    
        if (getShaderAndTechnique(material, shader, technique))
        {
            LOG("Found Shader and Technique for " << " " << material->name() <<
                " " << shader->name() << " " << mesh->name());
        }
        else
        {
            LOG("Failed to find Shader and Technique for " << mesh->name());
        }
    
        // This is dumb, but it is only for this demo.
        // I didn't port over my shader cache management code from Ktr.
        _trackedMeshes.insert(mesh);
    }
    return true;
}

bool
ShaderMgr::addShaderFromManifest(const std::string& shaderName)
{
    // Shadername, shader file path
    std::map<std::string, std::string>::const_iterator it =
        _shaderNameManifest.find(shaderName);
    if (it != _shaderNameManifest.end())
    {
        LOG ("Precompiling user shader " << shaderName);
        return addShader (it->second);
    }
    return false;
    
}

bool 
ShaderMgr::getShaderAndTechnique(Material* material, 
                                 const IShader*& shader, 
                                 const GpuTechnique*& technique)
{
    if (material)
    {
        bool foundShader = false;
        IShader* existingShader = 0;

        bool found = false;
        for (auto shaderIt = _shaderList.begin();
            shaderIt != _shaderList.end();
            shaderIt++)
        {
            if (shaderIt->second->name() == material->shaderName())
            {
                found = true;
            }
        }

        if (!found)
        {
            // If a shader cannot be found, load it from the user manifest.
            if (!addShaderFromManifest(material->shaderName()))
            {
                return false;
            }
        }

        for (auto shaderIt = _shaderList.begin(); 
             shaderIt != _shaderList.end(); 
             shaderIt++)
        {
            if (material->shaderName() == (*shaderIt).second->name())
            {
                if (shader = (*shaderIt).second)
                {
                    if (shader->getTechniqueByName (material->techniqueName(), 
                                                    technique))
                    {
                        {
                            material->setShader(shader);
                            material->setTechnique(technique);

                            LOG ("Resolved shader for material " << material->name() << 
                                " using technique " << material->techniqueName());
                            return true;
                        }
                    }
                }
            }
        }

        LOG ("Failed to resolve shader for material " << material->name() << 
            " using technique " << material->techniqueName());
    }
    else
    {
        LOG ("Material is not valid");
    }

    return false;
}


void
ShaderMgr::remove(Ibl::Entity* entity)
{
    const std::vector<Mesh*>& meshes = entity->meshes();
    for (auto meshIt = meshes.begin(); meshIt != meshes.end(); meshIt++)
    {
        auto trackedIt = _trackedMeshes.find(*meshIt);
        if (trackedIt != _trackedMeshes.end())
        {
            _trackedMeshes.erase(trackedIt);
        }
    }
}


}