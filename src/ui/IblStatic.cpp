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

#include <IblStatic.h>
#include <IblWidgetDialog.h>

namespace Ibl
{
Static::Static ( Dialog *dialog, Ibl::IDevice* renderMgr ) :
Control (dialog, renderMgr)
{
    _type = _CONTROL_STATIC;
    _dialog = dialog;

    memset ( &m_strText, 0, sizeof(m_strText) );  

    for( uint32_t i=0; i < (uint32_t)_elements.size(); i++ )
    {
        UIElement* pElement = _elements[i];
        safedelete( pElement );
    }

    _elements.clear();
}

void Static::render(float elapsedTime )
{    
    if( visible() == false )
        return;
    
    _CONTROL_STATE iState = _STATE_NORMAL;

    if( _enabled == false )
        iState = _STATE_DISABLED;
        
    UIElement* pElement = _elements[0];

    pElement->FontColor.Blend( iState, elapsedTime );
    _dialog->DrawText( m_strText, pElement, &_boundingBox, true );
}

HRESULT Static::GetTextCopy( LPWSTR strDest, UINT bufferCount )
{
    if( strDest == nullptr || bufferCount == 0 )
    {
        return E_INVALIDARG;
    }

    StringCchCopy( strDest, bufferCount, m_strText );
    return S_OK;
}

HRESULT Static::SetText( LPCWSTR strText )
{
    if( strText == nullptr )
    {
        m_strText[0] = 0;
        return S_OK;
    }   
    StringCchCopy( m_strText, MAX_PATH, strText); 
    return S_OK;
}

}