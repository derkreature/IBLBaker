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


#ifndef INCLUDED_WIDGET_DIALOG
#define INCLUDED_WIDGET_DIALOG

#include <IblWidgetTypes.h>
#include <IblWindow.h>
#include <IblWidgetUIElement.h>
#include <IblIRenderResource.h>
#include <IblIDevice.h>
#include <IblRegion.h>
#include <IblCamera.h>

namespace Ibl
{
class Control;
class Button;
class Static;
class CheckBox;
class RadioButton;
class ComboBox;
class Slider;
class EditBox;
class ListBox;
class ScrollBar;
class ImageWidget;
class WaveformWidget;
class WindowData;

//---------------------------------------------------------
// All controls must be assigned to a dialog, which handles
// input and rendering for the controls.
//---------------------------------------------------------
class Dialog : public Ibl::IRenderResource, public Ibl::WindowDependency
{
//Ibl::WindowDependency,
  public:
    Dialog (Ibl::Window*, 
            Ibl::IDevice* renderMgr);
    virtual ~Dialog();

     virtual bool free();
     virtual bool create();
     virtual bool cache();

     // Control creation
     virtual HRESULT addStatic (int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault=false, Static** ppCreated=0);
     virtual HRESULT addButton (int ID, LPCWSTR strText, int x, int y, int width, int height, UINT nHotkey=0, bool bIsDefault=false, Button** ppCreated=0);
     virtual HRESULT addCheckBox (int ID, LPCWSTR strText, int x, int y, int width, int height, bool bChecked=false, UINT nHotkey=0, bool bIsDefault=false, CheckBox** ppCreated=0);
     virtual HRESULT addRadioButton (int ID, UINT nButtonGroup, LPCWSTR strText, int x, int y, int width, int height, bool bChecked=false, UINT nHotkey=0, bool bIsDefault=false, RadioButton** ppCreated=0);
     virtual HRESULT addComboBox (int ID, int x, int y, int width, int height, UINT nHotKey=0, bool bIsDefault=false, ComboBox** ppCreated=0);
     virtual HRESULT addSlider (int ID, int x, int y, int width, int height, int min=0, int max=100, int value=50, bool bIsDefault=false, Slider** ppCreated=0);
     virtual HRESULT addEditBox (int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault=false, EditBox** ppCreated=0);
     virtual HRESULT addListBox (int ID, int x, int y, int width, int height, uint32_t dwStyle=0, ListBox** ppCreated=0);
     virtual HRESULT addImageWidget (int ID, const Ibl::ITexture* texture, const Region2f& bounds, ImageWidget** out);
     virtual HRESULT addImageWidget (int ID, const std::string& texturename, const Region2f& bounds, ImageWidget** out = 0);

     virtual HRESULT addControl (Control* pControl);
     virtual HRESULT initControl (Control* pControl);

     // Control retrieval
     virtual Static*      GetStatic( int ID ) { return (Static*) getControl( ID, _CONTROL_STATIC ); }
     virtual Button*      GetButton( int ID ) { return (Button*) getControl( ID, _CONTROL_BUTTON ); }
     virtual CheckBox*    GetCheckBox( int ID ) { return (CheckBox*) getControl( ID, _CONTROL_CHECKBOX ); }
     virtual RadioButton* GetRadioButton( int ID ) { return (RadioButton*) getControl( ID, _CONTROL_RADIOBUTTON ); }
     virtual ComboBox*    GetComboBox( int ID ) { return (ComboBox*) getControl( ID, _CONTROL_COMBOBOX ); }
     virtual Slider*      GetSlider( int ID ) { return (Slider*) getControl( ID, _CONTROL_SLIDER ); }
     virtual EditBox*     GetEditBox( int ID ) { return (EditBox*) getControl( ID, _CONTROL_EDITBOX ); }
     virtual ListBox*     GetListBox( int ID ) { return (ListBox*) getControl( ID, _CONTROL_LISTBOX ); }

     virtual Control* getControl( int ID );
     virtual Control* getControl( int ID, UINT nControlType );
     virtual Control* getControlAtPoint( POINT pt );

     virtual bool getControlEnabled( int ID );
     virtual void SetControlEnabled( int ID, bool bEnabled );

     virtual void ClearRadioButtonGroup( UINT nGroup );
     virtual void ClearComboBox( int ID );

     // Access the default display Elements used when adding new controls
     virtual  HRESULT       SetDefaultElement( UINT nControlType, UINT iElement, UIElement* pElement );
     virtual UIElement* GetDefaultElement( UINT nControlType, UINT iElement );

     // Methods called by controls
     virtual  void SendEvent( UINT nEvent, bool bTriggeredByUser, Control* pControl );
     virtual void RequestFocus( Control* pControl );

     // render helpers
     virtual HRESULT DrawRect( RECT* pRect, Ibl::PackedColor color );
     virtual HRESULT DrawSprite( UIElement* pElement, RECT* prcDest );
     virtual  HRESULT DrawText( LPCWSTR strText, UIElement* pElement, RECT* prcDest, bool bShadow = false, int nCount = -1 );

     void         resetZ();
     // Attributes
     virtual bool visible() { return _visible; }
     virtual void setVisible( bool bVisible ) { _visible = bVisible; }
     virtual bool GetMinimized() { return m_bMinimized; }
     virtual void SetMinimized( bool bMinimized ) { m_bMinimized = bMinimized; }
     virtual void SetBackgroundColors( Ibl::PackedColor colorAllCorners ) { SetBackgroundColors( colorAllCorners, colorAllCorners, colorAllCorners, colorAllCorners ); }
     virtual void SetBackgroundColors( Ibl::PackedColor colorTopLeft, 
                                       Ibl::PackedColor colorTopRight, 
                                       Ibl::PackedColor colorBottomLeft, 
                                       Ibl::PackedColor colorBottomRight );
     virtual void EnableCaption( bool bEnable ) { _caption = bEnable; }
     virtual int GetCaptionHeight() { return m_nCaptionHeight; }
     virtual void SetCaptionHeight( int nHeight ) { m_nCaptionHeight = nHeight; }
     virtual void SetCaptionText( const WCHAR *pwszText ) { StringCchCopy( m_wszCaption, sizeof(m_wszCaption)/sizeof(m_wszCaption[0]), pwszText); }
     virtual void location( POINT &Pt )  { Pt.x = _x; Pt.y = _y; }
     virtual void setLocation( int x, int y ) { _x = x; _y = y; }
     virtual void setSize( int width, int height ) { _width = width; _height = height;  }
     virtual int GetWidth() { return _width; }
     virtual int GetHeight() { return _height; }

    virtual void SetNextDialog (Dialog* pNextDialog);

    static void SetrefreshTime( float fTime );

    static Control* GetNextControl( Control* pControl );
    static Control* GetPrevControl( Control* pControl );

    virtual void RemoveControl( int ID );
    virtual void RemoveAllControls();

    // Sets the callback used to notify the app of control events
    virtual void SetCallback( PCALLBACKGUIEVENT pCallback ) { m_pCallbackEvent = pCallback; }
    virtual void EnableNonUserEvents( bool bEnable ) { m_bNonUserEvents = bEnable; }
    virtual void EnableKeyboardInput( bool bEnable ) { m_bKeyboardInput = bEnable; }
    virtual void EnableMouseInput( bool bEnable ) { m_bMouseInput = bEnable; }

    // Device state notification
    virtual void refresh();
    virtual HRESULT render(float elapsedTime, const Ibl::Camera* camera);    

    // Shared resource access. Indexed fonts and textures are shared among
    // all the controls.
    virtual HRESULT       SetFont( UINT index, const char* strFaceName, LONG height, LONG weight );
    virtual FontNode*     GetFont( UINT index );

    virtual HRESULT          SetTexture( const char* strFilename );
    virtual TextureNode*     GetTexture( UINT index );

    virtual bool          dialogHasFocus();

    static void ClearFocus();
    virtual void FocusDefaultControl();

    bool m_bNonUserEvents;
    bool m_bKeyboardInput;
    bool m_bMouseInput;
    
    const WindowHandle windowHandle() const { return (*_parentWindow);};

  protected:
    virtual bool                  handleWindowChanged(Ibl::WindowData* data);

  private:
    int                            m_nDefaultControlID;

    static double                s_fTimerefresh;
    double                        m_fTimeLastrefresh;

    // Initialize default Elements
    void                        InitDefaultElements();

    // Windows message handlers
    void                        OnMouseMove( POINT pt );
    void                        OnMouseUp( POINT pt );

    // Control events
    bool                        OnCycleFocus( bool bForward );

    static Control*                s_pControlFocus;        
    static Control*                s_pControlPressed;      

    Control*                       m_pControlMouseOver;           

    bool                           _visible;
    bool                           _caption;
    bool                           m_bMinimized;
    WCHAR                          m_wszCaption[256];

    int                            _x;
    int                            _y;
    int                            _width;
    int                            _height;
    int                            m_nCaptionHeight;

    Ibl::PackedColor               m_colorTopLeft;
    Ibl::PackedColor               m_colorTopRight;
    Ibl::PackedColor               m_colorBottomLeft;
    Ibl::PackedColor               m_colorBottomRight;

    PCALLBACKGUIEVENT              m_pCallbackEvent;

    std::vector<int>                m_Textures;  
    std::vector<int>                m_Fonts;     

    std::vector< Control* >         m_Controls;
    std::vector< ElementHolder* >   m_DefaultElements;

    UIElement m_CapElement;  

    Dialog*                        m_pNextDialog;
    Dialog*                        m_pPrevDialog;

    Ibl::Window*                    _parentWindow;
    const Ibl::Camera*              _currentCamera;
};
}
#endif