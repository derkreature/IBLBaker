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

#include <IblComboBox.h>
#include <IblWidgetDialog.h>
#include <IblDialogResourceManager.h>
#include <IblWidgetTypes.h>

namespace Ibl
{

ComboBox::ComboBox (Dialog *dialog, Ibl::IDevice* renderMgr ) : 
    Button (dialog, renderMgr),
    m_ScrollBar(dialog, renderMgr)
{
    _type = _CONTROL_COMBOBOX;

    m_nDropHeight = 100;

    m_nSBWidth = 16;
    m_bOpened = false;
    m_iSelected = -1;
    m_iFocused = -1;
}

ComboBox::~ComboBox()
{
    RemoveAllItems();
}

void ComboBox::setTextColor (Ibl::PackedColor Color )
{
    UIElement* pElement = _elements[0];

    if( pElement )
        pElement->FontColor.States[_STATE_NORMAL] = Color;

    pElement = _elements[2];

    if( pElement )
        pElement->FontColor.States[_STATE_NORMAL] = Color;
}

void ComboBox::updateRects()
{
    
    //problem seems to stem from here
    Button::updateRects();

    _buttonBounds = _boundingBox;
    _buttonBounds.left = _buttonBounds.right - RectHeight( _buttonBounds );

    m_rcText = _boundingBox;
    m_rcText.right = _buttonBounds.left;

    m_rcDropdown = m_rcText;
    OffsetRect( &m_rcDropdown, 0, (int) (0.90f * RectHeight( m_rcText )) );
    m_rcDropdown.bottom += m_nDropHeight;
    m_rcDropdown.right -= m_nSBWidth;

    m_rcDropdownText = m_rcDropdown;
    m_rcDropdownText.left += (int) (0.1f * RectWidth( m_rcDropdown ));
    m_rcDropdownText.right -= (int) (0.1f * RectWidth( m_rcDropdown ));
    m_rcDropdownText.top += (int) (0.1f * RectHeight( m_rcDropdown ));
    m_rcDropdownText.bottom -= (int) (0.1f * RectHeight( m_rcDropdown ));

    // Update the scrollbar's rects
    m_ScrollBar.setLocation( m_rcDropdown.right, m_rcDropdown.top+2 );
    m_ScrollBar.setSize( m_nSBWidth, RectHeight( m_rcDropdown )-2 );

    
    
    FontNode* pFontNode = GetGlobalDialogResourceManager()->GetFontNode( _elements[2]->iFont );
    
    if( pFontNode && pFontNode->nHeight )
    {
        m_ScrollBar.SetPageSize( RectHeight( m_rcDropdownText ) / pFontNode->nHeight );

        // The selected item may have been scrolled off the page.
        // Ensure that it is in page again.
        m_ScrollBar.ShowItem( m_iSelected );
    }
}

void ComboBox::focusOut()
{
    Button::focusOut();

    m_bOpened = false;
}

bool ComboBox::handleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    const uint32_t REPEAT_MASK = (0x40000000);

    if( !_enabled || !visible() )
        return false;

    // Let the scroll bar have a chance to handle it first
    if( m_ScrollBar.handleKeyboard( uMsg, wParam, lParam ) )
        return true;

    switch( uMsg )
    {
        case WM_KEYDOWN:
        {
            switch( wParam )
            {
                case VK_RETURN:
                    if( m_bOpened )
                    {
                        if( m_iSelected != m_iFocused )
                        {
                            m_iSelected = m_iFocused;
                            _dialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
                        }
                        m_bOpened = false;
                        
                        if( !_dialog->m_bKeyboardInput )
                            _dialog->ClearFocus();

                        return true;
                    }
                    break;

                case VK_F4:
                    // Filter out auto-repeats
                    if( lParam & REPEAT_MASK )
                        return true;

                    m_bOpened = !m_bOpened;

                    if( !m_bOpened )
                    {
                        _dialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );

                        if( !_dialog->m_bKeyboardInput )
                            _dialog->ClearFocus();
                    }

                    return true;

                case VK_LEFT:
                case VK_UP:
                    if( m_iFocused > 0 )
                    {
                        m_iFocused--;
                        m_iSelected = m_iFocused;

                        if( !m_bOpened )
                            _dialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
                    }
                    
                    return true;

                case VK_RIGHT:
                case VK_DOWN:
                    if( m_iFocused+1 < (int)GetNumItems() )
                    {
                        m_iFocused++;
                        m_iSelected = m_iFocused;

                        if( !m_bOpened )
                            _dialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
                    }

                    return true;
            }
            break;
        }
    }

    return false;
}

bool ComboBox::handleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    if( !_enabled || !visible() )
        return false;

    // Let the scroll bar handle it first.
    if( m_ScrollBar.handleMouse( uMsg, pt, wParam, lParam ) )
        return true;

    switch( uMsg )
    {
        case WM_MOUSEMOVE:
        {
            if( m_bOpened && PtInRect( &m_rcDropdown, pt ) )
            {
                // Determine which item has been selected
                for( uint32_t i=0; i < (uint32_t)m_Items.size(); i++ )
                {
                    ComboBoxItem* pItem = m_Items[i];
                    if( pItem -> bVisible &&
                        PtInRect( &pItem->rcActive, pt ) )
                    {
                        m_iFocused = i;
                    }
                }
                return true;
            }
            break;
        }

        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        {
            if( containsPoint( pt ) )
            {
                // Pressed while inside the control
                _pressed = true;

                // mdavidson
                SetCapture(_dialog->windowHandle());

                if( !_hasFocus )
                    _dialog->RequestFocus( this );

                // Toggle dropdown
                if( _hasFocus )
                {
                    m_bOpened = !m_bOpened;
                
                    if( !m_bOpened )
                    {
                        if( !_dialog->m_bKeyboardInput )
                            _dialog->ClearFocus();
                    }
                }

                return true;
            }

            // Perhaps this click is within the dropdown
            if( m_bOpened && PtInRect( &m_rcDropdown, pt ) )
            {
                // Determine which item has been selected
                for( uint32_t i=m_ScrollBar.GetTrackPos(); i < (uint32_t)m_Items.size(); i++ )
                {
                    ComboBoxItem* pItem = m_Items[i ];
                    if( pItem -> bVisible &&
                        PtInRect( &pItem->rcActive, pt ) )
                    {
                        m_iFocused = m_iSelected = i;
                        _dialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
                        m_bOpened = false;
                        
                        if( !_dialog->m_bKeyboardInput )
                            _dialog->ClearFocus();

                        break;
                    }
                }

                return true;
            }

            // Mouse click not on main control or in dropdown, fire an event if needed
            if( m_bOpened )
            {
                m_iFocused = m_iSelected;

                _dialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
                m_bOpened = false;
            }

            // Make sure the control is no longer in a pressed state
            _pressed = false;

            // Release focus if appropriate
            if( !_dialog->m_bKeyboardInput )
            {
                _dialog->ClearFocus();
            }

            break;
        }

        case WM_LBUTTONUP:
        {
            if( _pressed && containsPoint( pt ) )
            {
                // Button click
                _pressed = false;
                ReleaseCapture();
                return true;
            }

            break;
        }

        case WM_MOUSEWHEEL:
        {
            int zDelta = (short) HIWORD(wParam) / WHEEL_DELTA;
            if( m_bOpened )
            {
                UINT uLines;
                SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
                m_ScrollBar.Scroll( -zDelta * uLines );
            } else
            {
                if( zDelta > 0 )
                {
                    if( m_iFocused > 0 )
                    {
                        m_iFocused--;
                        m_iSelected = m_iFocused;     
                        
                        if( !m_bOpened )
                            _dialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
                    }          
                }
                else
                {
                    if( m_iFocused+1 < (int)GetNumItems() )
                    {
                        m_iFocused++;
                        m_iSelected = m_iFocused;   

                        if( !m_bOpened )
                            _dialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
                    }
                }
            }
            return true;
        }
    };
    
    return false;
}

void ComboBox::hotkey()
{
    if( m_bOpened )
        return;

    if( m_iSelected == -1 )
        return;

    _dialog->RequestFocus( this ); 

    m_iSelected++;
    
    if( m_iSelected >= (int) m_Items.size() )
        m_iSelected = 0;

    m_iFocused = m_iSelected;
    _dialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
}

void ComboBox::render( float elapsedTime )
{
    _CONTROL_STATE iState = _STATE_NORMAL;
    
    if( !m_bOpened )
        iState = _STATE_HIDDEN;

    // Dropdown box
    UIElement* pElement = _elements[2];

    // If we have not initialized the scroll bar page size,
    // do that now.
    static bool bSBInit;
    if( !bSBInit )
    {
        // Update the page size of the scroll bar
        if( GetGlobalDialogResourceManager()->GetFontNode( pElement->iFont )->nHeight )
            m_ScrollBar.SetPageSize( RectHeight( m_rcDropdownText ) / GetGlobalDialogResourceManager()->GetFontNode( pElement->iFont )->nHeight );
        else
            m_ScrollBar.SetPageSize( RectHeight( m_rcDropdownText ) );
        bSBInit = true;
    }

    // Scroll bar
    if( m_bOpened )
        m_ScrollBar.render( elapsedTime );

    // Blend current color
    pElement->TextureColor.Blend( iState, elapsedTime );
    pElement->FontColor.Blend( iState, elapsedTime );

    _dialog->DrawSprite( pElement, &m_rcDropdown );

    // Selection outline
    UIElement* pSelectionElement = _elements[3];
    pSelectionElement->TextureColor.Current = pElement->TextureColor.Current;
    pSelectionElement->FontColor.Current = fromPackedColor(pSelectionElement->FontColor.States[ _STATE_NORMAL ]);

    FontNode* pFont = _dialog->GetFont( pElement->iFont );
    int curY = m_rcDropdownText.top;
    int nRemainingHeight = RectHeight( m_rcDropdownText );
    //WCHAR strDropdown[4096] = {0};

    for( uint32_t i = m_ScrollBar.GetTrackPos(); i < (uint32_t)m_Items.size(); i++ )
    {
        ComboBoxItem* pItem = m_Items[i];

        // Make sure there's room left in the dropdown
        nRemainingHeight -= pFont->nHeight;
        if( nRemainingHeight < 0 )
        {
            pItem->bVisible = false;
            continue;
        }

        SetRect( &pItem->rcActive, m_rcDropdownText.left, curY, m_rcDropdownText.right, curY + pFont->nHeight );
        curY += pFont->nHeight;

        pItem->bVisible = true;

        if( m_bOpened )
        {
            if( (int)i == m_iFocused )
            {
                RECT rc;
                SetRect( &rc, m_rcDropdown.left, pItem->rcActive.top-2, m_rcDropdown.right, pItem->rcActive.bottom+2 );
                _dialog->DrawSprite( pSelectionElement, &rc );
                _dialog->DrawText( pItem->strText, pSelectionElement, &pItem->rcActive );
            }
            else
            {
                _dialog->DrawText( pItem->strText, pElement, &pItem->rcActive );
            }
        }
    }

    int nOffsetX = 0;
    int nOffsetY = 0;

    iState = _STATE_NORMAL;
    
    if( _visible == false )
        iState = _STATE_HIDDEN;
    else if( _enabled == false )
        iState = _STATE_DISABLED;
    else if( _pressed )
    {
        iState = _STATE_PRESSED;

        nOffsetX = 1;
        nOffsetY = 2;
    }
    else if( _mouseOver )
    {
        iState = _STATE_MOUSEOVER;

        nOffsetX = -1;
        nOffsetY = -2;
    }
    else if( _hasFocus )
        iState = _STATE_FOCUS;

    float fBlendRate = ( iState == _STATE_PRESSED ) ? 0.0f : 0.8f;
    
    // Button
    pElement = _elements[1];
    
    // Blend current color
    pElement->TextureColor.Blend( iState, elapsedTime, fBlendRate );
    
    RECT rcWindow = _buttonBounds;
    OffsetRect( &rcWindow, nOffsetX, nOffsetY );
    _dialog->DrawSprite( pElement, &rcWindow );

    if( m_bOpened )
        iState = _STATE_PRESSED;

    // Main text box
    //TODO: remove magic numbers
    pElement = _elements[0 ];
    
    // Blend current color
    pElement->TextureColor.Blend( iState, elapsedTime, fBlendRate );
    pElement->FontColor.Blend( iState, elapsedTime, fBlendRate );

    _dialog->DrawSprite( pElement, &m_rcText);
    
    if( m_iSelected >= 0 && m_iSelected < (int) m_Items.size() )
    {
        ComboBoxItem* pItem = m_Items[m_iSelected ];
        if( pItem != nullptr )
        {
            _dialog->DrawText( pItem->strText, pElement, &m_rcText );
        
        }
    }
}

HRESULT ComboBox::AddItem( const WCHAR* strText, void* pData )
{
    // Validate parameters
    if( strText== nullptr )
    {
        return E_INVALIDARG;
    }
    
    // Create a new item and set the data
    ComboBoxItem* pItem = new ComboBoxItem;
    if( pItem == nullptr )
    {
        return E_FAIL;
    }
    
    memset ( pItem, 0, sizeof(ComboBoxItem) );
    StringCchCopy( pItem->strText, 256, strText );
    pItem->pData = pData;

    m_Items.push_back( pItem );

    // Update the scroll bar with new range
    m_ScrollBar.SetTrackRange( 0, (int)m_Items.size() );

    // If this is the only item in the list, it's selected
    if( GetNumItems() == 1 )
    {
        m_iSelected = 0;
        m_iFocused = 0;
        _dialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, false, this );
    }

    return S_OK;
}

void ComboBox::RemoveItem( UINT index )
{
    ComboBoxItem* pItem = m_Items[index ];
    safedelete( pItem );
//    m_Items.Remove( index );
    //matt, removal problem, return to.
            //  m_Controls.erase();
    m_ScrollBar.SetTrackRange( 0, (int)m_Items.size() );
    if( m_iSelected >= (int)m_Items.size() )
        m_iSelected = (uint32_t)(m_Items.size() - 1);
}

void ComboBox::RemoveAllItems()
{
    for( uint32_t i=0; i < (uint32_t)m_Items.size(); i++ )
    {
        ComboBoxItem* pItem = m_Items[i];
        safedelete( pItem );
    }

    m_Items.clear();
    m_ScrollBar.SetTrackRange( 0, 1 );
    m_iFocused = m_iSelected = -1;
}


bool ComboBox::ContainsItem( const WCHAR* strText, UINT iStart )
{
    return ( -1 != FindItem( strText, iStart ) );
}

int ComboBox::FindItem( const WCHAR* strText, UINT iStart )
{
    if( strText == nullptr )
        return -1;

    for( uint32_t i = iStart; i < (uint32_t)m_Items.size(); i++ )
    {
        ComboBoxItem* pItem = m_Items[i];

        if( 0 == wcscmp( pItem->strText, strText ) )
        {
            return i;
        }
    }

    return -1;
}

void* ComboBox::GetSelectedData()
{
    if( m_iSelected < 0 )
        return nullptr;

    ComboBoxItem* pItem = m_Items[m_iSelected];
    return pItem->pData;
}

ComboBoxItem* ComboBox::GetSelectedItem()
{
    if( m_iSelected < 0 )
        return nullptr;

    return m_Items[m_iSelected ];
}

void* ComboBox::GetItemData( const WCHAR* strText )
{
    int index = FindItem( strText );
    if( index == -1 )
    {
        return nullptr;
    }

    ComboBoxItem* pItem = m_Items[index];
    if( pItem == nullptr )
    {
        return nullptr;
    }

    return pItem->pData;
}

void* ComboBox::GetItemData( int nIndex )
{
    if( nIndex < 0 || nIndex >= (int)m_Items.size() )
        return nullptr;

    return m_Items[nIndex]->pData;
}

HRESULT ComboBox::SetSelectedByIndex( UINT index )
{
    if( index >= GetNumItems() )
        return E_INVALIDARG;

    m_iFocused = m_iSelected = index;
    _dialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, false, this );

    return S_OK;
}

HRESULT ComboBox::SetSelectedByText( const WCHAR* strText )
{
    if( strText == nullptr )
        return E_INVALIDARG;

    int index = FindItem( strText );
    if( index == -1 )
        return E_FAIL;

    m_iFocused = m_iSelected = index;
    _dialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, false, this );

    return S_OK;
}

HRESULT ComboBox::SetSelectedByData( void* pData )
{
    for( uint32_t i=0; i < (uint32_t)m_Items.size(); i++ )
    {
        ComboBoxItem* pItem = m_Items[i];

        if( pItem->pData == pData )
        {
            m_iFocused = m_iSelected = i;
            _dialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, false, this );
            return S_OK;
        }
    }

    return E_FAIL;
}

HRESULT 
ComboBox::init() 
{ 
    return _dialog->initControl( &m_ScrollBar ); 
}
}