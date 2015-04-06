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
#include <IblITexture.h>
#include <IblTextureImage.h>
#include <IblDDSCodec.h>

namespace Ibl
{

ITexture::ITexture (Ibl::IDevice* device) : 
           IRenderResource (device),
           _width (0),
           _height (0),
           _depth (1),
           _textureCount (0),
           _multiSampleCount (1),
           _multiSampleQuality (0),
           _activeSlice (-1),
           _inUse (true),
           _resource(nullptr),
           _channels (0),
           _pixelChannelPitch (0),
           _activeMipId(-1)
{
}

ITexture::~ITexture() 
{
    safedelete(_resource);
}

// Clear UAV, texture must be created with UAV support.
void
ITexture::clearUnorderedAccessViewUint(uint32_t value) const
{
}

bool
ITexture::inUse() const
{
    return _inUse;
}


const Ibl::TextureParameters* 
ITexture::resource() const
{
    return _resource;
}

void
ITexture::setInUse(bool value)
{
    _inUse = value;
}

const ISurface *
ITexture::surface(int32_t arrayId, int32_t mipId) const 
{ 
    return nullptr; 
};

unsigned int
ITexture::width() const 
{ 
    return _width; 
}

unsigned int
ITexture::height() const 
{ 
    return _height; 
}

unsigned int
ITexture::channels() const 
{ 
    return _channels; 
}

unsigned int
ITexture::depth() const 
{ 
    return _depth; 
}

PixelFormat
ITexture::format() const 
{ 
    return _format; 
}

unsigned int
ITexture::textureCount() const 
{ 
    return _textureCount; 
}

void
ITexture::setSize (const Ibl::Vector2i& size)  
{ 
    _width = size.x; _height = size.y; 
}

unsigned int
ITexture::pixelChannelPitch() const
{
    return _pixelChannelPitch;
}

unsigned int
ITexture::bytesPerPixel() const
{
    return _channels * _pixelChannelPitch;
}

unsigned int
ITexture::area() const
{ 
    return _width * _height;
}

size_t
ITexture::byteSize() const
{
    return area() * bytesPerPixel();
}

int
ITexture::multiSampleCount() const 
{ 
    return _multiSampleCount; 
}

int
ITexture::multiSampleQuality() const 
{ 
    return _multiSampleQuality; 
}

int
ITexture::activeSlice() const 
{
     return _activeSlice; 
}

void
ITexture::setActiveSlice(int32_t activeSlice) 
{
     _activeSlice = activeSlice; 
}

void
ITexture::setActiveMipId(int32_t activeMipId)
{
    _activeMipId = activeMipId;
}

int32_t
ITexture::activeMipId () const
{
    return _activeMipId;
}

}