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

#include <IblVertexStream.h>
#include <IblLog.h>

namespace Ibl
{
VertexStream::VertexStream (Ibl::DeclarationUsage usage,
                            uint32_t usageIndex,
                            uint32_t strideArg, 
                            uint32_t countArg, 
                            const float* streamSrc) :
    _usage (usage),
    _usageIndex (usageIndex),
    _stream (nullptr),
    _count (countArg),
    _stride (strideArg)
{
    setStream (_stride, _count, streamSrc);    
}

VertexStream::~VertexStream()
{
    _stride = 0;
    _count = 0;
    if (_stream)
    {
        free(_stream);
    }
}

void
VertexStream::optimize()
{
    if (_stream)
    {
        free(_stream);
        _stream = 0;
    }
}

uint32_t
VertexStream::id (const VertexStream& vertexStream) 
{ 
    uint32_t id = (1 << (vertexStream.usage() + 16));

    if (vertexStream.usageIndex() >= 0)
    {
        id |= (1 << ((vertexStream.usageIndex()+1)));
    }

    //LOG ("Vertex stream Id is " << id);
    return id;
} 

uint32_t
VertexStream::id (const VertexElement& element) 
{ 
    uint32_t id = (1 << (element.usage() + 16)); 
    if (element.usageIndex() >= 0)
    {
        id |= (1 << ((element.usageIndex()+1)));
    }
    
    //LOG ("Element Stream Id is " << id);
    return id;
}

void
VertexStream::setStream(uint32_t strideArg, 
                        uint32_t countArg,
                        const float* streamSrc)
{
    _stride = strideArg;    
    _count = countArg;

    if (size() > 0)
    {
        if (_stream)
            free (_stream);

        if (_stream = (float*)malloc(size() * sizeof(float)))
        {
            setStream (streamSrc);
        }
    }
}

void
VertexStream::setStream(const float* streamSrc)
{
    if (_stream)
    {        
        memcpy (_stream, streamSrc, bufferSizeInBytes());
    }
}

const float*
VertexStream::stream () const
{
    return _stream;
}

float*
VertexStream::stream()
{
    return _stream;
}

const uint32_t
VertexStream::bufferSizeInBytes() const
{
    return size() * sizeof (float);
}

const uint32_t 
VertexStream::stride () const
{
    return _stride;
}

const uint32_t
VertexStream::count () const
{
    return _count;
}

const uint32_t
VertexStream::size () const
{
    return _stride * _count;
}

const Ibl::DeclarationUsage
VertexStream::usage () const
{
    return _usage;
}

const uint32_t
VertexStream::usageIndex() const
{
    return _usageIndex;
}


}
