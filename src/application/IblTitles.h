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

#ifndef INCLUDED_TITLES
#define INCLUDED_TITLES

#include <IblPostEffect.h>

namespace Ibl
{
class ScreenOrientedQuad;

class Title
{
  public:
    Title(Ibl::IDevice* device);
    virtual ~Title();

    bool                       create(const std::string& texturePathName,
                                      const Ibl::Region2f& bounds,
                                      const Ibl::Vector4f& blendTerms);

    void                       update (float);
    void                       render();

  private:
    Ibl::IDevice*                _device;
    const Ibl::ITexture*         _image;
    const Ibl::GpuTechnique*     _technique;
    const Ibl::IShader*          _shader;

    float                       _currentAlpha;
    Ibl::Vector4f                _blendTerms;
    std::string                 _texturePath;
    Ibl::ScreenOrientedQuad*     _quad;
    Ibl::Material*               _material;
    Region2f                    _bounds;
    float                       _currentTime;
};

//----------------
// Titles manager.
//----------------
class Titles : public Ibl::PostEffect
{
  public:
    Titles  (Ibl::IDevice* device);
    virtual ~Titles ();

    //----------------------------------------
    // Release device resources on lost device
    //----------------------------------------
    virtual bool                free();
    virtual bool                cache();

    //--------------------------------------------
    // Create the shader and all related resources
    //--------------------------------------------
    virtual bool                update (float elapsed);
    bool                        addTitle(Ibl::Title* title);
    bool                        isFinished() const;

    void                        setTitleLength(float);
    float                       titleLength() const;

    //---------------------------
    // Render the titles texture.
    //---------------------------
    void                         render();

    //------------------------
    // Render the post effect.
    //------------------------
    virtual bool                render (const Ibl::ITexture* surface, 
                                        const Ibl::Camera* camera);

  protected:
    std::vector<Ibl::Title*>    _titles;

    //-----------------------
    // Target render texture.
    //-----------------------
    Ibl::ITexture*              _titlesTexture;
    //---------------------------------
    // Total required length of titles.
    //---------------------------------
    float                      _titleLength;
    float                      _titlesTime;

    const Ibl::GpuTechnique *   _postProcessTechnique;
    const Ibl::GpuVariable *    _titlesTextureVariable;
    const Ibl::GpuVariable *    _backbufferTextureVariable;
    const Ibl::GpuVariable *    _titlesTimeVariable;
};

}

#endif