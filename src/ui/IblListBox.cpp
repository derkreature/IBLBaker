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

#include <IblListBox.h>
#include <IblWidgetDialog.h>
#include <IblDialogResourceManager.h>
#include <IblWidgetTypes.h>

namespace Ibl
{
ListBox::ListBox (Dialog *dialog, Ibl::IDevice* renderMgr) :
    Control (dialog, renderMgr),
    m_ScrollBar( dialog, renderMgr )
{
    _type = _CONTROL_LISTBOX;
 
    m_dwStyle = 0;
    m_nSBWidth = 16;
    m_nSelected = -1;
    m_nSelStart = 0;
    m_bDrag = false;
    m_nBorder = 6;
    m_nMargin = 5;
    m_nTextHeight = 0;
}


//--------------------------------------------------------------------------------------
ListBox::~ListBox()
{
    RemoveAllItems();
}


//--------------------------------------------------------------------------------------
void ListBox::updateRects()
{
    Control::updateRects();

    m_rcSelection = _boundingBox;
    m_rcSelection.right -= m_nSBWidth;
    InflateRect( &m_rcSelection, -m_nBorder, -m_nBorder );
    m_rcText = m_rcSelection;
    InflateRect( &m_rcText, -m_nMargin, 0 );

    // Update the scrollbar's rects
    m_ScrollBar.setLocation( _boundingBox.right - m_nSBWidth, _boundingBox.top );
    m_ScrollBar.setSize( m_nSBWidth, _height );
    FontNode* pFontNode = GetGlobalDialogResourceManager()->GetFontNode( _elements[0 ]->iFont );
    if( pFontNode && pFontNode->nHeight )
    {
        m_ScrollBar.SetPageSize( RectHeight( m_rcText ) / pFontNode->nHeight );

        // The selected item may have been scrolled off the page.
        // Ensure that it is in page again.
        m_ScrollBar.ShowItem( m_nSelected );
    }
}


//--------------------------------------------------------------------------------------
HRESULT ListBox::AddItem( const WCHAR *wszText, void *pData )
{
    HRESULT hr = S_OK;
    ListBoxItem *pNewItem = new ListBoxItem;
    if( !pNewItem )
        return E_OUTOFMEMORY;

    StringCchCopy( pNewItem->strText, 256, wszText );
    pNewItem->pData = pData;
    SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
    pNewItem->bSelected = false;

    m_Items.push_back( pNewItem );
    
      m_ScrollBar.SetTrackRange( 0, (int)m_Items.size() );
    

    return hr;
}


//--------------------------------------------------------------------------------------
HRESULT ListBox::InsertItem( int nIndex, const WCHAR *wszText, void *pData )
{
    ListBoxItem *pNewItem = new ListBoxItem;
    if( !pNewItem )
        return E_OUTOFMEMORY;

    StringCchCopy( pNewItem->strText, 256, wszText );
    pNewItem->pData = pData;
    SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
    pNewItem->bSelected = false;

    HRESULT hr = S_OK;
    m_Items[ nIndex] =pNewItem ;
    if( SUCCEEDED( hr ) )
        m_ScrollBar.SetTrackRange( 0, (int)m_Items.size() );
    else
        safedelete( pNewItem );

    return hr;
}


//--------------------------------------------------------------------------------------
void ListBox::RemoveItem( int nIndex )
{
    if( nIndex < 0 || nIndex >= (int)m_Items.size() )
        return;

    ListBoxItem *pItem = m_Items[nIndex ];

    delete pItem;
    //matt, removal problem, return to.
            //  m_Controls.erase();
 //   m_Items.erase( nIndex );
    m_ScrollBar.SetTrackRange( 0, (int)m_Items.size() );

    if( m_nSelected >= (uint32_t)m_Items.size() )
        m_nSelected = (uint32_t)m_Items.size() - 1;

    _dialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
}


//--------------------------------------------------------------------------------------
void ListBox::RemoveItemByText( WCHAR *wszText )
{
}


//--------------------------------------------------------------------------------------
void ListBox::RemoveItemByData( void *pData )
{
}


//--------------------------------------------------------------------------------------
void ListBox::RemoveAllItems()
{
    for( uint32_t i = 0; i < (uint32_t)m_Items.size(); ++i )
    {
        ListBoxItem *pItem = m_Items[i];
        delete pItem;
    }

    m_Items.clear();
    m_ScrollBar.SetTrackRange( 0, 1 );
}


//--------------------------------------------------------------------------------------
ListBoxItem *ListBox::GetItem( int nIndex )
{
    if( nIndex < 0 || nIndex >= (int)m_Items.size() )
        return nullptr;

    return m_Items[nIndex];
}


//--------------------------------------------------------------------------------------
// For single-selection listbox, returns the index of the selected item.
// For multi-selection, returns the first selected item after the nPreviousSelected position.
// To search for the first selected item, the app passes -1 for nPreviousSelected.  For
// subsequent searches, the app passes the returned index back to GetSelectedIndex as.
// nPreviousSelected.
// Returns -1 on error or if no item is selected.
int ListBox::GetSelectedIndex( int nPreviousSelected )
{
    if( nPreviousSelected < -1 )
        return -1;

    if( m_dwStyle & MULTISELECTION )
    {
        // Multiple selection enabled. Search for the next item with the selected flag.
        for( int i = nPreviousSelected + 1; i < (int)m_Items.size(); ++i )
        {
            ListBoxItem *pItem = m_Items[i ];

            if( pItem->bSelected )
                return i;
        }

        return -1;
    }
    else
    {
        // Single selection
        return m_nSelected;
    }
}


//--------------------------------------------------------------------------------------
void ListBox::SelectItem( int nNewIndex )
{
    // If no item exists, do nothing.
    if( m_Items.size() == 0 )
        return;

    int nOldSelected = m_nSelected;

    // Adjust m_nSelected
    m_nSelected = nNewIndex;

    // Perform capping
    if( m_nSelected < 0 )
        m_nSelected = 0;
    if( m_nSelected >= (uint32_t)m_Items.size() )
        m_nSelected = (uint32_t)m_Items.size() - 1;

    if( nOldSelected != m_nSelected )
    {
        if( m_dwStyle & MULTISELECTION )
        {
            m_Items[m_nSelected]->bSelected = true;
        }

        // Update selection start
        m_nSelStart = m_nSelected;

        // Adjust scroll bar
        m_ScrollBar.ShowItem( m_nSelected );
    }

    _dialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
}


//--------------------------------------------------------------------------------------
bool ListBox::handleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( !_enabled || !_visible)
        return false;

    // Let the scroll bar have a chance to handle it first
    if( m_ScrollBar.handleKeyboard( uMsg, wParam, lParam ) )
        return true;

    switch( uMsg )
    {
        case WM_KEYDOWN:
            switch( wParam )
            {
                case VK_UP:
                case VK_DOWN:
                case VK_NEXT:
                case VK_PRIOR:
                case VK_HOME:
                case VK_END:

                    // If no item exists, do nothing.
                    if( m_Items.size() == 0 )
                        return true;

                    int nOldSelected = m_nSelected;

                    // Adjust m_nSelected
                    switch( wParam )
                    {
                        case VK_UP: --m_nSelected; break;
                        case VK_DOWN: ++m_nSelected; break;
                        case VK_NEXT: m_nSelected += m_ScrollBar.GetPageSize() - 1; break;
                        case VK_PRIOR: m_nSelected -= m_ScrollBar.GetPageSize() - 1; break;
                        case VK_HOME: m_nSelected = 0; break;
                        case VK_END: m_nSelected = (uint32_t)(m_Items.size() - 1); break;
                    }

                    // Perform capping
                    if( m_nSelected < 0 )
                        m_nSelected = 0;
                    if( m_nSelected >= (uint32_t)m_Items.size() )
                        m_nSelected = (uint32_t)m_Items.size() - 1;

                    if( nOldSelected != m_nSelected )
                    {
                        if( m_dwStyle & MULTISELECTION )
                        {
                            // Multiple selection

                            // Clear all selection
                            for( int i = 0; i < (int)m_Items.size(); ++i )
                            {
                                ListBoxItem *pItem = m_Items[i];
                                pItem->bSelected = false;
                            }

                            if( GetKeyState( VK_SHIFT ) < 0 )
                            {
                                // Select all items from m_nSelStart to
                                // m_nSelected
                                int nEnd = Ibl::Limits<int>::maxVal ( m_nSelStart, m_nSelected );

                                for( int n = Ibl::Limits<int>::minVal ( m_nSelStart, m_nSelected ); n <= nEnd; ++n )
                                    m_Items[n]->bSelected = true;
                            }
                            else
                            {
                                m_Items[m_nSelected]->bSelected = true;

                                // Update selection start
                                m_nSelStart = m_nSelected;
                            }
                        } else
                            m_nSelStart = m_nSelected;

                        // Adjust scroll bar

                        m_ScrollBar.ShowItem( m_nSelected );

                        // Send notification

                        _dialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
                    }
                    return true;
            }
            break;
    }

    return false;
}


//--------------------------------------------------------------------------------------
bool ListBox::handleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    if( !_enabled || !_visible )
        return false;

    // First acquire focus
    if( WM_LBUTTONDOWN == uMsg )
        if( !_hasFocus )
            _dialog->RequestFocus( this );

    // Let the scroll bar handle it first.
    if( m_ScrollBar.handleMouse( uMsg, pt, wParam, lParam ) )
        return true;

    switch( uMsg )
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
            // Check for clicks in the text area
            if( m_Items.size() > 0 && PtInRect( &m_rcSelection, pt ) )
            {
                // Compute the index of the clicked item

                int nClicked;
                if( m_nTextHeight )
                    nClicked = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
                else
                    nClicked = -1;

                // Only proceed if the click falls on top of an item.

                if( nClicked >= m_ScrollBar.GetTrackPos() &&
                    nClicked < (int)m_Items.size() &&
                    nClicked < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
                {
                    SetCapture (_dialog->windowHandle());
                    m_bDrag = true;

                    // If this is a double click, fire off an event and exit
                    // since the first click would have taken care of the selection
                    // updating.
                    if( uMsg == WM_LBUTTONDBLCLK )
                    {
                        _dialog->SendEvent( EVENT_LISTBOX_ITEM_DBLCLK, true, this );
                        return true;
                    }

                    m_nSelected = nClicked;
                    if( !( wParam & MK_SHIFT ) )
                        m_nSelStart = m_nSelected;

                    // If this is a multi-selection listbox, update per-item
                    // selection data.

                    if( m_dwStyle & MULTISELECTION )
                    {
                        // Determine behavior based on the state of Shift and Ctrl

                        ListBoxItem *pSelItem = m_Items[m_nSelected ];
                        if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == MK_CONTROL )
                        {
                            // Control click. Reverse the selection of this item.

                            pSelItem->bSelected = !pSelItem->bSelected;
                        } else
                        if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == MK_SHIFT )
                        {
                            // Shift click. Set the selection for all items
                            // from last selected item to the current item.
                            // Clear everything else.

                            int nBegin = Ibl::Limits<int>::minVal ( m_nSelStart, m_nSelected );
                            int nEnd = Ibl::Limits<int>::maxVal ( m_nSelStart, m_nSelected );

                            for( int i = 0; i < nBegin; ++i )
                            {
                                ListBoxItem *pItem = m_Items[i ];
                                pItem->bSelected = false;
                            }

                            for( int i = nEnd + 1; i < (int)m_Items.size(); ++i )
                            {
                                ListBoxItem *pItem = m_Items[i];
                                pItem->bSelected = false;
                            }

                            for( int i = nBegin; i <= nEnd; ++i )
                            {
                                ListBoxItem *pItem = m_Items[i ];
                                pItem->bSelected = true;
                            }
                        } else
                        if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == ( MK_SHIFT|MK_CONTROL ) )
                        {
                            // Control-Shift-click.

                            // The behavior is:
                            //   Set all items from m_nSelStart to m_nSelected to
                            //     the same state as m_nSelStart, not including m_nSelected.
                            //   Set m_nSelected to selected.

                            int nBegin = Ibl::Limits<int>::minVal ( m_nSelStart, m_nSelected );
                            int nEnd = Ibl::Limits<int>::maxVal ( m_nSelStart, m_nSelected );

                            // The two ends do not need to be set here.

                            bool bLastSelected = m_Items[m_nSelStart ]->bSelected;
                            for( int i = nBegin + 1; i < nEnd; ++i )
                            {
                                ListBoxItem *pItem = m_Items[i ];
                                pItem->bSelected = bLastSelected;
                            }

                            pSelItem->bSelected = true;

                            // Restore m_nSelected to the previous value
                            // This matches the Windows behavior

                            m_nSelected = m_nSelStart;
                        } else
                        {
                            // Simple click.  Clear all items and select the clicked
                            // item.


                            for( int i = 0; i < (int)m_Items.size(); ++i )
                            {
                                ListBoxItem *pItem = m_Items[i ];
                                pItem->bSelected = false;
                            }

                            pSelItem->bSelected = true;
                        }
                    }  // End of multi-selection case

                    _dialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
                }

                return true;
            }
            break;

        case WM_LBUTTONUP:
        {
            ReleaseCapture();
            m_bDrag = false;

            if( m_nSelected != -1 )
            {
                // Set all items between m_nSelStart and m_nSelected to
                // the same state as m_nSelStart
                int nEnd = Ibl::Limits<int>::maxVal ( m_nSelStart, m_nSelected );

                for( int n = Ibl::Limits<int>::minVal ( m_nSelStart, m_nSelected ) + 1; n < nEnd; ++n )
                    m_Items[n]->bSelected = m_Items[m_nSelStart]->bSelected;
                m_Items[m_nSelected]->bSelected = m_Items[m_nSelStart]->bSelected;

                // If m_nSelStart and m_nSelected are not the same,
                // the user has dragged the mouse to make a selection.
                // Notify the application of this.
                if( m_nSelStart != m_nSelected )
                    _dialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
            }
            return false;
        }

        case WM_MOUSEMOVE:
            if( m_bDrag )
            {
                // Compute the index of the item below cursor

                int nItem;
                if( m_nTextHeight )
                    nItem = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
                else
                    nItem = -1;

                // Only proceed if the cursor is on top of an item.

                if( nItem >= (int)m_ScrollBar.GetTrackPos() &&
                    nItem < (int)m_Items.size() &&
                    nItem < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
                {
                    m_nSelected = nItem;
                    _dialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
                } else
                if( nItem < (int)m_ScrollBar.GetTrackPos() )
                {
                    // User drags the mouse above window top
                    m_ScrollBar.Scroll( -1 );
                    m_nSelected = m_ScrollBar.GetTrackPos();
                    _dialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
                } else
                if( nItem >= m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
                {
                    // User drags the mouse below window bottom
                    m_ScrollBar.Scroll( 1 );
                    m_nSelected = Ibl::Limits<int>::minVal ( (int)m_Items.size(), m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() ) - 1;
                    _dialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
                }
            }
            break;

        case WM_MOUSEWHEEL:
        {
            UINT uLines;
            SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
            int nScrollAmount = int((short)HIWORD(wParam)) / WHEEL_DELTA * uLines;
            m_ScrollBar.Scroll( -nScrollAmount );
            return true;
        }
    }

    return false;
}


//--------------------------------------------------------------------------------------
void ListBox::render (float elapsedTime)
{
    if( _visible == false )
        return;

    UIElement* pElement = _elements[0 ];
    pElement->TextureColor.Blend( _STATE_NORMAL, elapsedTime );
    pElement->FontColor.Blend( _STATE_NORMAL, elapsedTime );

    UIElement* pSelElement = _elements[1 ];
    pSelElement->TextureColor.Blend( _STATE_NORMAL, elapsedTime );
    pSelElement->FontColor.Blend( _STATE_NORMAL, elapsedTime );

    _dialog->DrawSprite( pElement, &_boundingBox );

    // render the text
    if( m_Items.size() > 0 )
    {
        // Find out the height of a single line of text
        RECT rc = m_rcText;
        RECT rcSel = m_rcSelection;
        rc.bottom = rc.top + GetGlobalDialogResourceManager()->GetFontNode( pElement->iFont )->nHeight;

        // Update the line height formation
        m_nTextHeight = rc.bottom - rc.top;

        static bool bSBInit;
        if( !bSBInit )
        {
            // Update the page size of the scroll bar
            if( m_nTextHeight )
                m_ScrollBar.SetPageSize( RectHeight( m_rcText ) / m_nTextHeight );
            else
                m_ScrollBar.SetPageSize( RectHeight( m_rcText ) );
            bSBInit = true;
        }

        rc.right = m_rcText.right;
        for( uint32_t i = m_ScrollBar.GetTrackPos(); i < (uint32_t)m_Items.size(); ++i )
        {
            if( rc.bottom > m_rcText.bottom )
                break;

            ListBoxItem *pItem = m_Items[i ];

            // Determine if we need to render this item with the
            // selected element.
            bool bSelectedStyle = false;

            if( !( m_dwStyle & MULTISELECTION ) && i == m_nSelected )
                bSelectedStyle = true;
            else
            if( m_dwStyle & MULTISELECTION )
            {
                if( m_bDrag &&
                    ( ( i >= m_nSelected && i < m_nSelStart ) ||
                      ( i <= m_nSelected && i > m_nSelStart ) ) )
                    bSelectedStyle = m_Items[m_nSelStart]->bSelected;
                else
                if( pItem->bSelected )
                    bSelectedStyle = true;
            }

            if( bSelectedStyle )
            {
                rcSel.top = rc.top; rcSel.bottom = rc.bottom;
                _dialog->DrawSprite( pSelElement, &rcSel );
                _dialog->DrawText( pItem->strText, pSelElement, &rc );
            }
            else
                _dialog->DrawText( pItem->strText, pElement, &rc );

            OffsetRect( &rc, 0, m_nTextHeight );
        }
    }

    // render the scroll bar
    m_ScrollBar.render (elapsedTime );
}

HRESULT 
ListBox::init() 
{ 
    return _dialog->initControl( &m_ScrollBar ); 
}

}