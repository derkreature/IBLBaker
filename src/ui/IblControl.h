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

#ifndef INCLUDED_BB_CONTROL
#define INCLUDED_BB_CONTROL

#include <IblWidgetTypes.h>
#include <IblWidgetUIElement.h>
#include <IblIDevice.h>

namespace Ibl
{
class Dialog;
class UIElement;

inline int RectWidth( RECT &rc ) { return ( (rc).right - (rc).left ); }
inline int RectHeight( RECT &rc ) { return ( (rc).bottom - (rc).top ); }

//-----------------------------------------------------------------------------
// Base class for controls
//-----------------------------------------------------------------------------
class  Control : public Ibl::IRenderResource
{
public:
    Control (Dialog *pDialog, Ibl::IDevice*);
    virtual ~Control();

    virtual bool                free() { return true;}
    virtual bool                create() { return true;}
    virtual bool                cache() { return true;}

    virtual HRESULT                init() { return S_OK; }
    virtual void                refresh();
    virtual void                render( float elapsedTime ) { };

    virtual bool                msgProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return false; }

    virtual bool                handleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return false; }
    virtual bool                handleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam ) { return false; }

    virtual bool                canHaveFocus() { return false; }
    virtual void                focusIn() { _hasFocus = true; }
    virtual void                focusOut() { _hasFocus = false; }
    virtual void                mouseEnter() { _mouseOver = true; }
    virtual void                mouseLeave() { _mouseOver = false; }
 
    virtual BOOL                containsPoint( POINT pt ) { return PtInRect( &_boundingBox, pt ); }

    virtual void                setEnabled( bool bEnabled ) { _enabled = bEnabled; }
    virtual bool                enabled() { return _enabled; }
    virtual void                setVisible( bool bVisible ) { _visible = bVisible; }
    virtual bool                visible() { return _visible; }

    UINT                        getType() const { return _type; }

    int                            id() const { return _id; }
    void                        setId( int ID ) { _id = ID; }

    void                        setLocation( int x, int y ) { _x = x; _y = y; updateRects(); }
    void                        setSize( int width, int height ) { _width = width; _height = height; updateRects(); }

    void                        setHotkey( UINT nHotkey ) { _hotkey = nHotkey; }
    UINT                        hotkey() { return _hotkey; }

    void                        setUserData( void *pUserData ) { _userData = pUserData; }
    void *                        userData() const { return _userData; }

    virtual void                setTextColor(Ibl::PackedColor Color );
    UIElement*                    element( UINT iElement ) { return _elements[iElement]; }
    HRESULT                        setElement( UINT iElement, UIElement* pElement);

    bool                        _visible;

    bool                        _mouseOver;               
    bool                        _hasFocus;               
    bool                        _isDefault;          
    bool                        usesSpriteSystem() const { return _usesSpriteSystem; }
    int                            _x, _y;
    int                            _width, _height;

    Dialog*                        _dialog;
    UINT                        _index;              
    std::vector <UIElement*>    _elements;  

    Ibl::IDevice*                deviceInterface() { return _deviceInterface; }

  protected:
    virtual void                updateRects();

    int                            _id;
    _CONTROL_TYPE                _type;  
    UINT                        _hotkey;            
    void *                        _userData;    
    bool                        _enabled;   
    bool                        _usesSpriteSystem;
    RECT                        _boundingBox;      
};

}
#endif