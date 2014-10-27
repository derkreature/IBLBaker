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

#include <IblVertexDeclarationMgr.h>
#include <IblIVertexDeclaration.h>
#include <IblIDevice.h>
#include <IblLog.h>

namespace Ibl
{
VertexDeclarationMgr* VertexDeclarationMgr::_declarationMgr = 0;

VertexDeclarationMgr::VertexDeclarationMgr (const Ibl::Application* application,
                                            Ibl::IDevice* device) :
    _deviceInterface(device)
{
    _declarationMgr = this;
    _currentVertexDeclaration = 0;
    _vertexDeclarations.reserve (100);
}

VertexDeclarationMgr::~VertexDeclarationMgr()
{
    for (auto it = _vertexDeclarations.begin(); it != _vertexDeclarations.end(); it++)
        _deviceInterface->destroyResource(*it);
}
    
VertexDeclarationMgr*  VertexDeclarationMgr::vertexDeclarationMgr()
{
    return _declarationMgr;
}

IVertexDeclaration*
VertexDeclarationMgr::findVertexDeclaration(Ibl::VertexDeclarationParameters* resource)
{
    const std::vector <VertexElement> & srcElements = resource->elements();
    for (auto it = _vertexDeclarations.begin(); it != _vertexDeclarations.end(); it++)
    {
        const std::vector <VertexElement> & dstElements =
            (*it)->getDeclaration();

        if (dstElements.size() != srcElements.size())
        {
            continue;
        }

        bool isSameDeclaration = true;

        for (size_t i = 0; i < dstElements.size(); i++)
        {
            if (dstElements[i] != srcElements[i])
            {
                isSameDeclaration = false;
                break;
            }
        }

        if (isSameDeclaration)
        {
            // Got correct decl.
            return (*it);
        }
    }
    return 0;
}

IVertexDeclaration*         
VertexDeclarationMgr::createVertexDeclaration(Ibl::VertexDeclarationParameters* resource)
{
    IVertexDeclaration* declaration;
    if (declaration = findVertexDeclaration (resource))
    {
        return declaration;
    }
    else
    {
        IVertexDeclaration* vertexDeclaration = 0;
        if (vertexDeclaration = _deviceInterface->createVertexDeclaration(resource))
        {
            _vertexDeclarations.push_back (vertexDeclaration);
            return vertexDeclaration;
        }
    }
    return 0;
}

bool
VertexDeclarationMgr::bindVertexDeclaration(const IVertexDeclaration* declaration)
{
    if (_currentVertexDeclaration != declaration)
    {
        _currentVertexDeclaration = declaration;
        if (_currentVertexDeclaration)
        {
            return _currentVertexDeclaration->bind();
        }
        else
        {
            return false;
        }
    }
    else
    {
        return true;
    }
}
}
