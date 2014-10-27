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
/* Based on code from the OGRE engine:
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2012 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#ifndef BB_DDS_CODEC
#define BB_DDS_CODEC

#include <IblImageCodec.h>

namespace Ibl
{
// Forward declarations
struct DXTColorBlock;
struct DXTExplicitAlphaBlock;
struct DXTInterpolatedAlphaBlock;

/** Codec specialized in loading DDS (Direct Draw Surface) images.
@remarks
    We implement our own codec here since we need to be able to keep DXT
    data compressed if the card supports it.
*/
class DDSCodec : public ImageCodec
{
  private:
    std::string mType;

    void flipEndian(void * pData, size_t size, size_t count) const;
    void flipEndian(void * pData, size_t size) const;

    PixelFormat convertFourCCFormat(uint32_t fourcc) const;
    PixelFormat convertPixelFormat(uint32_t rgbBits, uint32_t rMask, 
                                   uint32_t gMask, uint32_t bMask, uint32_t aMask) const;

    /// Unpack DXT colours into array of 16 colour values
    void unpackDXTColor(PixelFormat pf, const DXTColorBlock& block, ColorValue* pCol) const;
    /// Unpack DXT alphas into array of 16 colour values
    void unpackDXTAlpha(const DXTExplicitAlphaBlock& block, ColorValue* pCol) const;
    /// Unpack DXT alphas into array of 16 colour values
    void unpackDXTAlpha(const DXTInterpolatedAlphaBlock& block, ColorValue* pCol) const;

    /// Single registered codec instance
    static DDSCodec* msInstance;
  public:
    DDSCodec();
    virtual ~DDSCodec() { }

    /// @copydoc Codec::code
    DataStreamPtr code(MemoryDataStreamPtr& input, CodecDataPtr& pData) const;
    /// @copydoc Codec::codeToFile
    void codeToFile(MemoryDataStreamPtr& input, const std::string& outFileName, CodecDataPtr& pData) const;
    /// @copydoc Codec::decode
    DecodeResult decode(DataStreamPtr& input) const;
    /// @copydoc Codec::magicNumberToFileExt
    std::string magicNumberToFileExt(const char *magicNumberPtr, size_t maxbytes) const;
    
    virtual std::string getType() const;        

    /// Static method to startup and register the DDS codec
    static void startup(void);
    /// Static method to shutdown and unregister the DDS codec
    static void shutdown(void);

    static bool _forceDecompression;
};
}

#endif

