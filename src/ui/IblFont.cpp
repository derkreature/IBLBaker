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

/* Based on work from:
   
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

#include <IblFont.h>
#include <IblIShader.h>
#include <IblLog.h>
#include <IblIVertexBuffer.h>
#include <IblTextureMgr.h>
#include <IblMaterial.h>
#include <IblDynamicRenderer.h>
#include <IblShaderMgr.h>
#include <IblIShader.h>
#include <IblIEffect.h>
#include <IblGpuVariable.h>
#include <string>

namespace Ibl
{
class FontLoader
{
  public:
    FontLoader (FILE *f, Font *font, const char * fontFile);

    // Must be implemented by derived class
    virtual int32_t load() = 0; 

  protected:
    void loadPage(int32_t id, const char *pageFile, const char *fontFile);
    void setFontInfo(int32_t outlineThickness);
    void setCommonInfo(int32_t fontHeight, int32_t base, int32_t scaleW, int32_t scaleH, int32_t pages, bool isPacked);
    void addChar(int32_t id, int32_t x, int32_t y, int32_t w, int32_t h, int32_t xoffset, int32_t yoffset, int32_t xadvance, int32_t page, int32_t chnl);
    void addKerningPair(int32_t first, int32_t second, int32_t amount);

    FILE * f;
    Font *font;
    const char *fontFile;
    int32_t outlineThickness;
};

class FontLoaderTextFormat : public FontLoader
{
public:
    FontLoaderTextFormat(FILE *f, Font *font, const char *fontFile);
    int32_t load();

    int32_t skipWhiteSpace(std::string &str, int32_t start);
    int32_t findEndOfToken(std::string &str, int32_t start);

    void interpretInfo(std::string &str, int32_t start);
    void interpretCommon(std::string &str, int32_t start);
    void interpretChar(std::string &str, int32_t start);
    void interpretSpacing(std::string &str, int32_t start);
    void interpretKerning(std::string &str, int32_t start);
    void interpretPage(std::string &str, int32_t start, const char *fontFile);
};

class FontLoaderBinaryFormat : public FontLoader
{
public:
    FontLoaderBinaryFormat(FILE *f, Font *font, const char *fontFile);

    int32_t  load();

    void readInfoBlock(int32_t size);
    void readCommonBlock(int32_t size);
    void readPagesBlock(int32_t size);
    void readCharsBlock(int32_t size);
    void readKerningPairsBlock(int32_t size);
};

Font::Font (Ibl::IDevice* device) : 
    IRenderResource (device)
{
    _fontShader = 0;
    _fontHeight = 0;
    _buffer = 0;
    _base = 0;
    _scaleW = 0;
    _scaleH = 0;
    _scale = 1.0f;
    _hasOutline = false;
    _encoding = NONE;
    
    if (!_deviceInterface->shaderMgr()->addShader ("IblSpriteShader.fx", _shader))
    {
        LOG ("Could not create sprite shader");
    }

    if (!_shader->getTechniqueByName ("basic", _basicTechnique))
    {
        LOG ("Could not find sprite technique basic");
    }

    _dynamicRenderer = new Ibl::DynamicRenderer(device);
}

Font::~Font()
{
    auto it = _chars.begin();
    while( it != _chars.end() )
    {
        delete it->second;
        it++;
    }
    for (auto it = _pages.begin(); it != _pages.end(); it++)
    {
        delete *it;
    }
    _pages.erase(_pages.begin(), _pages.end());

    safedelete(_dynamicRenderer);
}

bool
Font::initialize (const std::string& family, float size)
{
    _family = family;
    _size = size;
    // Work out the font that we need and load using "init"
    

    // matt - bug.
    std::string fontFile = "data/fonts/Arial.fnt";
    // Load the font
    FILE *f = fopen(fontFile.c_str(), "rb");
    bool result = false;

    // Determine format by reading the first bytes of the file
    char str[4] = {0};
    fread(str, 3, 1, f);
    fseek(f, 0, SEEK_SET);

    FontLoader *loader = 0;
    if( strcmp(str, "BMF") == 0 )
        loader = new FontLoaderBinaryFormat(f, this, fontFile.c_str());
    else
        loader = new FontLoaderTextFormat(f, this, fontFile.c_str());

    if (loader->load())
        result = true;

    setHeight (_size);

    delete loader;

    return true;
}

void 
Font::setTextEncoding(FontTextEncoding encoding)
{
    _encoding = encoding;
}

CharDescriptor * 
Font::getChar(int32_t id)
{
    auto it = _chars.find(id);
    if (it == _chars.end()) 
        return 0;
    return it->second;
}

bool
Font::create()
{
    return true;
}

bool
Font::free()
{
    return true;
}

bool
Font::cache()
{
    return true;
}

bool
Font::matches (const std::string& family, float size)
{
    return family == _family && size == _size;
}

bool
Font::render (const Ibl::Camera* camera,
              const std::string& text,
              const Ibl::Vector2f& location,
              const Ibl::Vector4f& color)
{
    Ibl::Viewport viewport;
    _deviceInterface->getViewport(&viewport);

    float x = location.x;//(-viewport._width * 0.5f) + location.x; // Rescale over.
    float y = viewport._height - location.y - (_fontHeight * _scale);//((viewport._height - location.y) + (-viewport._height * 0.5f)) - (_size);

    write (camera, x, y, 0.0, text, -1, FONT_ALIGN_LEFT, color);

    return true;
}

// Internal
float Font::adjustForKerningPairs(int32_t first, int32_t second)
{    
    CharDescriptor *ch = getChar(first);
    if (ch == 0) 
        return 0;
    for (UINT n = 0; n < ch->kerningPairs.size(); n += 2)
    {
        if (ch->kerningPairs[n] == second)
            return ch->kerningPairs[n+1] * _scale;
    }

    return 0;
}

float Font::getTextWidth(const std::string& text, int32_t count)
{
    if( count <= 0 )
        count = getTextLength(text);

    float x = 0;

    for( int32_t n = 0; n < count; )
    {
        int32_t charId = getTextChar(text,n,&n);

        CharDescriptor *ch = getChar(charId);
        if( ch == 0 ) ch = &_defChar;

        x += _scale * (ch->xAdv);

        if( n < count )
            x += adjustForKerningPairs(charId, getTextChar(text,n));
    }

    return x;
}

void Font::setHeight(float h)
{
    _scale = h / float(_fontHeight);
}

float Font::getHeight()
{
    return _scale * float(_fontHeight);
}

float Font::getBottomOffset()
{
    return _scale * (_base - _fontHeight);
}

float Font::getTopOffset()
{
    return _scale * (_base - 0);
}

int32_t 
Font::getTextLength(const std::string& strText)
{
    const char* text = strText.c_str();
/*
    if (_encoding == UTF16)
    {
        int32_t textLen = 0;
        for(;;)
        {
            uint32_t len;
            int32_t r = acUtility::DecodeUTF16(&text[textLen], &len);
            if( r > 0 )
                textLen += len;
            else if( r < 0 )
                textLen++;
            else
                return textLen;
        }
    }
    */

    // Both UTF8 and standard ASCII strings can use strlen
    return (int32_t)strlen(text);
}

// Internal
int32_t 
Font::getTextChar(const std::string& strText, int32_t pos, int32_t *nextPos)
{
    const char* text = strText.c_str();
    int32_t ch;
    uint32_t len;
    /*
    if( encoding == UTF8 )
    {
        ch = acUtility::DecodeUTF8(&text[pos], &len);
        if( ch == -1 ) len = 1;
    }
    else if( encoding == UTF16 )
    {
        ch = acUtility::DecodeUTF16(&text[pos], &len);
        if( ch == -1 ) len = 2;
    }
    else
    */
    {
        len = 1;
        ch = (unsigned char)text[pos];
    }

    if( nextPos ) *nextPos = pos + len;
    return ch;
}

int32_t 
Font::findTextChar(const std::string& strText, int32_t start, int32_t length, int32_t ch)
{
    const char* text = strText.c_str();
    int32_t pos = start;
    int32_t nextPos;
    int32_t currChar = -1;
    while( pos < length )
    {
        currChar = getTextChar(text, pos, &nextPos);
        if( currChar == ch )
            return pos;
        pos = nextPos;
    } 

    return -1;
}

void Font::internalWrite(const Ibl::Camera* camera, 
                         float x, float y, float z, 
                         const char *text, int32_t count, 
                         const Ibl::Vector4f& color,
                         float spacing)
{
    int32_t page = 0;

    _pages[0]->setShader(_shader);
    _pages[0]->setTechnique(_basicTechnique);
    _pages[0]->albedoColorProperty()->set (color);

    Ibl::Viewport viewPort;
    _deviceInterface->getViewport(&viewPort);

    Ibl::Matrix44f ortho;
    ortho.makeOrthoOffCenterLH(0, viewPort._width,  0, viewPort._height, viewPort._minZ, viewPort._maxZ);

    const GpuVariable * variable = 0;
    _shader->getParameterByName("orthoMatrix", variable);
    variable->setMatrix (&ortho._m[0][0]); 

    _dynamicRenderer->begin(_pages[0], camera);

    _deviceInterface->enableAlphaBlending();
   _deviceInterface->setSrcFunction (Ibl::SourceAlpha);
   _deviceInterface->setDestFunction (Ibl::InverseSourceAlpha);
   _deviceInterface->setBlendProperty (Ibl::OpAdd);
   _deviceInterface->setAlphaSrcFunction (Ibl::BlendOne);
   _deviceInterface->setAlphaDestFunction (Ibl::BlendZero);
   _deviceInterface->setAlphaBlendProperty(Ibl::OpAdd);
    y += _scale * float(_base);

    float yscale = (float)(_deviceInterface->backbuffer()->width()) / (float)(_deviceInterface->backbuffer()->height());


    for( int32_t n = 0; n < count; )
    {
        int32_t charId = getTextChar(text, n, &n);
        CharDescriptor *ch = getChar(charId);
        if( ch == 0 ) 
            ch = &_defChar;

        float u = (float(ch->srcX)) / (float)_scaleW;
        float v = (float(ch->srcY)) / (float)_scaleH;
        float u2 = u + float(ch->srcW) / (float)_scaleW;
        float v2 = v + float(ch->srcH) / (float)_scaleH;

        float a = _scale * float(ch->xAdv);
        float w = _scale * float(ch->srcW);
        float h = _scale * float(ch->srcH);
        float ox = _scale * float(ch->xOff);
        float oy = _scale * float(ch->yOff);


        if( ch->page != page )
        {
            LOG("Lots of chars, need to fix this ...");
        }
        
        // A
        _dynamicRenderer->vertex3f(Ibl::Vector3f(x+ox, (y-oy), z),
                                   Ibl::Vector2f(u, v));

        // B
        _dynamicRenderer->vertex3f(Ibl::Vector3f(x+w+ox, (y-oy) , z),
                                   Ibl::Vector2f(u2, v));

        // C
        _dynamicRenderer->vertex3f(Ibl::Vector3f(x+w+ox, (y-h-oy) , z),
                                   Ibl::Vector2f(u2, v2));

        // A
        _dynamicRenderer->vertex3f(Ibl::Vector3f(x+ox, (y-oy) , z),
                                   Ibl::Vector2f(u, v));

           // C
        _dynamicRenderer->vertex3f(Ibl::Vector3f(x+w+ox, (y-h-oy) , z),
                                   Ibl::Vector2f(u2, v2));

        // D
        _dynamicRenderer->vertex3f(Ibl::Vector3f(x+ox, (y-h-oy) , z),
                                   Ibl::Vector2f(u, v2));

        x += a;
        if( charId == ' ' )
            x += spacing;

        if( n < count )
            x += adjustForKerningPairs(charId, getTextChar(text,n));
    }
    _deviceInterface->setDrawMode(Ibl::Filled);
    _dynamicRenderer->end();
    _deviceInterface->disableAlphaBlending();    
}

void 
Font::write(const Ibl::Camera* camera,
            float x, float y, float z, 
            const std::string& text, int32_t count, uint32_t mode,
            const Ibl::Vector4f& color)
{
    if( count <= 0 )
        count = getTextLength(text);

    if( mode == FONT_ALIGN_CENTER )
    {
        float w = getTextWidth(text, count);
        x -= w/2.0f;
    }
    else if( mode == FONT_ALIGN_RIGHT )
    {
        float w = getTextWidth(text, count);
        x -= w;
    }

    internalWrite(camera, x, y, z, text.c_str(), count, color);
}

void 
Font::writeML(const Ibl::Camera* camera,
              float x, float y, float z, 
              const std::string& strText, int32_t count, uint32_t mode,
              const Ibl::Vector4f& color)
{
    const char* text = strText.c_str();

    if( count <= 0 )
        count = getTextLength(text);

    // Get first line
    int32_t pos = 0;
    int32_t len = findTextChar(text, pos, count, '\n'); 
    if( len == -1 ) len = count;
    while( pos < count )
    {
        float cx = x;
        if( mode == FONT_ALIGN_CENTER)
        {
            float w = getTextWidth(&text[pos], len);
            cx -= w/2;
        }
        else if( mode == FONT_ALIGN_RIGHT)
        {
            float w = getTextWidth(&text[pos], len);
            cx -= w;
        }

        internalWrite(camera, cx, y, z, &text[pos], len, color);

        y -= _scale * float(_fontHeight);

        // Get next line
        pos += len;
        int32_t ch = getTextChar(text, pos, &pos);
        if( ch == '\n' )
        {
            len = findTextChar(text, pos, count, '\n');
            if( len == -1 )
                len = count - pos;
            else 
                len = len - pos;
        }
    }
}

void 
Font::writeBox (const Ibl::Camera* camera, 
                float x, float y, float z, float width, 
                const std::string& strText, int32_t count, uint32_t mode,
                const Ibl::Vector4f& color)
{
    const char* text = strText.c_str();

    if( count <= 0 )
        count = getTextLength(text);

    float currWidth = 0, wordWidth;
    int32_t lineS = 0, lineE = 0, wordS = 0, wordE = 0;
    int32_t wordCount = 0;

    const char *s = " ";
    float spaceWidth = getTextWidth(s, 1);
    bool softBreak = false;
    
    for(; lineS < count;)
    {
        // Determine the extent of the line
        for(;;)
        {
            // Determine the number of characters in the word
            while( wordE < count && 
                getTextChar(text,wordE) != ' ' &&
                getTextChar(text,wordE) != '\n' )
                // Advance the cursor to the next character
                getTextChar(text,wordE,&wordE);

            // Determine the width of the word
            if( wordE > wordS )
            {
                wordCount++;
                wordWidth = getTextWidth(&text[wordS], wordE-wordS);
            }
            else
                wordWidth = 0;

            // Does the word fit on the line? The first word is always accepted.
            if( wordCount == 1 || currWidth + (wordCount > 1 ? spaceWidth : 0) + wordWidth <= width )
            {
                // Increase the line extent to the end of the word
                lineE = wordE;
                currWidth += (wordCount > 1 ? spaceWidth : 0) + wordWidth;

                // Did we reach the end of the line?
                if( wordE == count || getTextChar(text,wordE) == '\n' )
                {
                    softBreak = false;

                    // Skip the newline character
                    if( wordE < count )
                        // Advance the cursor to the next character
                        getTextChar(text,wordE,&wordE);

                    break;
                }                

                // Skip the trailing space
                if( wordE < count && getTextChar(text,wordE) == ' ' )
                    // Advance the cursor to the next character
                    getTextChar(text,wordE,&wordE);

                // Move to next word
                wordS = wordE;
            }
            else
            {
                softBreak = true;

                // Skip the trailing space
                if( wordE < count && getTextChar(text,wordE) == ' ' )
                    // Advance the cursor to the next character
                    getTextChar(text,wordE,&wordE);

                break;
            }
        }

        // Write the line
        if( mode == FONT_ALIGN_JUSTIFY )
        {
            float spacing = 0;
            if( softBreak )
            {
                if( wordCount > 2 )
                    spacing = (width - currWidth) / (wordCount-2);
                else
                    spacing = (width - currWidth);
            }
            
            internalWrite(camera, x, y, z, &text[lineS], lineE - lineS, color, spacing);
        }
        else
        {
            float cx = x;
            if( mode == FONT_ALIGN_RIGHT )
                cx = x + width - currWidth;
            else if( mode == FONT_ALIGN_CENTER )
                cx = x + 0.5f*(width - currWidth);

            internalWrite(camera, cx, y, z, &text[lineS], lineE - lineS, color);
        }

        if( softBreak )
        {
            // Skip the trailing space
            if( lineE < count && getTextChar(text,lineE) == ' ' )
                // Advance the cursor to the next character
                getTextChar(text,lineE,&lineE);

            // We've already counted the first word on the next line
            currWidth = wordWidth;
            wordCount = 1;
        }
        else
        {
            // Skip the line break
            if( lineE < count && getTextChar(text,lineE) == '\n' )
                // Advance the cursor to the next character
                getTextChar(text,lineE,&lineE);

            currWidth = 0;
            wordCount = 0;
        }
        
        // Move to next line
        lineS = lineE;
        wordS = wordE;
        y -= _scale * float(_fontHeight);
    }
}

void 
Font::preparePixelPerfectOutput()
{
/*
*/
}

//----------------------------------------------------------------------
// FontLoader
//
// This is the base class for all loader classes. This is the only class
// that has access to and knows how to set the Font members.
//----------------------------------------------------------------------

FontLoader::FontLoader(FILE *f, Font *font, const char *fontFile)
{
    this->f = f;
    this->font = font;
    this->fontFile = fontFile;

    outlineThickness = 0;
}

void FontLoader::loadPage(int32_t id, const char *pageFile, const char *fontFile)
{
    // Load the font textures
    std::string texturePathName = std::string("data/textures/fonts/") + pageFile;
    Material* material = new Material(font->_deviceInterface);
    material->setAlbedoMap (texturePathName);

    font->_pages[id] = material;
}

void FontLoader::setFontInfo(int32_t outlineThickness)
{
    this->outlineThickness = outlineThickness;
}

void FontLoader::setCommonInfo(int32_t fontHeight, int32_t base, int32_t scaleW, int32_t scaleH, int32_t pages, bool isPacked)
{
    font->_fontHeight = fontHeight;
    font->_base = base;
    font->_scaleW = scaleW;
    font->_scaleH = scaleH;
    font->_pages.resize(pages);
    for( int32_t n = 0; n < pages; n++ )
        font->_pages[n] = 0;

    if( isPacked && outlineThickness )
        font->_hasOutline = true;
}

void FontLoader::addChar(int32_t id, int32_t x, int32_t y, int32_t w, int32_t h, 
                         int32_t xoffset, int32_t yoffset, 
                         int32_t xadvance, int32_t page, int32_t chnl)
{
    // Convert to a 4 element vector
    if     ( chnl == 1 ) chnl = 0x00010000;  // Blue channel
    else if( chnl == 2 ) chnl = 0x00000100;  // Green channel
    else if( chnl == 4 ) chnl = 0x00000001;  // Red channel
    else if( chnl == 8 ) chnl = 0x01000000;  // Alpha channel
    else chnl = 0;

    if( id >= 0 )
    {
        CharDescriptor *ch = new CharDescriptor;
        ch->srcX = x;
        ch->srcY = y;
        ch->srcW = w;
        ch->srcH = h;
        ch->xOff = xoffset;
        ch->yOff = yoffset;
        ch->xAdv = xadvance;
        ch->page = page;
        ch->chnl = chnl;

        font->_chars.insert(std::map<int, CharDescriptor*>::value_type(id, ch));
    }

    if( id == -1 )
    {
        font->_defChar.srcX = x;
        font->_defChar.srcY = y;
        font->_defChar.srcW = w;
        font->_defChar.srcH = h;
        font->_defChar.xOff = xoffset;
        font->_defChar.yOff = yoffset;
        font->_defChar.xAdv = xadvance;
        font->_defChar.page = page;
        font->_defChar.chnl = chnl;
    }
}

void FontLoader::addKerningPair(int32_t first, int32_t second, int32_t amount)
{
    if( first >= 0 && first < 256 && font->_chars[first] )
    {
        font->_chars[first]->kerningPairs.push_back(second);
        font->_chars[first]->kerningPairs.push_back(amount);
    }
}

//=============================================================================
// FontLoaderTextFormat
//
// This class implements the logic for loading a BMFont file in text format
//=============================================================================

FontLoaderTextFormat::FontLoaderTextFormat(FILE *f, Font *font, const char *fontFile) : FontLoader(f, font, fontFile)
{
}

int32_t FontLoaderTextFormat::load()
{
    std::string line;

    while( !feof(f) )
    {
        // Read until line feed (or EOF)
        line = "";
        line.reserve(256);
        while( !feof(f) )
        {
            char ch;
            if( fread(&ch, 1, 1, f) )
            {
                if( ch != '\n' ) 
                    line += ch; 
                else
                    break;
            }
        }

        // Skip white spaces
        int32_t pos = skipWhiteSpace(line, 0);
        if( pos >= (int32_t)(line.size()) ) break;

        // Read token
        int32_t pos2 = findEndOfToken(line, pos);
        std::string token = line.substr(pos, pos2-pos);

        // Interpret line
        if( token == "info" )
            interpretInfo(line, pos2);
        else if( token == "common" )
            interpretCommon(line, pos2);
        else if( token == "char" )
            interpretChar(line, pos2);
        else if( token == "kerning" )
            interpretKerning(line, pos2);
        else if( token == "page" )
            interpretPage(line, pos2, fontFile);
    }

    fclose(f);

    // Success
    return 0;
}

int32_t FontLoaderTextFormat::skipWhiteSpace(std::string &str, int32_t start)
{
    UINT n = start;
    while( n < str.size() )
    {
        char ch = str[n];
        if( ch != ' ' && 
            ch != '\t' && 
            ch != '\r' && 
            ch != '\n' )
            break;

        ++n;
    }

    return n;
}

int32_t FontLoaderTextFormat::findEndOfToken(std::string &str, int32_t start)
{
    UINT n = start;
    if( str[n] == '"' )
    {
        n++;
        while( n < str.size() )
        {
            char ch = str[n];
            if( ch == '"' )
            {
                // Include the last quote char in the token
                ++n;
                break;
            }
            ++n;
        }
    }
    else
    {
        while( n < str.size() )
        {
            char ch = str[n];
            if( ch == ' ' ||
                ch == '\t' ||
                ch == '\r' ||
                ch == '\n' ||
                ch == '=' )
                break;

            ++n;
        }
    }

    return n;
}

void FontLoaderTextFormat::interpretKerning(std::string &str, int32_t start)
{
    // Read the attributes
    int32_t first = 0;
    int32_t second = 0;
    int32_t amount = 0;

    int32_t pos = -1;
    int32_t pos2 = start;
    while( true )
    {
        pos = skipWhiteSpace(str, pos2);
        
        if (pos >= (int32_t)(str.size())) break;
        pos2 = findEndOfToken(str, pos);

        std::string token = str.substr(pos, pos2-pos);

        pos = skipWhiteSpace(str, pos2);
        if (pos >= (int32_t)(str.size())) break;

        if( pos == str.size() || str[pos] != '=' ) break;

        pos = skipWhiteSpace(str, pos+1);
        if (pos >= (int32_t)(str.size())) break;
        pos2 = findEndOfToken(str, pos);

        std::string value = str.substr(pos, pos2-pos);

        if( token == "first" )
            first = strtol(value.c_str(), 0, 10);
        else if( token == "second" )
            second = strtol(value.c_str(), 0, 10);
        else if( token == "amount" )
            amount = strtol(value.c_str(), 0, 10);

        if( pos == str.size() ) break;
    }

    // Store the attributes
    addKerningPair(first, second, amount);
}

void FontLoaderTextFormat::interpretChar(std::string &str, int32_t start)
{
    // Read all attributes
    int32_t id = 0;
    int32_t x = 0;
    int32_t y = 0;
    int32_t width = 0;
    int32_t height = 0;
    int32_t xoffset = 0;
    int32_t yoffset = 0;
    int32_t xadvance = 0;
    int32_t page = 0;
    int32_t chnl = 0;

    int32_t pos = -1;
    int32_t pos2 = start;
    while( true )
    {
        pos = skipWhiteSpace(str, pos2);
        if (pos >= (int32_t)(str.size())) break;

        pos2 = findEndOfToken(str, pos);

        std::string token = str.substr(pos, pos2-pos);

        pos = skipWhiteSpace(str, pos2);
        if (pos >= (int32_t)(str.size())) break;
        if (pos == (int32_t)(str.length()) || str[pos] != '=') break;

        pos = skipWhiteSpace(str, pos+1);
        if (pos >= (int32_t)(str.size())) break;
        pos2 = findEndOfToken(str, pos);

        std::string value = str.substr(pos, pos2-pos);

        if( token == "id" )
            id = strtol(value.c_str(), 0, 10);
        else if( token == "x" )
            x = strtol(value.c_str(), 0, 10);
        else if( token == "y" )
            y = strtol(value.c_str(), 0, 10);
        else if( token == "width" )
            width = strtol(value.c_str(), 0, 10);
        else if( token == "height" )
            height = strtol(value.c_str(), 0, 10);
        else if( token == "xoffset" )
            xoffset = strtol(value.c_str(), 0, 10);
        else if( token == "yoffset" )
            yoffset = strtol(value.c_str(), 0, 10);
        else if( token == "xadvance" )
            xadvance = strtol(value.c_str(), 0, 10);
        else if( token == "page" )
            page = strtol(value.c_str(), 0, 10);
        else if( token == "chnl" )
            chnl = strtol(value.c_str(), 0, 10);

        if( pos == str.size() ) break;
    }

    // Store the attributes
    addChar(id, x, y, width, height, xoffset, yoffset, xadvance, page, chnl);
}

void FontLoaderTextFormat::interpretCommon(std::string &str, int32_t start)
{
    int32_t fontHeight = 0;
    int32_t base = 0;
    int32_t scaleW = 0;
    int32_t scaleH = 0;
    int32_t pages = 0;
    int32_t packed = 0;

    // Read all attributes
    int32_t pos = -1;
    int32_t pos2 = start;

    while( true )
    {
        pos = skipWhiteSpace(str, pos2);
        if (pos >= (int32_t)(str.size())) break;

        pos2 = findEndOfToken(str, pos);

        std::string token = str.substr(pos, pos2-pos);

        pos = skipWhiteSpace(str, pos2);
        if (pos >= (int32_t)(str.size())) break;
        if (pos == (int32_t)(str.size()) || str[pos] != '=') break;

        pos = skipWhiteSpace(str, pos+1);
        if (pos >= (int32_t)(str.size())) break;
        pos2 = findEndOfToken(str, pos);

        std::string value = str.substr(pos, pos2-pos);

        if( token == "lineHeight" )
            fontHeight = (short)strtol(value.c_str(), 0, 10);
        else if( token == "base" )
            base = (short)strtol(value.c_str(), 0, 10);
        else if( token == "scaleW" )
            scaleW = (short)strtol(value.c_str(), 0, 10);
        else if( token == "scaleH" )
            scaleH = (short)strtol(value.c_str(), 0, 10);
        else if( token == "pages" )
            pages = strtol(value.c_str(), 0, 10);
        else if( token == "packed" )
            packed = strtol(value.c_str(), 0, 10);

        if( pos == str.size() ) break;
    }

    setCommonInfo(fontHeight, base, scaleW, scaleH, pages, packed ? true : false);
}

void FontLoaderTextFormat::interpretInfo(std::string &str, int32_t start)
{
    int32_t outlineThickness = 0;

    // Read all attributes
    int32_t pos, pos2 = start;
    while( true )
    {
        pos = skipWhiteSpace(str, pos2);
        
        if (pos >= (int32_t)(str.size())) break;

        pos2 = findEndOfToken(str, pos);

        std::string token = str.substr(pos, pos2-pos);

        pos = skipWhiteSpace(str, pos2);
        if (pos >= (int32_t)(str.size())) break;
        if (pos == (int32_t)(str.size()) || str[pos] != '=') break;

        pos = skipWhiteSpace(str, pos+1);
        if (pos >= (int32_t)(str.size())) break;
        pos2 = findEndOfToken(str, pos);

        std::string value = str.substr(pos, pos2-pos);

        if( token == "outline" )
            outlineThickness = (short)strtol(value.c_str(), 0, 10);

        if( pos == str.size() ) break;
    }

    setFontInfo(outlineThickness);
}

void FontLoaderTextFormat::interpretPage(std::string &str, int32_t start, const char *fontFile)
{
    int32_t id = 0;
    std::string file;

    // Read all attributes
    int32_t pos, pos2 = start;
    while( true )
    {
        pos = skipWhiteSpace(str, pos2);

        if (pos >= (int32_t)(str.size())) break;

        pos2 = findEndOfToken(str, pos);

        std::string token = str.substr(pos, pos2-pos);

        pos = skipWhiteSpace(str, pos2);
        if (pos >= (int32_t)(str.size())) break;
        if (pos == (int32_t)(str.size()) || str[pos] != '=') break;

        pos = skipWhiteSpace(str, pos+1);
        if (pos >= (int32_t)(str.size())) break;
        pos2 = findEndOfToken(str, pos);

        std::string value = str.substr(pos, pos2-pos);

        if( token == "id" )
            id = strtol(value.c_str(), 0, 10);
        else if( token == "file" )
            file = value.substr(1, value.length()-2);

        if( pos == str.size() ) break;
    }

    loadPage(id, file.c_str(), fontFile);
}

//---------------------------------------------------------------------------
// FontLoaderBinaryFormat
//
// This class implements the logic for loading a BMFont file in binary format
//---------------------------------------------------------------------------

FontLoaderBinaryFormat::FontLoaderBinaryFormat(FILE *f, Font *font, const char *fontFile) : 
    FontLoader(f, font, fontFile)
{
}

int32_t FontLoaderBinaryFormat::load()
{
    // Read and validate the tag. It should be 66, 77, 70, 2, 
    // or 'BMF' and 2 where the number is the file version.
    char magicString[4];
    fread(magicString, 4, 1, f);
    if( strncmp(magicString, "BMF\003", 4) != 0 )
    {
        LOG("Unrecognized format for " << fontFile);
        fclose(f);
        return -1;
    }

    // Read each block
    char blockType;
    int32_t blockSize;
    while( fread(&blockType, 1, 1, f) )
    {
        // Read the blockSize
        fread(&blockSize, 4, 1, f);

        switch( blockType )
        {
        case 1: // info
            readInfoBlock(blockSize);
            break;
        case 2: // common
            readCommonBlock(blockSize);
            break;
        case 3: // pages
            readPagesBlock(blockSize);
            break;
        case 4: // chars
            readCharsBlock(blockSize);
            break;
        case 5: // kerning pairs
            readKerningPairsBlock(blockSize);
            break;
        default:
            LOG("Unexpected block type " << blockType);
            fclose(f);
            return -1;
        }
    }

    fclose(f);

    // Success
    return 0;
}

void FontLoaderBinaryFormat::readInfoBlock(int32_t size)
{
#pragma pack(push)
#pragma pack(1)
struct infoBlock
{
    WORD fontSize;
    BYTE reserved:4;
    BYTE bold    :1;
    BYTE italic  :1;
    BYTE unicode :1;
    BYTE smooth  :1;
    BYTE charSet;
    WORD stretchH;
    BYTE aa;
    BYTE paddingUp;
    BYTE paddingRight;
    BYTE paddingDown;
    BYTE paddingLeft;
    BYTE spacingHoriz;
    BYTE spacingVert;
    BYTE outline;         // Added with version 2
    char fontName[1];
};
#pragma pack(pop)

    char *buffer = new char[size];
    fread(buffer, size, 1, f);

    // We're only interested in the outline thickness
    infoBlock *blk = (infoBlock*)buffer;
    setFontInfo(blk->outline);

    delete[] buffer;
}

void FontLoaderBinaryFormat::readCommonBlock(int32_t size)
{
#pragma pack(push)
#pragma pack(1)
struct commonBlock
{
    WORD lineHeight;
    WORD base;
    WORD scaleW;
    WORD scaleH;
    WORD pages;
    BYTE packed  :1;
    BYTE reserved:7;
    BYTE alphaChnl;
    BYTE redChnl;
    BYTE greenChnl;
    BYTE blueChnl;
}; 
#pragma pack(pop)

    char *buffer = new char[size];
    fread(buffer, size, 1, f);

    commonBlock *blk = (commonBlock*)buffer;

    setCommonInfo(blk->lineHeight, blk->base, blk->scaleW, blk->scaleH, blk->pages, blk->packed ? true : false);

    delete[] buffer;
}

void FontLoaderBinaryFormat::readPagesBlock(int32_t size)
{
#pragma pack(push)
#pragma pack(1)
struct pagesBlock
{
    char pageNames[1];
};
#pragma pack(pop)

    char *buffer = new char[size];
    fread(buffer, size, 1, f);

    pagesBlock *blk = (pagesBlock*)buffer;

    for( int32_t id = 0, pos = 0; pos < size; id++ )
    {
        loadPage(id, &blk->pageNames[pos], fontFile);
        pos += 1 + (int32_t)strlen(&blk->pageNames[pos]);
    }

    delete[] buffer;
}

void FontLoaderBinaryFormat::readCharsBlock(int32_t size)
{
#pragma pack(push)
#pragma pack(1)
struct charsBlock
{
    struct charInfo
    {
        DWORD id;
        WORD  x;
        WORD  y;
        WORD  width;
        WORD  height;
        short xoffset;
        short yoffset;
        short xadvance;
        BYTE  page;
        BYTE  chnl;
    } chars[1];
};
#pragma pack(pop)

    char *buffer = new char[size];
    fread(buffer, size, 1, f);

    charsBlock *blk = (charsBlock*)buffer;

    for( int32_t n = 0; int(n*sizeof(charsBlock::charInfo)) < size; n++ )
    {
        addChar(blk->chars[n].id,
                blk->chars[n].x,
                blk->chars[n].y,
                blk->chars[n].width,
                blk->chars[n].height,
                blk->chars[n].xoffset,
                blk->chars[n].yoffset,
                blk->chars[n].xadvance,
                blk->chars[n].page,
                blk->chars[n].chnl);
    }

    delete[] buffer;
}

void FontLoaderBinaryFormat::readKerningPairsBlock(int32_t size)
{
#pragma pack(push)
#pragma pack(1)
struct kerningPairsBlock
{
    struct kerningPair
    {
        DWORD first;
        DWORD second;
        short amount;
    } kerningPairs[1];
};
#pragma pack(pop)

    char *buffer = new char[size];
    fread(buffer, size, 1, f);

    kerningPairsBlock *blk = (kerningPairsBlock*)buffer;

    for( int32_t n = 0; (int32_t)(n*sizeof(kerningPairsBlock::kerningPair)) < size; n++ )
    {
        addKerningPair(blk->kerningPairs[n].first,
                       blk->kerningPairs[n].second,
                       blk->kerningPairs[n].amount);
    }

    delete[] buffer;
}

}
