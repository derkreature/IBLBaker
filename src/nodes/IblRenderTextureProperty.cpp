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
#include <IblRenderTextureProperty.h>
#include <IblIRenderResourceParameters.h>
#include <IblITexture.h>

namespace Ibl
{
RenderTextureProperty::RenderTextureProperty(Ibl::Node* node, const std::string& name, bool useMips, bool autoGenMips) : 
Property (node, name),
_dimensionDependency(nullptr),
_widthDependency (nullptr),
_heightDependency (nullptr),
_depthDependency (nullptr),
_formatDependency (nullptr),
_deviceDependency (nullptr),
_textureProperty(new Ibl::TextureProperty(this, std::string("renderTexture"), this)),
_texture (0),
_useMips(useMips),
_autoGenMips(autoGenMips)
{
    _textureProperty->set (nullptr);
    using std::placeholders::_1;
    _tasks.insert(std::make_pair(_textureProperty, std::bind(&RenderTextureProperty::computeTexture, this, _1)));
}

RenderTextureProperty::~RenderTextureProperty()
{
    safedelete (_texture);
}

void
RenderTextureProperty::computeTexture(const Property* property) const
{
    safedelete(_texture);

    int32_t slices = 1;
    if (_depthDependency)
    {
        slices = _depthDependency->get();
    }

    int32_t numMips = 1;
    if (_useMips)
    {
        uint32_t mipResolution = (uint32_t)(_widthDependency->get());
        do
        {
            numMips++;
            mipResolution = mipResolution >> 1;
        }
        while (mipResolution > 1);
    }

    TextureParameters textureResource =
        TextureParameters(std::string("RTTProperty"),
                          (Ibl::TextureDimension)(_dimensionDependency->get()),
                          Ibl::RenderTarget,
                          (Ibl::PixelFormat)_formatDependency->get(),
                           Ibl::Vector3i(_widthDependency->get(), _heightDependency->get(), slices),
                           _autoGenMips, 
                           slices, 
                           1, 
                           0, 
                           numMips);

    if (_texture = _deviceDependency->get()->createTexture(&textureResource))
    {
        _textureProperty->set(_texture);
        return;
    }
}

void
RenderTextureProperty::removeDependency(Property* p, size_t dependencyId)
{
    switch ((RenderTextureProperty::DependencyId)dependencyId)
    {
      case RenderTextureProperty::Dimension:
          _dimensionDependency = nullptr;
          break;
      case RenderTextureProperty::Width:
          _widthDependency = nullptr;
          break;
      case RenderTextureProperty::Height:
          _heightDependency = nullptr;
          break;
      case RenderTextureProperty::Depth:
          _depthDependency = nullptr;
          break;
      case RenderTextureProperty::Device:
          _deviceDependency = nullptr;
          break;
      case RenderTextureProperty::Format:
          _formatDependency = nullptr;
          break;
    }

    removeDependency(p, dependencyId);
}

void
RenderTextureProperty::addDependency (Property* p, size_t dependencyId)
{
    switch ((RenderTextureProperty::DependencyId)dependencyId)
    {
      case RenderTextureProperty::Dimension:
          _dimensionDependency = dynamic_cast<IntProperty*>(p);
           break;
      case RenderTextureProperty::Width:
          _widthDependency = dynamic_cast<IntProperty*>(p);
          break;
      case RenderTextureProperty::Height:
          _heightDependency = dynamic_cast<IntProperty*>(p);
          break;
      case RenderTextureProperty::Depth:
          _depthDependency = dynamic_cast<IntProperty*>(p);
          break;
      case RenderTextureProperty::Device:
          _deviceDependency = dynamic_cast<DeviceProperty*>(p);
          break;
      case RenderTextureProperty::Format:
          _formatDependency = dynamic_cast<PixelFormatProperty*>(p);
          break;
    }

    Property::addDependency(p, dependencyId);
}

const TextureProperty*
RenderTextureProperty::renderTextureProperty() const
{
    return _textureProperty;
}

TextureProperty*
RenderTextureProperty::renderTextureProperty()
{
    return _textureProperty;
}

const ITexture*  
RenderTextureProperty::renderTexture() const
{
    return _textureProperty->get();
}

ITexture*
RenderTextureProperty::renderTexture()
{
    return _textureProperty->get();
}

}