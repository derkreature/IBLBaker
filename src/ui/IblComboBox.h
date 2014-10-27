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

#ifndef INCLUDED_BB_WIDGET_COMBO_BOX
#define INCLUDED_BB_WIDGET_COMBO_BOX

#include <IblButton.h>
#include <IblScrollBar.h>

namespace Ibl
{

struct ComboBoxItem
{
    WCHAR strText[256];
    void*  pData;

    RECT  rcActive;
    bool  bVisible;
};

class ComboBox : public Button
{
public:
    ComboBox( Dialog *pDialog, Ibl::IDevice* renderMgr );
    virtual ~ComboBox();
    
    virtual void setTextColor(Ibl::PackedColor Color );
    virtual HRESULT init();

    virtual bool handleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool handleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void hotkey();

    virtual bool canHaveFocus() { return (_visible && _enabled); }
    virtual void focusOut();
    virtual void render( float elapsedTime );

    virtual void updateRects(); 

    HRESULT AddItem( const WCHAR* strText, void* pData );
    void    RemoveAllItems();
    void    RemoveItem( UINT index );
    bool    ContainsItem( const WCHAR* strText, UINT iStart=0 );
    int     FindItem( const WCHAR* strText, UINT iStart=0 );
    void*   GetItemData( const WCHAR* strText );
    void*   GetItemData( int nIndex );
    void    SetDropHeight( UINT nHeight ) { m_nDropHeight = nHeight; updateRects(); }
    void    SetScrollBarWidth( int nWidth ) { m_nSBWidth = nWidth; updateRects(); }

    void*   GetSelectedData();
    ComboBoxItem* GetSelectedItem();

    UINT    GetNumItems() { return (int)m_Items.size(); }
    ComboBoxItem* GetItem( UINT index ) { return m_Items[index]; }

    HRESULT SetSelectedByIndex( UINT index );
    HRESULT SetSelectedByText( const WCHAR* strText );
    HRESULT SetSelectedByData( void* pData );  

protected:
    int     m_iSelected;
    int     m_iFocused;
    int     m_nDropHeight;
    ScrollBar m_ScrollBar;
    int     m_nSBWidth;

    bool    m_bOpened;

    RECT m_rcText;
    RECT _buttonBounds;
    RECT m_rcDropdown;
    RECT m_rcDropdownText;
    std::vector <ComboBoxItem*> m_Items;
};
}
#endif
