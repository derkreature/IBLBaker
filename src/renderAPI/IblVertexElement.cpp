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

#include <IblVertexElement.h>

namespace Ibl
{
VertexElement::VertexElement()
{

}

VertexElement::VertexElement (WORD streamVal, 
                              WORD offsetVal, 
                              BYTE typeVal, 
                              BYTE methodVal, 
                              BYTE usageVal, 
                              BYTE usageIndexVal,
                              BYTE streamIndexVal)
{
    _stream        = streamVal;
    _offset        = offsetVal;
    _type        = typeVal;
    _method        = methodVal;
    _usage      = usageVal;
    _usageIndex = usageIndexVal;
    _streamIndex = streamIndexVal;
}

VertexElement::VertexElement (const VertexElement& element)
{
    _stream = element._stream;
    _offset = element._offset;
    _type   = element._type;
    _method = element._method;
    _usageIndex = element._usageIndex;
    _usage  = element._usage;
    _streamIndex = element._streamIndex;
}

WORD
VertexElement::stream() const
{
    return _stream;
}

WORD
VertexElement::offset() const
{
    return _offset;
}

BYTE
VertexElement::type() const
{
    return _type;
}

BYTE
VertexElement::method() const
{
    return _method;
}

BYTE
VertexElement::usage() const
{
    return _usage;
}

BYTE
VertexElement::usageIndex() const
{
    return _usageIndex;
}


BYTE
VertexElement::streamIndex() const
{
    return _streamIndex;
}

bool
VertexElement::operator == (const VertexElement &src) const
{
    if (_usageIndex != src.usageIndex())
        return false;
    if (_usage != src.usage())
        return false;
    if (_method != src.method())
        return false;
    if (_type != src.type())
        return false;
    if (_offset != src.offset())
        return false;
    if (_stream != src.stream())
        return false;

    return true;
}

bool
VertexElement::operator != (const VertexElement &src) const
{
    if (_usageIndex != src.usageIndex())
        return true;
    if (_usage != src.usage())
        return true;
    if (_method != src.method())
        return true;
    if (_type != src.type())
        return true;
    if (_offset != src.offset())
        return true;
    if (_stream != src.stream())
        return true;

    return false;
}

}