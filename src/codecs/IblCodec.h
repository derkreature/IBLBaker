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

#ifndef INCLUDED_BB_CODEC
#define INCLUDED_BB_CODEC

#include <IblDataStream.h>
#include <IblIteratorWrapper.h>
#include <IblIteratorRange.h>

namespace Ibl
{
    typedef std::vector<std::string> StringVector;
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup General
	*  @{
	*/

    /** Abstract class that defines a 'codec'.
        @remarks
            A codec class works like a two-way filter for data - data entered on
            one end (the decode end) gets processed and transformed into easily
            usable data while data passed the other way around codes it back.
        @par
            The codec concept is a pretty generic one - you can easily understand
            how it can be used for images, sounds, archives, even compressed data.
    */
	class Codec
    {
    protected:
        typedef std::map<std::string, Codec* > CodecList; 
        /** A map that contains all the registered codecs.
        */
        static CodecList msMapCodecs;

    public:
        class CodecData 
        {
        public:
            virtual ~CodecData() {}

            /** Returns the type of the data.
            */
            virtual std::string dataType() const { return "CodecData"; }
        };
        typedef std::unique_ptr<CodecData> CodecDataPtr;

        typedef ConstMapIterator<CodecList> CodecIterator;

    public:
    	virtual ~Codec();
    	
        /** Registers a new codec in the database.
        */
        static void registerCodec( Codec *pCodec )
        {
			auto i = msMapCodecs.find(pCodec->getType());
			if (i != msMapCodecs.end())
            {
                std::ostringstream message;
                message << pCodec->getType() << " already has a registered codec. " << __FUNCTION__;
				throw(std::exception(message.str().c_str()));
            }

            msMapCodecs[pCodec->getType()] = pCodec;
        }

		/** Return whether a codec is registered already. 
		*/
		static bool isCodecRegistered( const std::string& codecType )
		{
			return msMapCodecs.find(codecType) != msMapCodecs.end();
		}

		/** Unregisters a codec from the database.
        */
        static void unRegisterCodec( Codec *pCodec )
        {
            msMapCodecs.erase(pCodec->getType());
        }

        /** Gets the iterator for the registered codecs. */
        static CodecIterator getCodecIterator(void)
        {
            return CodecIterator(msMapCodecs.begin(), msMapCodecs.end());
        }

        /** Gets the file extension list for the registered codecs. */
        static StringVector getExtensions(void);

        /** Gets the codec registered for the passed in file extension. */
        static Codec* getCodec(const std::string& extension);

		/** Gets the codec that can handle the given 'magic' identifier. 
		@param magicNumberPtr Pointer to a stream of bytes which should identify the file.
			Note that this may be more than needed - each codec may be looking for 
			a different size magic number.
		@param maxbytes The number of bytes passed
		*/
		static Codec* getCodec(char *magicNumberPtr, size_t maxbytes);

        /** Codes the data in the input stream and saves the result in the output
            stream.
        */
        virtual DataStreamPtr code(MemoryDataStreamPtr& input, CodecDataPtr& pData) const = 0;
        /** Codes the data in the input chunk and saves the result in the output
            filename provided. Provided for efficiency since coding to memory is
            progressive therefore memory required is unknown leading to reallocations.
        @param input The input data
        @param outFileName The filename to write to
        @param pData Extra information to be passed to the codec (codec type specific)
        */
        virtual void codeToFile(MemoryDataStreamPtr& input, const std::string& outFileName, CodecDataPtr& pData) const = 0;

        /// Result of a decoding; both a decoded data stream and CodecData metadata
        typedef std::pair<MemoryDataStreamPtr, CodecDataPtr> DecodeResult;
        /** Codes the data from the input chunk into the output chunk.
            @param input Stream containing the encoded data
            @note
                Has a variable number of arguments, which depend on the codec type.
        */
        virtual DecodeResult decode(DataStreamPtr& input) const = 0;

        /** Returns the type of the codec as a std::string
        */
        virtual std::string getType() const = 0;

        /** Returns the type of the data that supported by this codec as a std::string
        */
        virtual std::string getDataType() const = 0;

		/** Returns whether a magic number header matches this codec.
		@param magicNumberPtr Pointer to a stream of bytes which should identify the file.
			Note that this may be more than needed - each codec may be looking for 
			a different size magic number.
		@param maxbytes The number of bytes passed
		*/
		virtual bool magicNumberMatch(const char *magicNumberPtr, size_t maxbytes) const 
		{ return !magicNumberToFileExt(magicNumberPtr, maxbytes).empty(); }
		/** Maps a magic number header to a file extension, if this codec recognises it.
		@param magicNumberPtr Pointer to a stream of bytes which should identify the file.
			Note that this may be more than needed - each codec may be looking for 
			a different size magic number.
		@param maxbytes The number of bytes passed
		@return A blank string if the magic number was unknown, or a file extension.
		*/
		virtual std::string magicNumberToFileExt(const char *magicNumberPtr, size_t maxbytes) const = 0;
    };
	/** @} */
	/** @} */

} // namespace

#endif
