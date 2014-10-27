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

#ifndef INCLUDED_WIDGET_UI_ELEMENT
#define INCLUDED_WIDGET_UI_ELEMENT

#include <IblWidgetAPI.h>
#include <IblWidgetTypes.h>
#include <IblColor.h>

namespace Ibl
{
//------------------------------------------------------
// Contains all the display tweakables for a sub-control
//------------------------------------------------------
class UIElement
{
public:
    void                        SetTexture (UINT iTexture, 
                                            RECT* prcTexture, 
                                            PackedColor defaultTextureColor = Ibl::colourARGB(255, 255, 255, 255));

    void                        SetFont (UINT iFont, 
                                         PackedColor defaultFontColor = Ibl::colourARGB(255, 255, 255, 255), 
                                         uint32_t dwTextFormat = DT_CENTER | DT_VCENTER );
    
    void                        refresh();
    
    // Index of the texture for this Element 
    UINT                        iTexture;          
    // Index of the font for this Element
    UINT                        iFont;             
    // The format argument to DrawText 
    uint32_t                        dwTextFormat;     
    // Bounding rect of this element on the composite texture
    RECT                        rcTexture;             

    BlendColor                    TextureColor;
    BlendColor                    FontColor;
};

//--------------------------------------------------------------
// Contains all the display information for a given control type
//--------------------------------------------------------------
struct ElementHolder
{
    UINT nControlType;
    UINT iElement;
    UIElement Element;
};

}
#endif