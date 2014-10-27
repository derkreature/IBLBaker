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

/* This class is Based on work from:
   
   AngelCode Tool Box Library
   Copyright (c) 2007-2008 Andreas Jonsson
  
   This software is provided 'as-is', without any express or implied 
   warranty. In no event will the authors be held liable for any 
   damages arising from the use of this software.

   Permission is granted to anyone to use this software for any 
   purpose, including commercial applications, and to alter it and 
   redistribute it freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you 
      must not claim that you wrote the original software. If you use
      this software in a product, an acknowledgment in the product 
      documentation would be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and 
      must not be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source 
      distribution.
  
   Andreas Jonsson
   andreas@angelcode.com
*/

#ifndef INCLUDED_BB_FONT
#define INCLUDED_BB_FONT

#include <IblIRenderResource.h>
#include <IblColor.h>
#include <IblVector2.h>

namespace Ibl
{
class IVertexBuffer;
class IShader;
class ITexture;
class FontLoader;
class DynamicRenderer;
class Material;
class IShader;
class GpuTechnique;
class Camera;

enum FontTextEncoding
{
    NONE,
    UTF8,
    UTF16
};

struct CharDescriptor 
{
    CharDescriptor() : srcX(0), srcY(0), srcW(0), srcH(0), xOff(0), yOff(0), xAdv(0), page(0) {}

    short srcX;
    short srcY;
    short srcW;
    short srcH;
    short xOff;
    short yOff;
    short xAdv;
    short page;
    uint32_t chnl;

    std::vector<int> kerningPairs;
};

class Font : public IRenderResource
{
  public:
    Font (Ibl::IDevice* device);    
    virtual ~Font();

    virtual bool               create();
    virtual bool               free();
    virtual bool               cache();

    virtual bool               initialize (const std::string& family, float size);
    virtual bool               matches (const std::string& family, float size);
    bool                       render (const Ibl::Camera* camera,
                                       const std::string& text, 
                                       const Ibl::Vector2f& location,
                                       const Ibl::Vector4f& color);

    void                       setTextEncoding(FontTextEncoding encoding);

    float                      getTextWidth(const std::string& text, int32_t count);

    void                       write(const Ibl::Camera* camera, float x, float y, float z, 
                                     const std::string& text, int32_t count, uint32_t mode,
                                        const Ibl::Vector4f& color);

    void                       writeML(const Ibl::Camera* camera,
                                       float x, float y, float z, 
                                       const std::string& text, 
                                       int32_t count, uint32_t mode,
                                          const Ibl::Vector4f& color);

    void                       writeBox(const Ibl::Camera* camera,
                                        float x, float y, float z, 
                                        float width, const std::string& text, 
                                        int32_t count, unsigned mode,
                                           const Ibl::Vector4f& color);

    void                       setHeight(float h);
    float                      getHeight();

    float                      getBottomOffset();
    float                      getTopOffset();

    void                       prepareEffect();
    void                       preparePixelPerfectOutput();

  protected:
    friend class FontLoader;

    std::string                _family;
    float                      _size;
    Ibl::IShader *       _fontShader;
    const Ibl::ITexture *_texture;

    void                       internalWrite (const Ibl::Camera* camera,
                                              float x, float y, float z, 
                                              const char *text, int32_t count,
                                              const Ibl::Vector4f& color,
                                              float spacing = 0);

    float                      adjustForKerningPairs(int32_t first, int32_t second);

    CharDescriptor *           getChar(int32_t id);

    int32_t                    getTextLength(const std::string&);
    int32_t                    getTextChar(const std::string&, 
                                           int32_t pos, int32_t *nextPos = 0);
    int32_t                    findTextChar(const std::string& text, 
                                            int32_t start, int32_t length, int32_t ch);

    short                      _fontHeight; 
    short                       _base;       
    short                      _scaleW;
    short                      _scaleH;
    CharDescriptor             _defChar;
    bool                       _hasOutline;
    float                      _scale;

    FontTextEncoding           _encoding;

    std::map<int, CharDescriptor *> _chars;
    std::vector<Ibl::Material*> _pages;
    Ibl::IVertexBuffer *        _buffer;
    Ibl::DynamicRenderer *      _dynamicRenderer;

    const Ibl::IShader * _shader;
    const Ibl::GpuTechnique * _basicTechnique;
};

const int32_t FONT_ALIGN_LEFT    = 0;
const int32_t FONT_ALIGN_CENTER  = 1;
const int32_t FONT_ALIGN_RIGHT   = 2;
const int32_t FONT_ALIGN_JUSTIFY = 3;
}

#endif
