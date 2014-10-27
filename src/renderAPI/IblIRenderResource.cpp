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
#include <IblIRenderResource.h>

namespace Ibl
{
IRenderResource::RenderResourceVector  IRenderResource::_resources;

IRenderResource::IRenderResource(Ibl::IDevice* device) : 
    _deviceInterface (device)
{
    IRenderResource::addResource (this);
}

IRenderResource::~IRenderResource()
{
    IRenderResource::removeResource (this);
}

bool
IRenderResource::recreateOnResize() 
{
     return false; 
}

bool 
IRenderResource::addResource(IRenderResource* resource)
{
    {
    #ifdef _DEBUG
        assert (resource);
    #endif
        _resources.push_back(resource);
    }
    return true;
}

bool 
IRenderResource::releaseResources()
{
    // We need to make a copy because resources can be deleted as a result
    // of a resource being deleted.
    std::vector<IRenderResource*> resourcesCopy = _resources;
    for (auto it = resourcesCopy.begin(); it != resourcesCopy.end(); it++)
    {
        if (std::find(_resources.begin(), _resources.end(), *it) != _resources.end())
        {
            delete *it;
        }
    }
    
    _resources.clear();

    return true;
}

bool
IRenderResource::beginResize()
{
    std::vector<IRenderResource*> resourcesCopy = _resources;
    for (auto it = resourcesCopy.begin(); it != resourcesCopy.end(); it++)
    {
        if (std::find(_resources.begin(), _resources.end(), *it) != _resources.end())
        {
            if ((*it)->recreateOnResize())
            {
                if(!(*it)->free())
                {
                    LOG("Failed to free an item during resource management " << typeid(**it).name() <<
                        " " << __FILE__ << " " << __LINE__);
                    assert(0);
                    return false;
                }
            }
        }
    }
    LOG ("Finished freeing for resize");

    return true;
}

bool
IRenderResource::endResize()
{
    LOG ("About to resize...\n");
    // Step 1. Re create all of the render resource data
    {
        std::vector<IRenderResource*> resourcesCopy = _resources;
        for (auto it = resourcesCopy.begin(); it != resourcesCopy.end(); it++)
        {
            if (std::find(_resources.begin(), _resources.end(), *it) != _resources.end())
            {
                if ((*it)->recreateOnResize())
                {
                    if(!(*it)->create())
                    {
                        LOG("Failed to create an item during resource management " << typeid(**it).name() << 
                            " " << __FILE__ << " " << __LINE__);
                        assert(0);
                        return false;
                    }
                }
            }
        }
    }
    LOG ("About to cache ... \n");
    {
        // Step 2. Now all data is set, it should be safe to now reinit all of the data.
        std::vector<IRenderResource*> resourcesCopy = _resources;
        for (auto it = resourcesCopy.begin(); it != resourcesCopy.end(); it++)
        {
            if (std::find(_resources.begin(), _resources.end(), *it) != _resources.end())
            {
                if ((*it)->recreateOnResize())
                {
                    //LOG ("Caching item ...\n" <<  typeid(**rit).name() );    
                    if(!(*it)->cache())
                    {
                        LOG ("Failed to cache an item during resource management...\n" <<  
                             typeid(**it).name() << " " << __FILE__ << " " << __LINE__);    
                        assert(0);
                        return false;
                    }
                }
            }
        }
    }
    LOG ("Finished endResize!...\n");
    return true;
}

bool 
IRenderResource::freeResources()
{
    {
        std::vector<IRenderResource*> resourcesCopy = _resources;
        for (auto it = resourcesCopy.begin(); it != resourcesCopy.end(); it++)
        {
            auto rit = std::find(_resources.begin(), _resources.end(), *it);
            if (rit != _resources.end())
            {
                if(!(*rit)->free())
                {
                    LOG ("Failed to free...\n" <<  typeid(**rit).name() );
                    assert(0);
                    return false;
                }
            }
        }
        LOG ("Finished freeing hardware resources");
        return true;
    }
}

bool 
IRenderResource::removeResource(IRenderResource* resource)
{
    auto it = std::find (_resources.begin(), _resources.end(), resource);
    if(it != _resources.end())
        _resources.erase(it);

    return true;
}

}