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

#ifndef INCLUDED_BB_WIDGET_DIALOG_RESOURCE_MGR
#define INCLUDED_BB_WIDGET_DIALOG_RESOURCE_MGR

#include <IblWidgetTypes.h>
#include <IblWidgetUIElement.h>
#include <IblITexture.h>
#include <IblFont.h>

namespace Ibl
{
class IDevice;
class Sprite;

//-----------------------------
// Structs for shared resources
//-----------------------------
class TextureNode
{
  public:
    TextureNode() :
        _texture (0)
    {
    }

    ~TextureNode()
    {
    }

    const char*                filename() {return _filename;}
    void                       setFileName (const char* filename) { memcpy (_filename, filename, (strlen (filename)+1) * sizeof(char));}
    const Ibl::ITexture* texture() { return _texture;}
    void                       setTexture (const Ibl::ITexture* texture) { _texture = texture;}
 
  protected:
    char  _filename[MAX_PATH];
    const Ibl::ITexture* _texture;
};

struct FontNode
{
    Ibl::Font* font() { return _font;}
    char      strFace[MAX_PATH];
    Ibl::Font* _font;
    LONG      nHeight;
    LONG      nWeight;
};

//-----------------
// resource Manager
//-----------------
class DialogResourceManager
{
  public:
    int AddFont( const char* strFaceName, LONG height, LONG weight );
    int AddTexture( const char* strFilename );

    FontNode*     GetFontNode( int iIndex )     { return m_FontCache[iIndex ]; };
    TextureNode*  GetTextureNode( int iIndex )  { return m_TextureCache[iIndex]; };

    // Shared between all dialogs
           

    ~DialogResourceManager();

    // Automation helpers
    void setAutomation( bool b ) { _automation = b; }
    bool automation() const { return _automation; }

    HRESULT     create (Ibl::IDevice* device);
    HRESULT     reset ();
    void        lost ();
    void        destroy ();

    const Ibl::Sprite*  sprite();

  protected:
    friend DialogResourceManager*   GetGlobalDialogResourceManager();
    friend HRESULT Initialize3DEnvironment();
    friend HRESULT Reset3DEnvironment();
    friend void Cleanup3DEnvironment( bool bReleaseSettings );

    DialogResourceManager();

     // Sprite used for drawing
    Ibl::Sprite*         _sprite;

    std::vector <TextureNode*> m_TextureCache;   
    std::vector <FontNode*>    m_FontCache;         
    Ibl::IDevice*       _device;

    // resource creation helpers
    HRESULT                   CreateFont (UINT index);
    HRESULT                   CreateTexture (UINT index);

    // Automation helper
    bool                       _automation;
};
DialogResourceManager* GetGlobalDialogResourceManager();
void DestoryDialogResourceManager();

}
#endif