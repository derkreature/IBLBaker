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


#ifndef INCLUDED_BB_WIDGET_EDITBOX
#define INCLUDED_BB_WIDGET_EDITBOX

#include <IblControl.h>
#include <usp10.h>
#include <dimm.h>
#include <vector>

namespace Ibl
{
#define UNISCRIBE_DLLNAME L"\\usp10.dll"

#define GETPROCADDRESS( Module, APIName, Temp ) \
    Temp = GetProcAddress( Module, #APIName ); \
    if( Temp ) \
        *(FARPROC*)&_##APIName = Temp

#define PLACEHOLDERPROC( APIName ) \
    _##APIName = Dummy_##APIName

class EditBox : public Control
{
    friend class ExternalApiInitializer;
public:
    EditBox (Dialog *pDialog, Ibl::IDevice* renderMgr );
    virtual ~EditBox();

    virtual bool handleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool handleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual bool msgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual void updateRects();
    virtual bool canHaveFocus() { return (visible() && _enabled); }
    virtual void render( float elapsedTime );
    virtual void    focusIn();

    void SetText( LPCWSTR wszText, bool bSelected = false );
    LPCWSTR GetText() { return m_Buffer.GetBuffer(); }
    int GetTextLength() { return m_Buffer.GetTextSize(); }  // Returns text length in chars excluding nullptr.
    HRESULT GetTextCopy( LPWSTR strDest, UINT bufferCount );
    void ClearText();
    virtual void setTextColor( Ibl::PackedColor Color ) { m_TextColor = Color; }  // Text color
    void SetSelectedTextColor( Ibl::PackedColor Color ) { m_SelTextColor = Color; }  // Selected text color
    void SetSelectedBackColor( Ibl::PackedColor Color ) { m_SelBkColor = Color; }  // Selected background color
    void SetCaretColor( Ibl::PackedColor Color ) { m_CaretColor = Color; }  // Caret color
    void SetBorderWidth( int nBorder ) { m_nBorder = nBorder; updateRects(); }  // Border of the window
    void SetSpacing( int nSpacing ) { m_nSpacing = nSpacing; updateRects(); }
    void ParseFloatArray( float *pNumbers, int nCount );
    void SetTextFloatArray( const float *pNumbers, int nCount );

public:
    //-----------------------------------------------------------------------------
    // UniBuffer class for the edit control
    //-----------------------------------------------------------------------------
    class UniBuffer
    {
        friend class ExternalApiInitializer;
        // static member

        // Empty implementation of the Uniscribe API
        static HRESULT WINAPI Dummy_ScriptApplyDigitSubstitution (const SCRIPT_DIGITSUBSTITUTE*, 
                                                                  SCRIPT_CONTROL*, SCRIPT_STATE* ) { 
                                                                      return E_NOTIMPL; 
        }

        static HRESULT WINAPI Dummy_ScriptStringAnalyse (HDC, const void *, int, int, int, 
                                                         uint32_t, int, SCRIPT_CONTROL*, 
                                                         SCRIPT_STATE*, 
                                                         const int*, 
                                                         SCRIPT_TABDEF*, 
                                                         const BYTE*,
                                                         SCRIPT_STRING_ANALYSIS* ) 
        { return E_NOTIMPL; }
        
        static HRESULT WINAPI Dummy_ScriptStringCPtoX( SCRIPT_STRING_ANALYSIS, int, BOOL, int* ) { return E_NOTIMPL; }
        static HRESULT WINAPI Dummy_ScriptStringXtoCP( SCRIPT_STRING_ANALYSIS, int, int*, int* ) { return E_NOTIMPL; }
        static HRESULT WINAPI Dummy_ScriptStringFree( SCRIPT_STRING_ANALYSIS* ) { return E_NOTIMPL; }
        static const SCRIPT_LOGATTR* WINAPI Dummy_ScriptString_pLogAttr( SCRIPT_STRING_ANALYSIS ) { return nullptr; }
        static const int* WINAPI Dummy_ScriptString_pcOutChars( SCRIPT_STRING_ANALYSIS ) { return nullptr; }

        // Function pointers
        static HRESULT (WINAPI *_ScriptApplyDigitSubstitution)( const SCRIPT_DIGITSUBSTITUTE*, 
                                                                SCRIPT_CONTROL*, 
                                                                SCRIPT_STATE* );

        static HRESULT (WINAPI *_ScriptStringAnalyse)( HDC, const void *, 
                                                       int, int, int, 
                                                       uint32_t, int, 
                                                       SCRIPT_CONTROL*, 
                                                       SCRIPT_STATE*, 
                                                       const int*, 
                                                       SCRIPT_TABDEF*, 
                                                       const BYTE*, 
                                                       SCRIPT_STRING_ANALYSIS* );

        static HRESULT (WINAPI *_ScriptStringCPtoX)( SCRIPT_STRING_ANALYSIS, int, BOOL, int* );
        static HRESULT (WINAPI *_ScriptStringXtoCP)( SCRIPT_STRING_ANALYSIS, int, int*, int* );
        static HRESULT (WINAPI *_ScriptStringFree)( SCRIPT_STRING_ANALYSIS* );
        static const SCRIPT_LOGATTR* (WINAPI *_ScriptString_pLogAttr)( SCRIPT_STRING_ANALYSIS );
        static const int* (WINAPI *_ScriptString_pcOutChars)( SCRIPT_STRING_ANALYSIS );

        static HINSTANCE s_hDll;  // Uniscribe DLL handle

    // Instance data
    private:
        // Buffer to hold text
        WCHAR *m_pwszBuffer;    

        // Size of the buffer allocated, in characters
        int    m_nBufferSize;   

        // Uniscribe-specific
        // Font node for the font that this buffer uses
        FontNode* m_pFontNode; 

        // True if the string has changed since last analysis.
        bool m_bAnalyseRequired;

        // Analysis for the current string
        SCRIPT_STRING_ANALYSIS m_Analysis;  

    private:
        bool Grow( int nNewSize = -1 );

        // Uniscribe -- Analyse() analyses the string in the buffer
        HRESULT                Analyse();

    public:
        UniBuffer( int nInitialSize = 1 );
        ~UniBuffer();
        
        static void InitializeUniscribe();
        static void UninitializeUniscribe();

        int GetBufferSize() { return m_nBufferSize; }
        bool SetBufferSize( int nSize );
        int GetTextSize() { return lstrlenW( m_pwszBuffer ); }
        const WCHAR *GetBuffer() { return m_pwszBuffer; }
        const WCHAR &operator[]( int n ) const { return m_pwszBuffer[n]; }
        WCHAR &operator[]( int n );
        FontNode *GetFontNode() { return m_pFontNode; }
        void SetFontNode( FontNode *pFontNode ) { m_pFontNode = pFontNode; }
        void Clear();

        // Inserts the char at specified index.
        // If nIndex == -1, insert to the end.
        bool InsertChar( int nIndex, WCHAR wChar );

        // Removes the char at specified index.
        // If nIndex == -1, remove the last char.
        bool RemoveChar( int nIndex );

        // Inserts the first nCount characters of the string pStr at specified index.
        // If nCount == -1, the entire string is inserted.
        // If nIndex == -1, insert to the end.
        bool InsertString( int nIndex, const WCHAR *pStr, int nCount = -1 );

        bool SetText( LPCWSTR wszText );

        // Uniscribe
        HRESULT CPtoX( int nCP, BOOL bTrail, int *pX );
        HRESULT XtoCP( int nX, int *pCP, int *pnTrail );
        void GetPriorItemPos( int nCP, int *pPrior );
        void GetNextItemPos( int nCP, int *pPrior );
    };

protected:
    void PlaceCaret( int nCP );
    void DeleteSelectionText();
    void ResetCaretBlink();
    void CopyToClipboard();
    void PasteFromClipboard();

    // Buffer to hold text
    UniBuffer m_Buffer;     

    // Border of the window
    int      m_nBorder;     

    // Spacing between the text and the edge of border
    int                        m_nSpacing;

    // Bounding rectangle for the text
    RECT                       m_rcText;       

    // Convenient rectangles for rendering elements
    RECT                       m_rcrender[9];  

    // Caret blink time in milliseconds
    double   m_dfBlink;      

    // Last timestamp of caret blink
    double   m_dfLastBlink;  

    // Flag to indicate whether caret is currently visible
    bool                       m_bCaretOn;     

    // Caret position, in characters
    int                        m_nCaret;       
    
    // If true, control is in insert mode. Else, overwrite mode.
    bool                       m_bInsertMode;  
    
    // Starting position of the selection. The caret marks the end.
    int                        m_nSelStart;    

    // First visible character in the edit control
    int                        m_nFirstVisible;

    // Text color
    Ibl::PackedColor           m_TextColor;    

    // Selected text color
    Ibl::PackedColor           m_SelTextColor; 
    
    // Selected background color
    Ibl::PackedColor           m_SelBkColor;   
    
    // Caret color
    Ibl::PackedColor           m_CaretColor;   

    // Mouse-specific
    bool                       m_bMouseDrag;      

    // If true, we don't render the caret.
    static bool                s_bHideCaret;   
};

}
#endif