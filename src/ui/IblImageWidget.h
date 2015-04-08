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

#ifndef INCLUDED_IMAGE_WIDGET
#define INCLUDED_IMAGE_WIDGET

#include <IblRegion.h>

namespace Ibl
{
class IDevice;
class ScreenOrientedQuad;
class ITexture;
class IShader;
class Material;
class GpuTechnique;

class ImageWidget
{
  public: 
    ImageWidget(Ibl::IDevice* device,
                const std::string& texturePath,
                const Region2f& bounds);

    ImageWidget(Ibl::IDevice* device,
                const Ibl::ITexture* texture,
                const Region2f& bounds);

    virtual ~ImageWidget();

    virtual bool               cache();

    virtual HRESULT            init();
    virtual void               render (float elapsedTime);
    void                       setImage(const Ibl::ITexture* texture);

    void                       setBlendIn(float);

    void                       setVisible(bool);
    bool                       visible() const;

  protected:
    IDevice*                          _device;
    std::string                       _texturePath;
    Ibl::ScreenOrientedQuad*          _quad;
    const Ibl::ITexture*              _image;    
    Ibl::Material*                    _material;
    const Ibl::GpuTechnique*          _technique;
    const Ibl::IShader*               _shader;
    Region2f                          _bounds;
    bool                              _blendingIn;
    float                             _blendInTime;
    float                             _totalBlendInTime;
    bool                              _visible;
};
}
#endif