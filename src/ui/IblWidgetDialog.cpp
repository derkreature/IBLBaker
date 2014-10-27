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


#include <IblWidgetDialog.h>
#include <IblDialogResourceManager.h>
#include <IblControl.h>
#include <IblStatic.h>
#include <IblButton.h>
#include <IblCheckBox.h>
#include <IblRadioButton.h>
#include <IblComboBox.h>
#include <IblEditBox.h>
#include <IblSlider.h>
#include <IblListBox.h>
#include <IblImageWidget.h>
#include <IblIDevice.h>
#include <IblLog.h>
#include <IblTextureMgr.h>
#include <IblSprite.h>
#include <IblFontMgr.h>
#include <IblMatrix44.h>
#include <IblVector3.h>
#include <IblCamera.h>

namespace Ibl
{
float           currentZ;

double          Dialog::s_fTimerefresh = 0.0f;
Control*        Dialog::s_pControlFocus = nullptr;
Control*        Dialog::s_pControlPressed = nullptr;

Dialog::Dialog (Ibl::Window* parentWindow, 
                Ibl::IDevice* device) :
Ibl::IRenderResource (device),
_parentWindow(parentWindow),
_currentCamera (nullptr)
{
    EditBox::UniBuffer::InitializeUniscribe();

    _parentWindow->addWindowDependency(this);

    GetGlobalDialogResourceManager()->setAutomation(true);
    GetGlobalDialogResourceManager()->create(device);
    GetGlobalDialogResourceManager()->reset();

    _x = 0;
    _y = 0;
    _width = 0;
    _height = 0;

    _visible = true;
    _caption = false;
    m_bMinimized = false;
    m_wszCaption[0] = L'\0';
    m_nCaptionHeight = 18;

    m_colorTopLeft = 0;
    m_colorTopRight = 0;
    m_colorBottomLeft = 0;
    m_colorBottomRight = 0;

    m_pCallbackEvent = nullptr;

    m_fTimeLastrefresh = 0;

    m_pControlMouseOver = nullptr;

    m_pNextDialog = this;
    m_pPrevDialog = this;

    m_nDefaultControlID = 0xffff;
    m_bNonUserEvents = false;
    m_bKeyboardInput = false;
    m_bMouseInput = true;

    InitDefaultElements();
}

bool Dialog::dialogHasFocus()
{
    return s_pControlFocus != 0;
}

bool Dialog::free()
{
    GetGlobalDialogResourceManager()->lost();
    return true;
}

bool Dialog::create()
{    
    GetGlobalDialogResourceManager()->reset();
    return true;
}

bool Dialog::cache()
{
    return true;
}

void Dialog::SetrefreshTime( float fTime ){ s_fTimerefresh = fTime; }

Dialog::~Dialog()
{
    if (Ibl::Window * window = _parentWindow)
    {
        window->removeWindowDependency(this);
    }

    int i=0;
    RemoveAllControls();
    m_Fonts.clear();
    m_Textures.clear();

    for (unsigned int i = 0; i < m_DefaultElements.size(); i++ )
    {
        ElementHolder* pElementHolder = m_DefaultElements[i];
        safedelete (pElementHolder);
    }

    m_DefaultElements.clear();
}

void Dialog::RemoveControl( int ID )
{
    std::vector<Control*>::iterator controliterator;
    for (controliterator = m_Controls.begin(); 
         controliterator != m_Controls.end();
         controliterator++)
    {
        Control* pControl = (*controliterator);

        if( pControl->id() == ID )
        {
            // Clean focus first
            ClearFocus();

            // Clear references to this control
            if( s_pControlFocus == pControl )
                s_pControlFocus = nullptr;
            if( s_pControlPressed == pControl )
                s_pControlPressed = nullptr;
            if( m_pControlMouseOver == pControl )
                m_pControlMouseOver = nullptr;

            safedelete( pControl );
            //matt, removal problem, return to.
            m_Controls.erase(controliterator);
            return;
        }
    }
}

void Dialog::RemoveAllControls()
{
    if (s_pControlFocus && s_pControlFocus->_dialog == this)
        s_pControlFocus = nullptr;
    if (s_pControlPressed && s_pControlPressed->_dialog == this)
        s_pControlPressed = nullptr;
    m_pControlMouseOver = nullptr;

    for (unsigned int i=0; i < m_Controls.size(); i++ )
    {
        Control* pControl = m_Controls[i ];
        safedelete( pControl );
    }

    m_Controls.clear();
}

void Dialog::refresh()
{
    if( s_pControlFocus )
        s_pControlFocus->focusOut();

    if( m_pControlMouseOver )
        m_pControlMouseOver->mouseLeave();

    s_pControlFocus = nullptr;
    s_pControlPressed = nullptr;
    m_pControlMouseOver = nullptr;

    for( uint32_t i=0; i < (uint32_t)m_Controls.size(); i++ )
    {
        Control* pControl = m_Controls[i];
        pControl->refresh();
    }

    if( m_bKeyboardInput )
        FocusDefaultControl();
}

HRESULT 
Dialog::render(float elapsedTime, const Ibl::Camera* camera)
{   
    _currentCamera = camera;

    // See if the dialog needs to be refreshed
    if( m_fTimeLastrefresh < s_fTimerefresh )
    {
        refresh();
    }

    // For invisible dialog, out now.
    //if( !_visible )
     //   return S_OK;

   _deviceInterface->setAlphaToCoverageEnable (false);
   _deviceInterface->enableAlphaBlending();
   _deviceInterface->setSrcFunction (Ibl::SourceAlpha);
   _deviceInterface->setDestFunction (Ibl::InverseSourceAlpha);
   _deviceInterface->setBlendProperty (Ibl::OpAdd);
   _deviceInterface->setAlphaSrcFunction (Ibl::BlendOne);
   _deviceInterface->setAlphaDestFunction (Ibl::BlendZero);
   _deviceInterface->setAlphaBlendProperty(Ibl::OpAdd);
    
    _deviceInterface->setAlphaSrcFunction (Ibl::SourceAlpha);
    _deviceInterface->setAlphaDestFunction (Ibl::InverseSourceAlpha);
    _deviceInterface->setNullVertexShader();
    _deviceInterface->setNullPixelShader();
    _deviceInterface->enableDepthWrite();
    _deviceInterface->disableZTest();
    //_deviceInterface->disableDepthWrite();
    
    // If the dialog is minimized, skip rendering
    // its controls.
    if( !m_bMinimized )
    {
        for( uint32_t i=0; i < (uint32_t)m_Controls.size(); i++ )
        {
            Control* control = m_Controls[i];   

            // Focused control is drawn last
           // if( pControl != s_pControlFocus )
            // if (control->usesSpriteSystem())
            //    GetGlobalDialogResourceManager()->sprite()->begin( );

               control->render(elapsedTime );         

            // if (control->usesSpriteSystem())
            //   GetGlobalDialogResourceManager()->sprite()->end();
        }

        //if( s_pControlFocus != nullptr && s_pControlFocus->_dialog == this)
         //   s_pControlFocus->render( elapsedTime );
    }

    _deviceInterface->enableDepthWrite();
    _deviceInterface->disableAlphaBlending();
    return S_OK;
}

void Dialog::resetZ()
{
    currentZ = 1.0f;
}

VOID Dialog::SendEvent( UINT nEvent, bool bTriggeredByUser, Control* pControl )
{
    // If no callback has been registered there's nowhere to send the event to
    if( m_pCallbackEvent == nullptr )
        return;

    // Discard events triggered programatically if these types of events haven't been
    // enabled
    if( !bTriggeredByUser && !m_bNonUserEvents )
        return;

    m_pCallbackEvent( nEvent, pControl->id(), pControl );
}

HRESULT Dialog::SetFont( UINT index, const char* strFaceName, LONG height, LONG weight )
{
    int iFont = GetGlobalDialogResourceManager()->AddFont( strFaceName, height, weight );
  
    if(index < m_Fonts.size())
        m_Fonts[index] = iFont;
    else
        m_Fonts.push_back(iFont);

    return S_OK;
}

FontNode* Dialog::GetFont( UINT index )
{
    if( nullptr == GetGlobalDialogResourceManager() )
        return nullptr;

    return GetGlobalDialogResourceManager()->GetFontNode( m_Fonts[index ] );

    /*
    if (index < m_Fonts.size())
    {
        return GetGlobalDialogResourceManager()->GetFontNode( m_Fonts[index ] );
    }
    else
    {
        // Quick guard, I've screwed something up.
        return GetGlobalDialogResourceManager()->GetFontNode( m_Fonts[m_Fonts.size()-1 ] );
    }
    */
}

int DialogResourceManager::AddTexture( const char* strFilename )
{
    // See if this texture already exists
    for( uint32_t i=0; i < (uint32_t)m_TextureCache.size(); i++ )
    {
        TextureNode* pTextureNode = m_TextureCache[i];
        if ( std::string(pTextureNode->filename()) ==  std::string(strFilename))
        {
            return i;
        }
    }

    // Add a new texture and try to create it
    TextureNode* pNewTextureNode = new TextureNode();
    if( pNewTextureNode == nullptr )
        return -1;

    memset ( pNewTextureNode, 0, sizeof(TextureNode) );
    pNewTextureNode->setFileName ( strFilename );
    m_TextureCache.push_back( pNewTextureNode );
    
    uint32_t iTexture = (uint32_t)(m_TextureCache.size()-1);

    CreateTexture( iTexture );

    return iTexture;
}

HRESULT Dialog::SetTexture (const char* strFilename)
{
    unsigned int index = 0;
    // Make sure the list is at least as large as the index being set   
    int iTexture = GetGlobalDialogResourceManager()->AddTexture( strFilename );
    
    if(index < m_Textures.size())
    {
        m_Textures[index] = iTexture;
    }
    else
    {
        m_Textures.push_back( iTexture );
    
    }
    return S_OK;
}

TextureNode* Dialog::GetTexture( UINT index )
{
    if( nullptr == GetGlobalDialogResourceManager() )
        return nullptr;
    return GetGlobalDialogResourceManager()->GetTextureNode( m_Textures[index ] );
}

bool
Dialog::handleWindowChanged(Ibl::WindowData* windowData)
{
    bool bHandled = false;
    {
        HWND hWnd = windowData->_window; 
        UINT uMsg = windowData->_msg;
        WPARAM wParam = windowData->_wparam; 
        LPARAM lParam = windowData->_lparam;

        // For invisible dialog, do not handle anything.
        if( !_visible )
            return false;

        // If automation command-line switch is on, enable this dialog's keyboard input
        // upon any key press or mouse click.
        if(( WM_LBUTTONDOWN == uMsg || WM_LBUTTONDBLCLK == uMsg || WM_KEYDOWN == uMsg ) )
        {
            m_bKeyboardInput = true;
        }

        // If caption is enable, check for clicks in the caption area.
        if( _caption )
        {
            static bool bDrag;

            if( uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK )
            {
                POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };

                if( mousePoint.x >= _x && mousePoint.x < _x + _width &&
                    mousePoint.y >= _y && mousePoint.y < _y + m_nCaptionHeight )
                {
                    bDrag = true;
                    SetCapture(windowHandle());
                    return true;
                }
            } else
            if( uMsg == WM_LBUTTONUP && bDrag )
            {
                POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };

                if( mousePoint.x >= _x && mousePoint.x < _x + _width &&
                    mousePoint.y >= _y && mousePoint.y < _y + m_nCaptionHeight )
                {
                    ReleaseCapture();
                    bDrag = false;
                    m_bMinimized = !m_bMinimized;
                    return true;
                }
            }
        }

        // If the dialog is minimized, don't send any messages to controls.
        if( m_bMinimized )
            return false;

        // If a control is in focus, it belongs to this dialog, and it's enabled, then give
        // it the first chance at handling the message.
        if( s_pControlFocus && 
            s_pControlFocus->_dialog == this && 
            s_pControlFocus->enabled() )
        {
            // If the control msgProc handles it, then we don't.
            if( s_pControlFocus->msgProc( uMsg, wParam, lParam ) )
                return true;
        }

        switch( uMsg )
        {
            case WM_ACTIVATEAPP:
                // Call focusIn()/focusOut() of the control that currently has the focus
                // as the application is activated/deactivated.  This matches the Windows
                // behavior.
                if( s_pControlFocus && 
                    s_pControlFocus->_dialog == this && 
                    s_pControlFocus->enabled() )
                {
                    if( wParam )
                        s_pControlFocus->focusIn();
                    else
                        s_pControlFocus->focusOut();
                }
                break;

            // Keyboard messages
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYUP:
            {
                // If a control is in focus, it belongs to this dialog, and it's enabled, then give
                // it the first chance at handling the message.
                if( s_pControlFocus && 
                    s_pControlFocus->_dialog == this && 
                    s_pControlFocus->enabled() )
                {
                    if( s_pControlFocus->handleKeyboard( uMsg, wParam, lParam ) )
                        return true;
                }

                // Not yet handled, see if this matches a control's hotkey
                // Activate the hotkey if the focus doesn't belong to an
                // edit box.
                if( uMsg == WM_KEYDOWN && ( !s_pControlFocus ||
                                          ( s_pControlFocus->getType() != _CONTROL_EDITBOX
                                         && s_pControlFocus->getType() != _CONTROL_IMEEDITBOX ) ) )
                {
                    for( uint32_t i=0; i < (uint32_t)(m_Controls.size()); i++ )
                    {
                        Control* pControl = m_Controls[i ];
                        if( pControl->hotkey() == wParam )
                        {
                            pControl->hotkey();
                            return true;
                        }
                    }
                }

                // Not yet handled, check for focus messages
                if( uMsg == WM_KEYDOWN )
                {
                    // If keyboard input is not enabled, this message should be ignored
                    if( !m_bKeyboardInput )
                        return false;

                    switch( wParam )
                    {
                        case VK_RIGHT:
                        case VK_DOWN:
                            if( s_pControlFocus != nullptr )
                            {
                                return OnCycleFocus( true );
                            }
                            break;

                        case VK_LEFT:
                        case VK_UP:
                            if( s_pControlFocus != nullptr )
                            {
                                return OnCycleFocus( false );
                            }
                            break;

                        case VK_TAB: 
                            if( s_pControlFocus == nullptr )
                            {
                                FocusDefaultControl();
                            }
                            else
                            {
                                bool bShiftDown = ((GetAsyncKeyState( VK_SHIFT ) & 0x8000) != 0);
                                return OnCycleFocus( !bShiftDown );
                            }
                            return true;
                    }
                }

                break;
            }

            // Mouse messages
            case WM_MOUSEMOVE:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_XBUTTONDOWN:
            case WM_XBUTTONUP:
            case WM_LBUTTONDBLCLK:
            case WM_MBUTTONDBLCLK:
            case WM_RBUTTONDBLCLK:
            case WM_XBUTTONDBLCLK:
            case WM_MOUSEWHEEL:
            {
                // If not accepting mouse input, return false to indicate the message should still 
                // be handled by the application (usually to move the camera).

                //if( !m_bMouseInput )
                //    return false;

                POINT mousePoint;// = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
                GetCursorPos(&mousePoint);    
                ScreenToClient(windowHandle(), &mousePoint);

                //mousePoint.x -= _x;
                //mousePoint.y -= _y;

                // If caption is enabled, offset the Y coordinate by the negative of its height.
                if( _caption )
                    mousePoint.y += (m_nCaptionHeight * 2);

                // If a control is in focus, it belongs to this dialog, and it's enabled, then give
                // it the first chance at handling the message.
                if( s_pControlFocus && 
                    s_pControlFocus->_dialog == this && 
                    s_pControlFocus->enabled() )
                {
                    if( s_pControlFocus->handleMouse( uMsg, mousePoint, wParam, lParam ) )
                        return true;
                }

                // Not yet handled, see if the mouse is over any controls
                Control* pControl = getControlAtPoint( mousePoint );
                if( pControl != nullptr && pControl->enabled() )
                {
                    bHandled = pControl->handleMouse( uMsg, mousePoint, wParam, lParam );
                    if( bHandled )
                        return true;
                }
                else
                {
                    // Mouse not over any controls in this dialog, if there was a control
                    // which had focus it just lost it
                    if( uMsg == WM_LBUTTONDOWN && 
                        s_pControlFocus && 
                        s_pControlFocus->_dialog == this )
                    {
                        s_pControlFocus->focusOut();
                        s_pControlFocus = nullptr;
                    }
                }

                // Still not handled, hand this off to the dialog. Return false to indicate the
                // message should still be handled by the application (usually to move the camera).
                switch( uMsg )
                {
                    case WM_MOUSEMOVE:
                        OnMouseMove( mousePoint );
                        return false;
                }

                break;
            }
        }
    }

    return false;
}

Control* Dialog::getControlAtPoint( POINT pt )
{
    // Search through all child controls for the first one which
    // contains the mouse point
    for( uint32_t i=0; i < (uint32_t)m_Controls.size(); i++ )
    {
        Control* pControl = m_Controls[i];

        if( pControl == nullptr )
        {
            continue;
        }

        // We only return the current control if it is visible
        // and enabled.  Because getControlAtPoint() is used to do mouse
        // hittest, it makes sense to perform this filtering.
        if( pControl->containsPoint( pt ) && pControl->enabled() && pControl->visible() )
        {
            return pControl;
        }
    }

    return nullptr;
}

bool Dialog::getControlEnabled( int ID )
{
    Control* pControl = getControl( ID );
    if( pControl == nullptr )
        return false;

    return pControl->enabled();
}

void Dialog::SetControlEnabled( int ID, bool bEnabled )
{
    Control* pControl = getControl( ID );
    if( pControl == nullptr )
        return;

    pControl->setEnabled( bEnabled );
}

void Dialog::OnMouseUp( POINT pt )
{
    s_pControlPressed = nullptr;
    m_pControlMouseOver = nullptr;
}

void Dialog::OnMouseMove( POINT pt )
{
    // Figure out which control the mouse is over now
    Control* pControl = getControlAtPoint( pt );

    // If the mouse is still over the same control, nothing needs to be done
    if( pControl == m_pControlMouseOver )
        return;

    // Handle mouse leaving the old control
    if( m_pControlMouseOver )
        m_pControlMouseOver->mouseLeave();

    // Handle mouse entering the new control
    m_pControlMouseOver = pControl;
    if( pControl != nullptr )
        m_pControlMouseOver->mouseEnter();
}

HRESULT Dialog::SetDefaultElement( UINT nControlType, UINT iElement, UIElement* pElement )
{
    // If this Element type already exist in the list, simply update the stored Element
    for( uint32_t i=0; i < (uint32_t)m_DefaultElements.size(); i++ )
    {
        ElementHolder* pElementHolder = m_DefaultElements[i ];
        
        if( pElementHolder->nControlType == nControlType &&
            pElementHolder->iElement == iElement )
        {
            pElementHolder->Element = *pElement;
            return S_OK;
        }
    }

    // Otherwise, add a new entry
    ElementHolder* pNewHolder;
    pNewHolder = new ElementHolder;
    if( pNewHolder == nullptr )
        return E_OUTOFMEMORY;

    pNewHolder->nControlType = nControlType;
    pNewHolder->iElement = iElement;
    pNewHolder->Element = *pElement;

    m_DefaultElements.push_back(pNewHolder);

    return S_OK;
}

UIElement* Dialog::GetDefaultElement( UINT nControlType, UINT iElement )
{
    for( uint32_t i=0; i < (uint32_t)(m_DefaultElements.size()); i++ )
    {
        ElementHolder* pElementHolder = m_DefaultElements[i];
        
        if( pElementHolder->nControlType == nControlType &&
            pElementHolder->iElement == iElement )
        {
            return &pElementHolder->Element;
        }
    }
    
    return nullptr;
}

HRESULT Dialog::addStatic( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault, Static** ppCreated )
{
    HRESULT hr = S_OK;
    Static* pStatic = new Static (this, _deviceInterface);

    if( ppCreated != nullptr )
        *ppCreated = pStatic;

    if( pStatic == nullptr )
        return E_OUTOFMEMORY;

    hr = addControl( pStatic );
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pStatic->setId( ID ); 
    pStatic->SetText( strText );
    pStatic->setLocation( x, y );
    pStatic->setSize( width, height );
    pStatic->_isDefault = bIsDefault;

    return S_OK;
}

HRESULT Dialog::addButton( int ID, LPCWSTR strText, int x, int y, int width, int height, UINT nHotkey, bool bIsDefault, Button** ppCreated )
{
    HRESULT hr = S_OK;

    Button* pButton = new Button (this, _deviceInterface);

    if( ppCreated != nullptr )
        *ppCreated = pButton;

    if( pButton == nullptr )
        return E_OUTOFMEMORY;

    hr = addControl( pButton );
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pButton->setId( ID ); 
    pButton->SetText( strText );
    pButton->setLocation( x, y );
    pButton->setSize( width, height );
    pButton->setHotkey( nHotkey );
    pButton->_isDefault = bIsDefault;

    return S_OK;
}

HRESULT Dialog::addCheckBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bChecked, UINT nHotkey, bool bIsDefault, CheckBox** ppCreated )
{
    HRESULT hr = S_OK;

    CheckBox* pCheckBox = new CheckBox (this, _deviceInterface);

    if( ppCreated != nullptr )
        *ppCreated = pCheckBox;

    if( pCheckBox == nullptr )
        return E_OUTOFMEMORY;

    hr = addControl( pCheckBox );
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pCheckBox->setId( ID ); 
    pCheckBox->SetText( strText );
    pCheckBox->setLocation( x, y );
    pCheckBox->setSize( width, height );
    pCheckBox->setHotkey( nHotkey );
    pCheckBox->_isDefault = bIsDefault;
    pCheckBox->SetChecked( bChecked );
    
    return S_OK;
}

HRESULT Dialog::addRadioButton( int ID, UINT nButtonGroup, LPCWSTR strText, int x, int y, int width, int height, bool bChecked, UINT nHotkey, bool bIsDefault, RadioButton** ppCreated )
{
    HRESULT hr = S_OK;

    RadioButton* pRadioButton = new RadioButton (this, _deviceInterface);

    if( ppCreated != nullptr )
        *ppCreated = pRadioButton;

    if( pRadioButton == nullptr )
        return E_OUTOFMEMORY;

    hr = addControl( pRadioButton );
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pRadioButton->setId( ID ); 
    pRadioButton->SetText( strText );
    pRadioButton->SetButtonGroup( nButtonGroup );
    pRadioButton->setLocation( x, y );
    pRadioButton->setSize( width, height );
    pRadioButton->setHotkey( nHotkey );
    pRadioButton->SetChecked( bChecked );
    pRadioButton->_isDefault = bIsDefault;
    pRadioButton->SetChecked( bChecked );

    return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT Dialog::addComboBox( int ID, int x, int y, int width, int height, UINT nHotkey, bool bIsDefault, ComboBox** ppCreated )
{
    HRESULT hr = S_OK;

    ComboBox* pComboBox = new ComboBox (this, _deviceInterface);

    if( ppCreated != nullptr )
        *ppCreated = pComboBox;
    
    
    if( pComboBox == nullptr )
        return E_OUTOFMEMORY;

    
    hr = addControl( pComboBox );

    
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pComboBox->setId( ID );     
    pComboBox->setLocation( x, y );
    pComboBox->setSize( width, height );
        
    pComboBox->setHotkey( nHotkey );
    pComboBox->_isDefault = bIsDefault;

    return S_OK;
}

HRESULT Dialog::addSlider( int ID, int x, int y, int width, int height, int min, int max, int value, bool bIsDefault, Slider** ppCreated )
{
    HRESULT hr = S_OK;

    Slider* pSlider = new Slider (this, _deviceInterface);

    if( ppCreated != nullptr )
        *ppCreated = pSlider;

    if( pSlider == nullptr )
        return E_OUTOFMEMORY;

    hr = addControl( pSlider );
    if( FAILED(hr) )
        return hr;

    // Set the ID and list index
    pSlider->setId( ID ); 
    pSlider->setLocation( x, y );
    pSlider->setSize( width, height );
    pSlider->_isDefault = bIsDefault;
    pSlider->setRange( min, max );
    pSlider->set( value );

    return S_OK;
}

HRESULT 
Dialog::addImageWidget (int ID, const Ibl::ITexture* texture, const Region2f& bounds, ImageWidget** out)
{
    ImageWidget *imageWidget = 
        new ImageWidget(this, _deviceInterface, texture, bounds);
    if (out != 0)
        *out = imageWidget;

    if (FAILED(addControl(imageWidget)) )
    {
        safedelete (imageWidget);
        return E_FAIL;
    }

    imageWidget->setId( ID ); 
    imageWidget->_isDefault = false;

    return S_OK;
}

HRESULT 
Dialog::addImageWidget (int ID, const std::string& texturename, const Region2f& bounds, ImageWidget** out)
{
    ImageWidget *imageWidget = 
        new ImageWidget(this, _deviceInterface, texturename, bounds);
    if (out != 0)
        *out = imageWidget;

    if (FAILED(addControl(imageWidget)) )
    {
        safedelete (imageWidget);
        return E_FAIL;
    }

    imageWidget->setId( ID ); 
    imageWidget->_isDefault = false;
    return S_OK;
}


HRESULT Dialog::addEditBox (int ID, LPCWSTR strText, int x, int y, 
                            int width, int height,
                            bool bIsDefault, EditBox** ppCreated)
{
    HRESULT hr = S_OK;

    EditBox *pEditBox = new EditBox (this, _deviceInterface);

    if( ppCreated != nullptr )
        *ppCreated = pEditBox;

    if( pEditBox == nullptr )
        return E_OUTOFMEMORY;

    hr = addControl( pEditBox );
    if( FAILED(hr) )
        return hr;

    // Set the ID and position
    pEditBox->setId( ID ); 
    pEditBox->setLocation( x, y );
    pEditBox->setSize( width, height );
    pEditBox->_isDefault = bIsDefault;

    if( strText )
        pEditBox->SetText( strText );

    return S_OK;
}

HRESULT Dialog::addListBox( int ID, int x, int y, int width, int height, uint32_t dwStyle, ListBox** ppCreated )
{
    HRESULT hr = S_OK;
    ListBox *pListBox = new ListBox (this, _deviceInterface);

    if( ppCreated != nullptr )
        *ppCreated = pListBox;

    if( pListBox == nullptr )
        return E_OUTOFMEMORY;

    hr = addControl( pListBox );
    if( FAILED(hr) )
        return hr;

    // Set the ID and position
    pListBox->setId( ID );
    pListBox->setLocation( x, y );
    pListBox->setSize( width, height );
    pListBox->SetStyle( dwStyle );

    return S_OK;
}

HRESULT Dialog::initControl( Control* pControl )
{
    if( pControl == nullptr )
        return E_INVALIDARG;

    pControl->_index = (uint32_t)m_Controls.size();
    
    
    // Look for a default Element entries
    for( uint32_t i=0; i < (uint32_t)m_DefaultElements.size(); i++ )
    {
        ElementHolder* pElementHolder = m_DefaultElements[i ];
        if( pElementHolder->nControlType == pControl->getType() )
        {
            pControl->setElement( pElementHolder->iElement, &pElementHolder->Element );
        }
    }

    pControl->init();
    return S_OK;
}

HRESULT Dialog::addControl( Control* pControl )
{
    HRESULT hr = S_OK;

    hr = initControl( pControl );
    
    // Add to the list
    m_Controls.push_back( pControl );
  
    return S_OK;
}

Control* Dialog::getControl (int ID)
{
    // Try to find the control with the given ID
    for( uint32_t i=0; i < (uint32_t)m_Controls.size(); i++ )
    {
        Control* pControl = m_Controls[i];

        if( pControl->id() == ID )
        {
            return pControl;
        }
    }

    // Not found
    return 0;
}

Control* Dialog::getControl( int ID, UINT nControlType )
{
    // Try to find the control with the given ID
    for( uint32_t i=0; i < (uint32_t)m_Controls.size(); i++ )
    {
        Control* pControl = m_Controls[i];

        if( pControl->id() == ID && pControl->getType() == nControlType )
        {
            return pControl;
        }
    }

    // Not found
    return nullptr;
}

Control* Dialog::GetNextControl( Control* pControl )
{
    int index = pControl->_index + 1;

    Dialog* pDialog = pControl->_dialog;
    
    // Cycle through dialogs in the loop to find the next control. Note
    // that if only one control exists in all looped dialogs it will
    // be the returned 'next' control.
    while( index >= (int) pDialog->m_Controls.size() )
    {
        pDialog = pDialog->m_pNextDialog;
        index = 0;
    }
    
    return pDialog->m_Controls[index ];    
}

Control* Dialog::GetPrevControl( Control* pControl )
{
    uint32_t index = (uint32_t)(pControl->_index - 1);

    Dialog* pDialog = pControl->_dialog;
    
    // Cycle through dialogs in the loop to find the next control. Note
    // that if only one control exists in all looped dialogs it will
    // be the returned 'previous' control.
    while( index < 0 )
    {
        pDialog = pDialog->m_pPrevDialog;
        if( pDialog == nullptr )
            pDialog = pControl->_dialog;

        index = (uint32_t)(pDialog->m_Controls.size() - 1);
    }
    
    return pDialog->m_Controls[index];    
}


void Dialog::ClearRadioButtonGroup( UINT nButtonGroup )
{
    // Find all radio buttons with the given group number
    for( uint32_t i=0; i < (uint32_t)m_Controls.size(); i++ )
    {
        Control* pControl = m_Controls[i ];

        if( pControl->getType() == _CONTROL_RADIOBUTTON )
        {
            RadioButton* pRadioButton = (RadioButton*) pControl;

            if( pRadioButton->GetButtonGroup() == nButtonGroup )
                pRadioButton->SetChecked( false, false );
        }
    }
}

void Dialog::ClearComboBox( int ID )
{
    ComboBox* pComboBox = GetComboBox( ID );
    if( pComboBox == nullptr )
        return;

    pComboBox->RemoveAllItems();
}

void Dialog::RequestFocus( Control* pControl )
{
    if( s_pControlFocus == pControl )
        return;

    if( !pControl->canHaveFocus() )
        return;

    if( s_pControlFocus )
        s_pControlFocus->focusOut();

    pControl->focusIn();
    s_pControlFocus = pControl;
}

HRESULT Dialog::DrawRect( RECT* pRect, Ibl::PackedColor color )
{
    /*
    RECT rcScreen = *pRect;
    OffsetRect( &rcScreen, _x, _y );

    // If caption is enabled, offset the Y position by its height.
    if( _caption )
        OffsetRect( &rcScreen, 0, m_nCaptionHeight );

    _SCREEN_VERTEX vertices[4] =
    {
        (float) rcScreen.left -0.5f,  (float) rcScreen.top -0.5f,    0.5f, 1.0f, color, 0, 0,
        (float) rcScreen.right -0.5f, (float) rcScreen.top -0.5f,    0.5f, 1.0f, color, 0, 0, 
        (float) rcScreen.right -0.5f, (float) rcScreen.bottom -0.5f, 0.5f, 1.0f, color, 0, 0, 
        (float) rcScreen.left -0.5f,  (float) rcScreen.bottom -0.5f, 0.5f, 1.0f, color, 0, 0,
    };

    IDirect3DDevice9* pd3dDevice = ((LPDIRECT3DDEVICE9) _deviceInterface->rawDevice());

    // Since we're doing our own drawing here we need to flush the sprites
    GetGlobalDialogResourceManager()->sprite()->flush();
    IDirect3DVertexDeclaration9 *pDecl = nullptr;
    pd3dDevice->GetVertexDeclaration( &pDecl );  // Preserve the sprite's current vertex decl
    pd3dDevice->SetFVF( _SCREEN_VERTEX::FVF );

    pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vertices, sizeof(_SCREEN_VERTEX) );

    // Restore the vertex decl
    pd3dDevice->SetVertexDeclaration( pDecl );
    pDecl->Release();
    */
    return S_OK;
}
 
HRESULT Dialog::DrawSprite( UIElement* pElement, RECT* prcDest )
{
    RECT rcTexture = pElement->rcTexture;
    
    RECT rcScreen = *prcDest;
    OffsetRect( &rcScreen, _x, _y );

    // If caption is enabled, offset the Y position by its height.
    if( _caption )
        OffsetRect( &rcScreen, 0, m_nCaptionHeight );

    TextureNode* textureNode = GetTexture( pElement->iTexture );
    
    float scaleX = (float) RectWidth( rcScreen ) / RectWidth( rcTexture );
    float scaleY = (float) RectHeight( rcScreen ) / RectHeight( rcTexture );

    /*
    //Ibl::Matrix44f scaling;
    // scaling.scaling (Ibl::Vector3f(scaleX, scaleY, 1.0));
    //GetGlobalDialogResourceManager()->sprite()->setTransform (scaling);

    Ibl::Vector3f position((float)rcScreen.left, 
                           (float)rcScreen.top, 0.0f );

    position.x /= scaleX;
    position.y /= scaleY;
    */
    currentZ -= 0.005f;
    return GetGlobalDialogResourceManager()->sprite()->render (_currentCamera, 
                                                               textureNode->texture(),
                                                               *prcDest,
                                                               rcTexture,
                                                               (float)_x, (float)_y, currentZ,
                                                               pElement->TextureColor.Current);
}

HRESULT Dialog::DrawText( LPCWSTR strText, UIElement* pElement, RECT* prcDest, bool bShadow, int nCount )
{
    HRESULT hr = S_OK;

    // No need to draw fully transparent layers
    if( pElement->FontColor.Current.w == 0 )
        return S_OK;

    RECT rcScreen = *prcDest;
    OffsetRect( &rcScreen, _x, _y );

    // If caption is enabled, offset the Y position by its height.
    if( _caption )
        OffsetRect( &rcScreen, 0, m_nCaptionHeight );

    FontNode* pFontNode = GetFont( pElement->iFont );

    std::wstring inputString(strText);
    std::string  convertedString(inputString.begin(), inputString.end());

    if( bShadow )
    {
        RECT rcShadow = rcScreen;
        Ibl::Vector2f position = 
            Ibl::Vector2f ((float)rcShadow.left + 12, (float)rcShadow.top);
        OffsetRect( &rcShadow, 1, 1 );

        if (!pFontNode->font()->render (_currentCamera, convertedString.c_str(), 
                                        position,
                                        pElement->FontColor.Current))
        {
            return E_FAIL;
        }
    }
    Ibl::Vector2f position = 
            Ibl::Vector2f ((float)rcScreen.left + 12, (float)rcScreen.top);


    pFontNode->font()->render (_currentCamera, convertedString.c_str(), position, 
                               pElement->FontColor.Current);

    return S_OK;
}

void Dialog::SetBackgroundColors (Ibl::PackedColor colorTopLeft, Ibl::PackedColor colorTopRight, Ibl::PackedColor colorBottomLeft, Ibl::PackedColor colorBottomRight )
{
    m_colorTopLeft = colorTopLeft;
    m_colorTopRight = colorTopRight;
    m_colorBottomLeft = colorBottomLeft;
    m_colorBottomRight = colorBottomRight;
}

void Dialog::SetNextDialog( Dialog* pNextDialog )
{ 
    if( pNextDialog == nullptr )
        pNextDialog = this;
    
    m_pNextDialog = (Dialog*)pNextDialog;
    m_pNextDialog->m_pPrevDialog = this;
}

void Dialog::ClearFocus()
{
    if( s_pControlFocus )
    {
        s_pControlFocus->focusOut();
        s_pControlFocus = nullptr;
    }

    ReleaseCapture();
}

void Dialog::FocusDefaultControl()
{
    // Check for default control in this dialog
    for( uint32_t i=0; i < (uint32_t)m_Controls.size(); i++ )
    {
        Control* pControl = m_Controls[i];
        if( pControl->_isDefault )
        {
            // Remove focus from the current control
            ClearFocus();

            // Give focus to the default control
            s_pControlFocus = pControl;
            s_pControlFocus->focusIn();
            return;
        }
    }
}

bool Dialog::OnCycleFocus( bool bForward )
{
    // This should only be handled by the dialog which owns the focused control, and 
    // only if a control currently has focus
/*
    if( s_pControlFocus == nullptr || s_pControlFocus->_dialog != this )
        return false;

    Control* pControl = s_pControlFocus;
    for( int i=0; i < 0xffff; i++ )
    {
        pControl = (bForward) ? GetNextControl( pControl ) : GetPrevControl( pControl );
        
        // If we've gone in a full circle then focus doesn't change
        if( pControl == s_pControlFocus )
            return true;

        // If the dialog accepts keybord input and the control can have focus then
        // move focus
        if( pControl->_dialog->m_bKeyboardInput && pControl->canHaveFocus() )
        {
            s_pControlFocus->focusOut();
            s_pControlFocus = pControl;
            s_pControlFocus->focusIn();
            return true;
        }
    }
*/
    return false;
}

HRESULT DialogResourceManager::CreateFont( UINT iFont )
{
    HRESULT hr = S_OK;

    FontNode* pFontNode = m_FontCache[iFont];

    if (FontMgr* fontMgr = _device->fontMgr())
    {
        if (Ibl::Font* font = fontMgr->createFont (pFontNode->strFace, (float)pFontNode->nHeight))
        {
            pFontNode->_font = font;
        }
    }

    return S_OK;
}

HRESULT DialogResourceManager::CreateTexture( UINT iTexture )
{
    TextureNode* textureNode = m_TextureCache[iTexture];
   
    if (!textureNode->texture())
    {
        if (const Ibl::ITexture* texture = 
            _device->textureMgr()->loadTexture(textureNode->filename()))
        {
            textureNode->setTexture (texture);
            return S_OK;
        }        
    }
    return E_FAIL;
}


void Dialog::InitDefaultElements()
{
    SetTexture ("data/textures/ui/controls/controls.dds");
    SetFont (0, "Arial", 14, FW_NORMAL);
    
    UIElement Element;
    RECT rcTexture;

    //-------------------------------------
    // Element for the caption
    //-------------------------------------
    m_CapElement.SetFont( 0 );
    SetRect( &rcTexture, 17, 269, 241, 287 );
    m_CapElement.SetTexture( 0, &rcTexture );
    m_CapElement.TextureColor.States[ _STATE_NORMAL ] = Ibl::colourARGB(255, 255, 255, 255);
    m_CapElement.FontColor.States[ _STATE_NORMAL ] = Ibl::colourARGB(255, 255, 255, 255);
    m_CapElement.SetFont( 0, Ibl::colourARGB(255, 255, 255, 255), DT_LEFT | DT_VCENTER );
    // Pre-blend as we don't need to transition the state
    m_CapElement.TextureColor.Blend( _STATE_NORMAL, 10.0f );
    m_CapElement.FontColor.Blend( _STATE_NORMAL, 10.0f );

    //-------------------------------------
    // Static
    //-------------------------------------
    Element.SetFont( 0 );
    Element.FontColor.States[ _STATE_DISABLED ] = Ibl::colourARGB( 200, 200, 200, 200 );

    // Assign the Element
    SetDefaultElement( _CONTROL_STATIC, 0, &Element );
    

    //-------------------------------------
    // Button - Button
    //-------------------------------------
    SetRect( &rcTexture, 0, 0, 136, 54 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0 );
    Element.TextureColor.States[ _STATE_NORMAL ] = Ibl::colourARGB(150, 255, 255, 255);
    Element.TextureColor.States[ _STATE_PRESSED ] = Ibl::colourARGB(200, 255, 255, 255);
    Element.FontColor.States[ _STATE_MOUSEOVER ] = Ibl::colourARGB(255, 0, 0, 0);
    
    // Assign the Element
    SetDefaultElement( _CONTROL_BUTTON, 0, &Element );
    

    //-------------------------------------
    // Button - Fill layer
    //-------------------------------------
    SetRect( &rcTexture, 136, 0, 272, 54 );
    Element.SetTexture( 0, &rcTexture, Ibl::colourARGB(0, 255, 255, 255) );
    Element.TextureColor.States[ _STATE_MOUSEOVER ] = Ibl::colourARGB(160, 255, 255, 255);
    Element.TextureColor.States[ _STATE_PRESSED ] = Ibl::colourARGB(60, 0, 0, 0);
    Element.TextureColor.States[ _STATE_FOCUS ] = Ibl::colourARGB(30, 255, 255, 255);
    
    
    // Assign the Element
    SetDefaultElement( _CONTROL_BUTTON, 1, &Element );

    //-------------------------------------
    // CheckBox - Box
    //-------------------------------------
    SetRect( &rcTexture, 0, 54, 27, 81 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, Ibl::colourARGB(255, 255, 255, 255), DT_LEFT | DT_VCENTER );
    Element.FontColor.States[ _STATE_DISABLED ] = Ibl::colourARGB( 200, 200, 200, 200 );
    Element.TextureColor.States[ _STATE_NORMAL ] = Ibl::colourARGB(150, 255, 255, 255);
    Element.TextureColor.States[ _STATE_FOCUS ] = Ibl::colourARGB(200, 255, 255, 255);
    Element.TextureColor.States[ _STATE_PRESSED ] = Ibl::colourARGB(255, 255, 255, 255);

    // MATT - INPUT STATES
    // Assign the Element
    SetDefaultElement( _CONTROL_CHECKBOX, 0, &Element );


    //-------------------------------------
    // CheckBox - Check
    //-------------------------------------
    SetRect( &rcTexture, 27, 54, 54, 81 );
    Element.SetTexture( 0, &rcTexture );
    
    // Assign the Element
    SetDefaultElement( _CONTROL_CHECKBOX, 1, &Element );


    //-------------------------------------
    // RadioButton - Box
    //-------------------------------------
    SetRect( &rcTexture, 54, 54, 81, 81 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, Ibl::colourARGB(255, 255, 255, 255), DT_LEFT | DT_VCENTER );
    Element.FontColor.States[ _STATE_DISABLED ] = Ibl::colourARGB( 200, 200, 200, 200 );
    Element.TextureColor.States[ _STATE_NORMAL ] = Ibl::colourARGB(150, 255, 255, 255);
    Element.TextureColor.States[ _STATE_FOCUS ] = Ibl::colourARGB(200, 255, 255, 255);
    Element.TextureColor.States[ _STATE_PRESSED ] = Ibl::colourARGB(255, 255, 255, 255);
    
    // Assign the Element
    SetDefaultElement( _CONTROL_RADIOBUTTON, 0, &Element );


    //-------------------------------------
    // RadioButton - Check
    //-------------------------------------
    SetRect( &rcTexture, 81, 54, 108, 81 );
    Element.SetTexture( 0, &rcTexture );
    
    // Assign the Element
    SetDefaultElement( _CONTROL_RADIOBUTTON, 1, &Element );


    //-------------------------------------
    // ComboBox - Main
    //-------------------------------------
    SetRect( &rcTexture, 7, 81, 247, 123 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0 );
    Element.TextureColor.States[ _STATE_NORMAL ] = Ibl::colourARGB(150, 200, 200, 200);
    Element.TextureColor.States[ _STATE_FOCUS ] = Ibl::colourARGB(170, 230, 230, 230);
    Element.TextureColor.States[ _STATE_DISABLED ] = Ibl::colourARGB(70, 200, 200, 200);
    Element.FontColor.States[ _STATE_MOUSEOVER ] = Ibl::colourARGB(255, 0, 0, 0);
    Element.FontColor.States[ _STATE_PRESSED ] = Ibl::colourARGB(255, 0, 0, 0);
    Element.FontColor.States[ _STATE_DISABLED ] = Ibl::colourARGB(200, 200, 200, 200);
    
    
    // Assign the Element
    SetDefaultElement( _CONTROL_COMBOBOX, 0, &Element );


    //-------------------------------------
    // ComboBox - Button
    //-------------------------------------
    SetRect( &rcTexture, 272, 0, 325, 49 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ _STATE_NORMAL ] = Ibl::colourARGB(150, 255, 255, 255);
    Element.TextureColor.States[ _STATE_PRESSED ] = Ibl::colourARGB(255, 150, 150, 150);
    Element.TextureColor.States[ _STATE_FOCUS ] = Ibl::colourARGB(200, 255, 255, 255);
    Element.TextureColor.States[ _STATE_DISABLED ] = Ibl::colourARGB(70, 255, 255, 255);
    
    // Assign the Element
    SetDefaultElement( _CONTROL_COMBOBOX, 1, &Element );


    //-------------------------------------
    // ComboBox - Dropdown
    //-------------------------------------
    SetRect( &rcTexture, 7, 123, 241, 265 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, Ibl::colourARGB(255, 0, 0, 0), DT_LEFT | DT_TOP );
    
    // Assign the Element
    SetDefaultElement( _CONTROL_COMBOBOX, 2, &Element );


    //-------------------------------------
    // ComboBox - Selection
    //-------------------------------------
    SetRect( &rcTexture, 7, 266, 241, 289 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, Ibl::colourARGB(255, 255, 255, 255), DT_LEFT | DT_TOP );
    
    // Assign the Element
    SetDefaultElement( _CONTROL_COMBOBOX, 3, &Element );


    //-------------------------------------
    // Slider - Track
    //-------------------------------------
    SetRect( &rcTexture, 1, 290, 280, 331 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ _STATE_NORMAL ] = Ibl::colourARGB(150, 255, 255, 255);
    Element.TextureColor.States[ _STATE_FOCUS ] = Ibl::colourARGB(200, 255, 255, 255);
    Element.TextureColor.States[ _STATE_DISABLED ] = Ibl::colourARGB(70, 255, 255, 255);
    
    // Assign the Element
    SetDefaultElement( _CONTROL_SLIDER, 0, &Element );

    //-------------------------------------
    // Slider - Button
    //-------------------------------------
    SetRect( &rcTexture, 248, 55, 289, 96 );
    Element.SetTexture( 0, &rcTexture );

    // Assign the Element
    SetDefaultElement( _CONTROL_SLIDER, 1, &Element );

    //-------------------------------------
    // ScrollBar - Track
    //-------------------------------------
    SetRect( &rcTexture, 243, 144, 265, 155 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ _STATE_DISABLED ] = Ibl::colourARGB(255, 200, 200, 200);
    
    
    // Assign the Element
    SetDefaultElement( _CONTROL_SCROLLBAR, 0, &Element );

    //-------------------------------------
    // ScrollBar - Up Arrow
    //-------------------------------------
    SetRect( &rcTexture, 243, 124, 265, 144 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ _STATE_DISABLED ] = Ibl::colourARGB(255, 200, 200, 200);
    
    
    // Assign the Element
    SetDefaultElement( _CONTROL_SCROLLBAR, 1, &Element );

    //-------------------------------------
    // ScrollBar - Down Arrow
    //-------------------------------------
    SetRect( &rcTexture, 243, 155, 265, 176 );
    Element.SetTexture( 0, &rcTexture );
    Element.TextureColor.States[ _STATE_DISABLED ] = Ibl::colourARGB(255, 200, 200, 200);
    
    
    // Assign the Element
    SetDefaultElement( _CONTROL_SCROLLBAR, 2, &Element );

    //-------------------------------------
    // ScrollBar - Button
    //-------------------------------------
    SetRect( &rcTexture, 266, 123, 286, 167 );
    Element.SetTexture( 0, &rcTexture );
    
    // Assign the Element
    SetDefaultElement( _CONTROL_SCROLLBAR, 3, &Element );


    //-------------------------------------
    // EditBox
    //-------------------------------------
    // Element assignment:
    //   0 - text area
    //   1 - top left border
    //   2 - top border
    //   3 - top right border
    //   4 - left border
    //   5 - right border
    //   6 - lower left border
    //   7 - lower border
    //   8 - lower right border

    Element.SetFont( 0, Ibl::colourARGB( 255, 0, 0, 0 ), DT_LEFT | DT_TOP );

    // Assign the style
    SetRect( &rcTexture, 14, 90, 241, 113 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( _CONTROL_EDITBOX, 0, &Element );
    SetRect( &rcTexture, 8, 82, 14, 90 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( _CONTROL_EDITBOX, 1, &Element );
    SetRect( &rcTexture, 14, 82, 241, 90 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( _CONTROL_EDITBOX, 2, &Element );
    SetRect( &rcTexture, 241, 82, 246, 90 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( _CONTROL_EDITBOX, 3, &Element );
    SetRect( &rcTexture, 8, 90, 14, 113 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( _CONTROL_EDITBOX, 4, &Element );
    SetRect( &rcTexture, 241, 90, 246, 113 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( _CONTROL_EDITBOX, 5, &Element );
    SetRect( &rcTexture, 8, 113, 14, 121 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( _CONTROL_EDITBOX, 6, &Element );
    SetRect( &rcTexture, 14, 113, 241, 121 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( _CONTROL_EDITBOX, 7, &Element );
    SetRect( &rcTexture, 241, 113, 246, 121 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( _CONTROL_EDITBOX, 8, &Element );


    //-------------------------------------
    // IMEEditBox
    //-------------------------------------

    Element.SetFont( 0, Ibl::colourARGB( 255, 0, 0, 0 ), DT_LEFT | DT_TOP );

    // Assign the style
    SetRect( &rcTexture, 14, 90, 241, 113 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( _CONTROL_IMEEDITBOX, 0, &Element );
    SetRect( &rcTexture, 8, 82, 14, 90 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( _CONTROL_IMEEDITBOX, 1, &Element );
    SetRect( &rcTexture, 14, 82, 241, 90 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( _CONTROL_IMEEDITBOX, 2, &Element );
    SetRect( &rcTexture, 241, 82, 246, 90 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( _CONTROL_IMEEDITBOX, 3, &Element );
    SetRect( &rcTexture, 8, 90, 14, 113 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( _CONTROL_IMEEDITBOX, 4, &Element );
    SetRect( &rcTexture, 241, 90, 246, 113 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( _CONTROL_IMEEDITBOX, 5, &Element );
    SetRect( &rcTexture, 8, 113, 14, 121 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( _CONTROL_IMEEDITBOX, 6, &Element );
    SetRect( &rcTexture, 14, 113, 241, 121 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( _CONTROL_IMEEDITBOX, 7, &Element );
    SetRect( &rcTexture, 241, 113, 246, 121 );
    Element.SetTexture( 0, &rcTexture );
    SetDefaultElement( _CONTROL_IMEEDITBOX, 8, &Element );
    // Element 9 for IME text, and indicator button
    SetRect( &rcTexture, 0, 0, 136, 54 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, Ibl::colourARGB( 255, 0, 0, 0 ), DT_CENTER | DT_VCENTER );
    SetDefaultElement( _CONTROL_IMEEDITBOX, 9, &Element );

    //-------------------------------------
    // ListBox - Main
    //-------------------------------------

    SetRect( &rcTexture, 13, 124, 241, 265 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, Ibl::colourARGB(255, 0, 0, 0), DT_LEFT | DT_TOP );

    // Assign the Element
    SetDefaultElement( _CONTROL_LISTBOX, 0, &Element );

    //-------------------------------------
    // ListBox - Selection
    //-------------------------------------

    SetRect( &rcTexture, 17, 269, 241, 287 );
    Element.SetTexture( 0, &rcTexture );
    Element.SetFont( 0, Ibl::colourARGB(255, 255, 255, 255), DT_LEFT | DT_TOP );

    // Assign the Element
    SetDefaultElement( _CONTROL_LISTBOX, 1, &Element );
}

}