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


#include <IblTextureMgr.h>
#include <IblIDevice.h>
#include <IblITexture.h>
#include <IblIRenderResourceParameters.h>
#include <IblLog.h>
#include <IblDDSCodec.h>
#include <IblFreeImageCodec.h>
#include <IblTextureImage.h>
#include <IblApplication.h>
#include <IblStringUtilities.h>
#include <direct.h>

namespace Ibl
{

TextureMgr::TextureMgr(const Ibl::Application* application,
                       Ibl::IDevice* device) :  
    _deviceInterface(device)
{
#if IBL_USE_ASS_IMP_AND_FREEIMAGE
    FreeImageCodec::startup();
#endif
    DDSCodec::startup();

}

TextureMgr::~TextureMgr()
{
    for (auto it = _textures.begin();
         it != _textures.end();
         it++)
    {
        ITexture* texture = it->second;
        _deviceInterface->destroyResource(texture);
    }

    for (auto it = _stagingTextures.begin();
         it != _stagingTextures.end();
         it++)
    {
        ITexture* texture = it->second;
        _deviceInterface->destroyResource(texture);
    }


#if IBL_USE_ASS_IMP_AND_FREEIMAGE
    FreeImageCodec::shutdown();
#endif
    DDSCodec::shutdown();

    _textures.erase (_textures.begin(), _textures.end());
}    

void
TextureMgr::update(float delta)
{
}

ITexture*
TextureMgr::loadTexture (const std::string& filename,
                         Ibl::PixelFormat format)
{
    if (filename.length() == 0)
        return nullptr;
    LOG ("Attempting to load texture " << filename);

    ITexture* texture = findTexture (filename);
    if (!texture)
    {
        std::vector<std::string>       filenames;
        filenames.push_back(filename);
        TextureParameters resource = TextureParameters(filename, Ibl::TwoD);

        if (texture = _deviceInterface->createTexture(&resource))
        {
            _textures.insert (std::make_pair(std::string(filename),
                              texture));
            LOG ("Loaded texture " << filename);
        }
        else
        {
            LOG ("Failed  " << filename);

        }
    }
    return texture;
}

ITexture*
TextureMgr::loadTextureSet (const std::string& key, 
                            const std::vector<std::string>      & filenames)
{
    ITexture* texture = findTexture (key);
    if (!texture)
    {
        TextureParameters resource = TextureParameters(filenames, Ibl::TwoD);
        if (texture = _deviceInterface->createTexture(&resource))
        {
            _textures.insert (std::make_pair(std::string(key),
                              texture));

            LOG ("Loaded texture array from: ")
            for (size_t i = 0;i < filenames.size(); i++)
            {
                LOG ("    " << filenames[i].c_str());
            }
        }
        else
        {
            LOG ("Failed to load texture array from: ")
            for (size_t i = 0;i < filenames.size(); i++)
            {
                LOG ("    " << filenames[i].c_str());
            }
        }
    }
    return texture;
}

ITexture*
TextureMgr::loadStagingTexture(const std::string& filename)
{
    if (filename.length() == 0)
        return nullptr;

    std::vector<std::string>       filenames;
    filenames.push_back(filename);

    TextureParameters resource = TextureParameters(filenames, Ibl::TwoD, Ibl::StagingFromFile);
    Ibl::ITexture* texture = nullptr;

    if (texture = _deviceInterface->createTexture(&resource))
    {
        _stagingTextures.insert (std::make_pair(std::string(filename),
                          texture));
    }
    return texture;
}

void
TextureMgr::recycle(const ITexture* texture)
{
    if (texture == nullptr)
        return;
    // Delete the texture. There is no reference counting right now, due to removal of
    // Kreature's product graph, so, uhhh, pray it is not in use elsewhere.
    for (auto it = _textures.begin(); it != _textures.end(); it++)
    {
        if (it->second == texture)
        {
            safedelete (it->second);
            _textures.erase(it);
            return;
        }
    }
}

const ITexture*               
TextureMgr::loadCubeTexture (const std::string& filename,
                             Ibl::PixelFormat format)
{
    if (filename.length() == 0)
        return nullptr;

    ITexture* texture = findTexture (filename);
    if (!texture)
    {
        TextureDimension dimension = CubeMap;
        std::vector<std::string>       filenames;
        filenames.push_back(filename);

        TextureParameters resource = TextureParameters(filenames, dimension);
        if (texture = _deviceInterface->createTexture(&resource))
        {
            _textures.insert (std::make_pair(std::string(filename), texture));        
            LOG ("Loaded cubemap texture " << filename);
        }
        else
        {
           LOG ("Failed to load cubemap texture " << filename);
        }
    }
    return texture;
}

const ITexture*
TextureMgr::loadThreeD (const std::string& filename,
                               Ibl::PixelFormat format)
{
    ITexture* texture = findTexture (filename);
    
    if (!texture)
    {
        // Load volume texture
        std::vector<std::string>       filenames;
        filenames.push_back(filename);

        Ibl::TextureParameters resource = 
            Ibl::TextureParameters (filenames, 
                Ibl::ThreeD, 
                Ibl::FromFile, 
                Ibl::PF_A8R8G8B8, 
                Ibl::Vector3i(0, 0, 0),
                true, // generate mips
                1 /* number of textures*/ );

        if (texture = _deviceInterface->createTexture(&resource))
        {
            _textures.insert (std::make_pair(std::string(filename), texture));  
            LOG ("Loaded volume texture " << filename);
        }
        else
        {
            LOG ("Failed to load volume texture " << filename);
        }
    }

    return texture;
}

const ITexture*
TextureMgr::createTexture (const std::string& name,
                           Ibl::PixelFormat format,
                           unsigned int width,
                           unsigned int height,
                           unsigned int depth)
{
    ITexture* texture = nullptr;
    LOG("This function does not have a body! " << __FILE__ << " " << __LINE__);
    assert(0);
    return texture;
}

ITexture*
TextureMgr::findTexture (const std::string& name)
{
    auto it = _textures.find(name);    
    if (it != _textures.end())
    {
        return it->second;
    }
    return 0;
}
}