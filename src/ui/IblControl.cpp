//-------//------------------------------------------------------------------------------------//
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


#include <IblControl.h>
#include <IblWidgetDialog.h>
#include <IblDialogResourceManager.h>

namespace Ibl
{

Control::Control (Dialog *dialog, Ibl::IDevice* device ) :
    Ibl::IRenderResource (device)
{
    _usesSpriteSystem = true;
    _visible = true;
    _type = _CONTROL_BUTTON;
    _dialog = dialog;
    _deviceInterface = device;
    _id = 0;
    _index = 0;
    _userData = nullptr;

    _enabled = true;
    _mouseOver = false;
    _hasFocus = false;
    _isDefault = false;

    _x = 0;
    _y = 0;
    _width = 0;
    _height = 0;

    memset ( &_boundingBox, 0, sizeof( _boundingBox ) );
}

Control::~Control()
{
    for( uint32_t i = 0; i < (uint32_t)_elements.size(); ++i )
    {
        delete _elements[i];
    }
    _elements.clear();
}

void
Control::setTextColor (Ibl::PackedColor Color )
{
    UIElement* pElement = _elements[0];

    if( pElement )
        pElement->FontColor.States[_STATE_NORMAL] = Color;
}

HRESULT
Control::setElement( UINT iElement, UIElement* pElement )
{
    HRESULT hr = S_OK;

    if( pElement == nullptr )
        return E_INVALIDARG;

    // Make certain the array is this large
    if(iElement >= _elements.size())
    {
        _elements.push_back( new UIElement() );
    }

    // Update the data
    UIElement* pCurElement = _elements[iElement];
    *pCurElement = *pElement;
    
    return S_OK;
}

void
Control::refresh()
{
    _mouseOver = false;
    _hasFocus = false;

    for( uint32_t i=0; i < (uint32_t)_elements.size(); i++ )
    {
        UIElement* pElement = _elements[i];
        pElement->refresh();
    }
}

void
Control::updateRects()
{
    SetRect( &_boundingBox, _x, _y, _x + _width, _y + _height );
}

}
