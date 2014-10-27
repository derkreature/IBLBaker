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
#include <IblAssetManager.h>
#include <IblDataStream.h>
#include <IblLog.h>
#include <sys/stat.h>

namespace Ibl
{

AssetManager::AssetManager()
{
}

AssetManager::~AssetManager()
{
}

bool
AssetManager::fileExists(const std::string& pathName)
{
    struct stat finfo;
    int statResult = stat(pathName.c_str(), &finfo);
    if (statResult == 0)
        return true;
    else 
        return false;
}

pugi::xml_document*
AssetManager::openXmlDocument(const std::string& resourcePathName)
{
    pugi::xml_document * xmlDocument = nullptr;

    std::unique_ptr<typename DataStream> stream = 
        std::unique_ptr<typename DataStream>(openStream(resourcePathName));
    if (stream)
    {
        xmlDocument = new pugi::xml_document();
        // Create intermediate buffer;
        char * buffer = (char*)malloc(sizeof(char) * stream->size()+1);
        memset(buffer, 0, stream->size()+1);
        if (buffer)
        {
            stream->readBytes(buffer, stream->size());
            if (xmlDocument->load(buffer) == false)
            {
                LOG ("Failed to load xml document from stream: " << resourcePathName)
                delete xmlDocument;
                xmlDocument = nullptr;
            }
            free(buffer);
        }
    }

    return xmlDocument;
}

AssetManager*
AssetManager::assetManager()
{
    static std::unique_ptr<AssetManager> _assetManager;
    if (!_assetManager)
    {
        _assetManager.reset(new AssetManager());
    }

    return _assetManager.get();
}


DataStream *
AssetManager::openStream (const std::string& streamPathName)
{
    DataStream* stream = nullptr;
    if (AssetManager::fileExists(streamPathName))
    {
        std::ios::openmode mode = std::ios::in | std::ios::binary;
        std::fstream* rwStream = new std::fstream();
        rwStream->open(streamPathName.c_str(), mode);
        // Should check ensure open succeeded, in case fail for some reason.
        if (rwStream->fail())
        {
            LOG ("Cannot open file: " << streamPathName);
        }
        else
        {
            rwStream->seekg (0, std::ios::end);
            size_t length = (size_t)(rwStream->tellg());
            rwStream->seekg (0, std::ios::beg);

            uint8_t* buffer = (uint8_t*)malloc (sizeof(uint8_t)*length);
            rwStream->read((char*)buffer, length);

            stream = new MemoryDataStream(streamPathName, buffer, length, true);
            rwStream->close();
        }
        delete rwStream;
        rwStream = nullptr;
    }

    if (!stream)
    {
        LOG ("ERROR could not open stream " << streamPathName);
    }

    return stream;
}


}
