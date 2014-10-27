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

#ifndef INCLUDED_BB_WIDGET_DIALOG_BASE
#define INCLUDED_BB_WIDGET_DIALOG_BASE

#include <IblWidgetTypes.h>
#include <IblWidgetUIElement.h>
#include <IblWindow.h>

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
class IMEEditBox;
class ListBox;
class ScrollBar;
    
class  DialogBase
{
public:
    DialogBase (const Ibl::Window* parentWindow){ _parentWindow = parentWindow;};
    ~DialogBase (){};

    // Windows message handler
    virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )= 0;

    // Control creation
    virtual HRESULT AddStatic( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault=false, Static** ppCreated=nullptr ) = 0;
    virtual HRESULT AddButton( int ID, LPCWSTR strText, int x, int y, int width, int height, UINT nHotkey=0, bool bIsDefault=false, Button** ppCreated=nullptr )=0;
    virtual HRESULT AddCheckBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bChecked=false, UINT nHotkey=0, bool bIsDefault=false, CheckBox** ppCreated=nullptr ) = 0;
    virtual HRESULT AddRadioButton( int ID, UINT nButtonGroup, LPCWSTR strText, int x, int y, int width, int height, bool bChecked=false, UINT nHotkey=0, bool bIsDefault=false, RadioButton** ppCreated=nullptr ) = 0;
    virtual HRESULT AddComboBox( int ID, int x, int y, int width, int height, UINT nHotKey=0, bool bIsDefault=false, ComboBox** ppCreated=nullptr ) = 0;
    virtual HRESULT AddSlider( int ID, int x, int y, int width, int height, int min=0, int max=100, int value=50, bool bIsDefault=false, Slider** ppCreated=nullptr ) = 0;
    virtual HRESULT AddEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault=false, EditBox** ppCreated=nullptr ) = 0;
    virtual HRESULT AddIMEEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault=false, IMEEditBox** ppCreated=nullptr ) = 0;
    virtual HRESULT AddListBox( int ID, int x, int y, int width, int height, uint32_t dwStyle=0, ListBox** ppCreated=nullptr ) = 0;
    virtual HRESULT AddControl( Control* pControl ) = 0;
    virtual HRESULT InitControl( Control* pControl ) = 0;

    // Control retrieval
    virtual Static*      GetStatic( int ID ) = 0;
    virtual Button*      GetButton( int ID )= 0;
    virtual CheckBox*    GetCheckBox( int ID ) = 0;
    virtual RadioButton* GetRadioButton( int ID ) = 0;
    virtual ComboBox*    GetComboBox( int ID ) = 0;
    virtual Slider*      GetSlider( int ID ) = 0;
    virtual EditBox*     GetEditBox( int ID ) = 0;
    virtual IMEEditBox*  GetIMEEditBox( int ID ) = 0;
    virtual ListBox*     GetListBox( int ID ) = 0;

    virtual Control* GetControl( int ID )= 0;
    virtual Control* GetControl( int ID, UINT nControlType )= 0;
    virtual Control* GetControlAtPoint( POINT pt )= 0;

    virtual bool GetControlEnabled( int ID )= 0;
    virtual  void SetControlEnabled( int ID, bool bEnabled )= 0;

    virtual void ClearRadioButtonGroup( UINT nGroup )= 0;
    virtual void ClearComboBox( int ID )= 0;

    // Access the default display Elements used when adding new controls
    virtual HRESULT       SetDefaultElement( UINT nControlType, UINT iElement, UIElement* pElement )= 0;;
    virtual UIElement* GetDefaultElement( UINT nControlType, UINT iElement )= 0;;

    // Methods called by controls
    virtual void SendEvent( UINT nEvent, bool bTriggeredByUser, Control* pControl )= 0;;
    virtual void RequestFocus( Control* pControl )= 0;;

    // Render helpers
    virtual HRESULT DrawRect( RECT* pRect, D3DCOLOR color )= 0;;
    virtual HRESULT DrawPolyLine( POINT* apPoints, UINT nNumPoints, D3DCOLOR color )= 0;
    virtual HRESULT DrawSprite( UIElement* pElement, RECT* prcDest )= 0;;
    virtual HRESULT CalcTextRect( LPCWSTR strText, UIElement* pElement, RECT* prcDest, int nCount = -1 )= 0;
    virtual HRESULT DrawText( LPCWSTR strText, UIElement* pElement, RECT* prcDest, bool bShadow = false, int nCount = -1 )= 0;

    // Attributes
    virtual bool GetVisible() = 0;
    virtual void SetVisible( bool bVisible ) = 0;
    virtual bool GetMinimized() = 0;
    virtual void SetMinimized( bool bMinimized ) = 0;
    virtual void SetBackgroundColors( D3DCOLOR colorAllCorners )= 0;
    virtual void SetBackgroundColors( D3DCOLOR colorTopLeft, D3DCOLOR colorTopRight, D3DCOLOR colorBottomLeft, D3DCOLOR colorBottomRight )= 0;
    virtual void EnableCaption( bool bEnable )= 0;
    virtual int GetCaptionHeight() = 0;
    virtual void SetCaptionHeight( int nHeight )= 0;
    virtual void SetCaptionText( const WCHAR *pwszText )= 0;
    virtual void GetLocation( POINT &Pt ) = 0;
    virtual void SetLocation( int x, int y ) = 0;
    virtual void SetSize( int width, int height ) = 0;
    virtual int GetWidth()= 0;
    virtual int GetHeight()= 0;

    virtual void SetNextDialog( DialogBase* pNextDialog ) = 0;

     

    virtual  void RemoveControl( int ID ) = 0;
    virtual void RemoveAllControls() = 0;

     // Sets the callback used to notify the app of control events
    virtual  void SetCallback( PCALLBACKGUIEVENT pCallback )  = 0;
    virtual  void EnableNonUserEvents( bool bEnable )  = 0;
    virtual  void EnableKeyboardInput( bool bEnable )  = 0;
    virtual  void EnableMouseInput( bool bEnable ) = 0;

     // Device state notification
    virtual  void Refresh() = 0;
    virtual HRESULT Render( float fElapsedTime ) = 0;    

    // Shared resource access. Indexed fonts and textures are shared among
    // all the controls.
    virtual  HRESULT       SetFont( UINT index, LPCWSTR strFaceName, LONG height, LONG weight ) = 0;
    virtual  FontNode* GetFont( UINT index ) = 0;

    virtual  HRESULT          SetTexture( UINT index, LPCWSTR strFilename ) = 0;
    virtual  TextureNode*      GetTexture( UINT index ) = 0;

    virtual  void FocusDefaultControl() = 0;
    
    const WindowHandle windowHandle() const { return (*_parentWindow);};

private:
    const Ibl::Window* _parentWindow;
};

}

#endif