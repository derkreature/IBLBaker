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

#include <IblSlider.h>
#include <IblWidgetDialog.h>
#include <IblDialogResourceManager.h>
#include <IblWidgetTypes.h>

namespace Ibl
{
Slider::Slider (Dialog *dialog, Ibl::IDevice* renderMgr ) :
Control (dialog, renderMgr)
{
    _type = _CONTROL_SLIDER;

    _min = 0;
    _max = 100;
    _value = 50;

    _pressed = false;
}

BOOL Slider::containsPoint( POINT pt ) 
{ 
    return ( PtInRect( &_boundingBox, pt ) || 
             PtInRect( &_buttonBounds, pt ) ); 
}

void Slider::updateRects()
{
    Control::updateRects();

    _buttonBounds = _boundingBox;
    _buttonBounds.right = _buttonBounds.left + RectHeight( _buttonBounds );
    OffsetRect( &_buttonBounds, -RectWidth( _buttonBounds )/2, 0 );

    _buttonX = (int) ( (_value - _min) * (float)RectWidth( _boundingBox ) / (_max - _min) );
    OffsetRect( &_buttonBounds, _buttonX, 0 );
}

int Slider::valueFromPos( int x )
{ 
    float fValuePerPixel = (float)(_max - _min) / RectWidth( _boundingBox );
    return (int) (0.5f + _min + fValuePerPixel * (x - _boundingBox.left)) ; 
}

bool Slider::handleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( !_enabled || !_visible )
        return false;

    switch( uMsg )
    {
        case WM_KEYDOWN:
        {
            switch( wParam )
            {
                case VK_HOME:
                    setInputTternal( _min, true );
                    return true;

                case VK_END:
                    setInputTternal( _max, true );
                    return true;

                case VK_PRIOR:
                case VK_LEFT:
                case VK_UP:
                    setInputTternal( _value - 1, true );
                    return true;

                case VK_NEXT:
                case VK_RIGHT:
                case VK_DOWN:
                    setInputTternal( _value + 1, true );
                    return true;
            }
            break;
        }
    }
    

    return false;
}

bool Slider::handleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
    if( !_enabled || !_visible )
        return false;

    switch( uMsg )
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        {
            if( PtInRect( &_buttonBounds, pt ) )
            {
                // Pressed while inside the control
                _pressed = true;

                // mdavidson
                SetCapture (_dialog->windowHandle());

                _dragX = pt.x;
                //m_nDragY = pt.y;
                _dragOffset = _buttonX - _dragX;

                //m_nDragValue = _value;

                if( !_hasFocus )
                    _dialog->RequestFocus( this );

                return true;
            }

            if( PtInRect( &_boundingBox, pt ) )
            {
                _dragX = pt.x;
                _dragOffset = 0;               
                _pressed = true;
                
                if( !_hasFocus )
                    _dialog->RequestFocus( this );

                if( pt.x > _buttonX + _x )
                {
                    setInputTternal( _value + 1, true );
                    return true;
                }

                if( pt.x < _buttonX + _x )
                {
                    setInputTternal( _value - 1, true );
                    return true;
                }
            }

            break;
        }

        case WM_LBUTTONUP:
        {
            if( _pressed )
            {
                _pressed = false;
                ReleaseCapture();
                _dialog->SendEvent( EVENT_SLIDER_VALUE_CHANGED, true, this );

                return true;
            }

            break;
        }

        case WM_MOUSEMOVE:
        {
            if( _pressed )
            {
                setInputTternal( valueFromPos( _x + pt.x + _dragOffset ), true );
                return true;
            }

            break;
        }
    };
    
    return false;
}

void Slider::setRange( int nMin, int nMax ) 
{
    _min = nMin;
    _max = nMax;

    setInputTternal( _value, false );
}

void Slider::setInputTternal( int nValue, bool bFromInput )
{
    // Clamp to range
    nValue = Ibl::Limits<int>::maxVal( _min, nValue );
    nValue = Ibl::Limits<int>::minVal( _max, nValue );
    
    if( nValue == _value )
        return;

    _value = nValue;
    updateRects();

    _dialog->SendEvent( EVENT_SLIDER_VALUE_CHANGED, bFromInput, this );
}

void Slider::render ( float elapsedTime )
{
    int nOffsetX = 0;
    int nOffsetY = 0;

    _CONTROL_STATE iState = _STATE_NORMAL;

    if( _visible == false )
    {
        iState = _STATE_HIDDEN;
    }
    else if( _enabled == false )
    {
        iState = _STATE_DISABLED;
    }
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
    {
        iState = _STATE_FOCUS;
    }

    float fBlendRate = ( iState == _STATE_PRESSED ) ? 0.0f : 0.8f;

    UIElement* pElement = _elements[0 ];
    
    // Blend current color
    pElement->TextureColor.Blend( iState, elapsedTime, fBlendRate ); 
    _dialog->DrawSprite( pElement, &_boundingBox );

    //TODO: remove magic numbers
    pElement = _elements[1 ];
       
    // Blend current color
    pElement->TextureColor.Blend( iState, elapsedTime, fBlendRate );
    _dialog->DrawSprite( pElement, &_buttonBounds );
}
}