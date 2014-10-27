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

#include <IblDialogResourceManager.h>
#include <IblIDevice.h>
#include <IblSprite.h>

namespace Ibl
{
DialogResourceManager * manager = nullptr;

DialogResourceManager* GetGlobalDialogResourceManager()
{
    if (manager == nullptr)
    {
        manager = new DialogResourceManager();
    }
    return manager;
}

void
DestoryDialogResourceManager()
{
    safedelete(manager);

}

DialogResourceManager::DialogResourceManager()
{
    _sprite = 0;
    _automation = false;
}

DialogResourceManager::~DialogResourceManager()
{
    uint32_t i;
    for( i=0; i < (uint32_t)m_FontCache.size(); i++ )
    {
        FontNode* pFontNode = m_FontCache[i];
        safedelete( pFontNode );
    }
    m_FontCache.clear();   

    for( i=0; i < (uint32_t)m_TextureCache.size(); i++ )
    {
        TextureNode* pTextureNode = m_TextureCache[i ];
        safedelete( pTextureNode );
    }
    m_TextureCache.clear();   
    safedelete(_sprite);
}

const Ibl::Sprite*    
DialogResourceManager::sprite()
{
    return _sprite;
}

HRESULT DialogResourceManager::create (Ibl::IDevice* device)
{
    HRESULT hr = S_OK;
    uint32_t i=0;

    _device = device;

    for( i=0; i < (uint32_t)m_FontCache.size(); i++ )
    {
        hr = CreateFont( i );
        if( FAILED(hr) )
            return hr;
    }
    
    for( i=0; i < (uint32_t) m_TextureCache.size(); i++ )
    {
        if (FAILED (CreateTexture( i )))
            return E_FAIL;
    }

    _sprite = new Ibl::Sprite(_device);
    if (_sprite->initialize())
    {
        return S_OK;
    }
    return E_FAIL;
}

HRESULT DialogResourceManager::reset()
{
    HRESULT hr = S_OK;
    return S_OK;
}

void DialogResourceManager::lost ()
{
}

void DialogResourceManager::destroy ()
{
}

int DialogResourceManager::AddFont( const char* strFaceName, LONG height, LONG weight )
{
    // See if this font already exists
    for( uint32_t i=0; i < (uint32_t)m_FontCache.size(); i++ )
    {
        // mdavidson, removed fontname comparison temporarily
        FontNode* pFontNode = m_FontCache[i];
        if (pFontNode->nHeight == height &&
            pFontNode->nWeight == weight )
        {
            return i;
        }
    }

    // Add a new font and try to create it
    FontNode* pNewFontNode = new FontNode();
    if( pNewFontNode == nullptr )
        return -1;

    memset ( pNewFontNode, 0, sizeof(FontNode) );
    memcpy ( pNewFontNode->strFace, strFaceName , strlen (strFaceName)+1 );

    pNewFontNode->nHeight = height;
    pNewFontNode->nWeight = weight;
    m_FontCache.push_back( pNewFontNode );
    
    uint32_t iFont = (uint32_t)(m_FontCache.size()-1);

    CreateFont( iFont );

    return iFont;
}

}