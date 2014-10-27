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
#include <IblStreamedMesh.h>
#include <IblIDevice.h>
#include <IblIVertexDeclaration.h>
#include <IblVertexStream.h>
#include <IblLog.h>

namespace Ibl
{
StreamedMesh::StreamedMesh(Ibl::IDevice* device) : 
    Mesh (device),
    _vertexBufferCpuMemory (nullptr)
{
    _positionStream = new VertexStreamProperty (this, std::string ("V"));
    _normalsStream = new VertexStreamProperty (this, std::string ("N"));
    _texCoordStream = new VertexStreamProperty (this, std::string ("UV"));

}

StreamedMesh::~StreamedMesh()
{
    if (_vertexBufferCpuMemory)
        ::free(_vertexBufferCpuMemory);

    for (auto it = _vertexStreams.begin(); it != _vertexStreams.end(); it++)
    {
        VertexStream* stream = it->second;
        safedelete (stream);
    }
    _vertexStreams.clear();
}

bool
StreamedMesh::cache()
{
    return true;
}

void
StreamedMesh::clearStreams()
{
    _vertexStreams.clear();
}

void*
StreamedMesh::internalStreamPtr()
{    
    if (vertexBufferSize() == 0)
    {
        LOG ("Error, internal vertex buffer size is 0");
        return 0;
    }

    if (!_vertexBufferCpuMemory)
    {
        _vertexBufferCpuMemory = (float*)malloc(sizeof(float)*vertexBufferSize());
    }

    // stash the buffer and put it in an array
    std::map <uint32_t, float*> streamMap;

    for (auto it = _vertexStreams.begin();
        it != _vertexStreams.end();
        it++)
    {
        streamMap.insert (std::make_pair(it->first, it->second->stream()));
    }

    const std::vector <VertexElement>& declaration = _vertexDeclaration->getDeclaration();
    float* vb = (float*)_vertexBufferCpuMemory;

    for (uint32_t i = 0; i < vertexCount(); i++)
    {    
        for (uint32_t j = 0; j < declaration.size()-1; j++)
        {
            VertexElement element = declaration[j];
            VertexStream* stream = 0;
            if (findStream (stream, element))
            {
                uint32_t streamId = VertexStream::id(*stream);
                auto streamIt =  streamMap.find(streamId);

                if (streamIt != streamMap.end() &&
                    streamIt->second)
                {
                    for (uint32_t k = 0; k < stream->stride(); k++)
                    {
                        *vb++ = streamIt->second[k];
                    }
                    streamIt->second += stream->stride();
                }
                else
                {
                    uint32_t stride = IVertexDeclaration::elementToSize(element.type());
                    for (uint32_t k = 0; k < stride; stride++)
                    {
                        *vb++ = 0;
                    }
                }
            }
            else
            {
                // can't find a stream for this element.
                // warn user, and 0 the buffer for the stride size
                uint32_t stride = IVertexDeclaration::elementToSize(element.type());
                for (uint32_t k = 0; k < stride; stride++)
                {
                    *vb++ = 0;
                }
            }
        }
    }

    return _vertexBufferCpuMemory;
}


bool
StreamedMesh::addStream (VertexStream* stream)
{
    _vertexStreams.insert (std::make_pair (VertexStream::id(*stream), stream));

    if (stream->usage() == Ibl::POSITION)
    {
        _positionStream->set (stream);
    }
    if (stream->usage() == Ibl::NORMAL)
    {
        _normalsStream->set (stream);
    }
    if (stream->usage() == Ibl::TEXCOORD)
    {
        _texCoordStream->set (stream);
    }

    return true;
}

const VertexStream*
StreamedMesh::stream(DeclarationUsage type, uint32_t index) const
{
    for (auto it = _vertexStreams.begin(); it != _vertexStreams.end(); it++)
    {
        VertexStream* stream = it->second;
        if (stream->usage() == type &&
            stream->usageIndex() == index)
        {
            return stream;
        }
    }
    return nullptr;
}

const VertexStream*
StreamedMesh::texCoordStream(uint32_t index) const
{
    for (auto it = _vertexStreams.begin(); it != _vertexStreams.end(); it++)
    {
        VertexStream* stream = it->second;
        if (stream->usage() == Ibl::TEXCOORD &&
            stream->usageIndex() == index)
        {
            return stream;
        }
    }
    return nullptr;
}

VertexStream*
StreamedMesh::positionStream()
{
    for (auto it = _vertexStreams.begin(); it != _vertexStreams.end(); it++)
    {
        VertexStream* stream = it->second;
        if (stream->usage() == Ibl::POSITION)
        {
            return stream;
        }
    }
    return nullptr;
}

const VertexStream*
StreamedMesh::positionStream() const
{
    for (auto it = _vertexStreams.begin(); it != _vertexStreams.end(); it++)
    {
        const VertexStream* stream = it->second;
        if (stream->usage() == Ibl::POSITION)
        {
            return stream;
        }
    }
    return nullptr;
}

bool
StreamedMesh::findStream (VertexStream*& stream, const VertexElement& element)
{
    stream = nullptr;
    auto it = _vertexStreams.find (VertexStream::id (element));    
    if (it != _vertexStreams.end())
    {
        stream = it->second;
        return true;
    }
    return false;
}


}