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

#include <IblDataStream.h>
#include <IblLog.h>

namespace Ibl
{
    template <typename T> DataStream& DataStream::operator >>(T& val)
    {
        read(static_cast<void*>(&val), sizeof(T));
        return *this;
    }

    size_t 
    DataStream::size(void) const 
    { 
        return mSize; 
    }


    void DataStream::setError() const
    {
    }

    bool DataStream::readBytes(void* buffer, size_t length)
    {
        return read(buffer, length) > 0;
    }

    bool DataStream::readFloat(float& value)
    {
        read(&value, 4);
        return true;
    }

    bool DataStream::readInteger(int& value)
    {
        read(&value, 4);
        return true;
    }

    bool DataStream::readUnsignedInteger(unsigned int& value)
    {
        read(&value, 4);
        return true;
    }

    bool DataStream::readString(std::string& strValue)
    {
        int length = 0;
        readInteger(length);

        uint8_t * buffer = (uint8_t*)malloc((length+1)*sizeof(uint8_t));
        memset (buffer, 0, sizeof(char) * (length+1));
        read (buffer, length);
        strValue = std::string((const char*)buffer);
 
        free(buffer);

        return true;
    }


    MemoryDataStream::MemoryDataStream(void* pMem, size_t inSize, bool freeOnClose, bool readOnly)
        : DataStream(static_cast<uint16_t>(readOnly ? READ : (READ | WRITE)))
    {
        mData = mPos = static_cast<uint8_t*>(pMem);
        mSize = inSize;
        mEnd = mData + mSize;
        mFreeOnClose = freeOnClose;
        assert(mEnd >= mPos);
    }

    MemoryDataStream::MemoryDataStream(const std::string& name, void* pMem, size_t inSize, 
        bool freeOnClose, bool readOnly)
        : DataStream(name, static_cast<uint16_t>(readOnly ? READ : (READ | WRITE)))
    {
        mData = mPos = static_cast<uint8_t*>(pMem);
        mSize = inSize;
        mEnd = mData + mSize;
        mFreeOnClose = freeOnClose;
        assert(mEnd >= mPos);
    }

    MemoryDataStream::MemoryDataStream(DataStream& sourceStream, 
        bool freeOnClose, bool readOnly)
        : DataStream(static_cast<uint16_t>(readOnly ? READ : (READ | WRITE)))
    {
        // Copy data from incoming stream
        mSize = sourceStream.size();
        if (mSize == 0 && !sourceStream.eof())
        {
            throw std::runtime_error("Unhandled stream copy condition");
        }
        else
        {
            mData = (uint8_t*)malloc(mSize * sizeof(uint8_t));
            mPos = mData;
            mEnd = mData + sourceStream.read(mData, mSize);
            mFreeOnClose = freeOnClose;
        }
        assert(mEnd >= mPos);
    }

    MemoryDataStream::MemoryDataStream(DataStreamPtr& sourceStream, 
        bool freeOnClose, bool readOnly)
        : DataStream(static_cast<uint16_t>(readOnly ? READ : (READ | WRITE)))
    {
        // Copy data from incoming stream
        mSize = sourceStream->size();
        if (mSize == 0 && !sourceStream->eof())
        {
            throw std::runtime_error("Unhandled stream copy condition");
        }
        else
        {
            mData = (uint8_t*)malloc(sizeof(uint8_t) * mSize);
            mPos = mData;
            mEnd = mData + sourceStream->read(mData, mSize);
            mFreeOnClose = freeOnClose;
        }
        assert(mEnd >= mPos);
    }

    MemoryDataStream::MemoryDataStream(const std::string& name, DataStream& sourceStream, 
        bool freeOnClose, bool readOnly)
        : DataStream(name, static_cast<uint16_t>(readOnly ? READ : (READ | WRITE)))
    {
        // Copy data from incoming stream
        mSize = sourceStream.size();
        if (mSize == 0 && !sourceStream.eof())
        {
            throw std::runtime_error("Unhandled stream copy condition");
        }
        else
        {
            mData = (uint8_t*)malloc(sizeof(uint8_t) * mSize);
            mPos = mData;
            mEnd = mData + sourceStream.read(mData, mSize);
            mFreeOnClose = freeOnClose;
        }
        assert(mEnd >= mPos);
    }

    MemoryDataStream::MemoryDataStream(const std::string& name, const DataStreamPtr& sourceStream, 
        bool freeOnClose, bool readOnly)
        : DataStream(name, static_cast<uint16_t>(readOnly ? READ : (READ | WRITE)))
    {
        // Copy data from incoming stream
        mSize = sourceStream->size();
        if (mSize == 0 && !sourceStream->eof())
        {            
            throw std::runtime_error("Unhandled stream copy condition");
        }
        else
        {
            mData = (uint8_t*)malloc(sizeof(uint8_t) * mSize);
            mPos = mData;
            mEnd = mData + sourceStream->read(mData, mSize);
            mFreeOnClose = freeOnClose;
        }
        assert(mEnd >= mPos);
    }

    MemoryDataStream::MemoryDataStream(size_t inSize, bool freeOnClose, bool readOnly)
        : DataStream(static_cast<uint16_t>(readOnly ? READ : (READ | WRITE)))
    {
        mSize = inSize;
        mFreeOnClose = freeOnClose;
        mData = (uint8_t*)malloc(sizeof(uint8_t) * mSize);
        mPos = mData;
        mEnd = mData + mSize;
        assert(mEnd >= mPos);
    }

    MemoryDataStream::MemoryDataStream(const std::string& name, size_t inSize, 
        bool freeOnClose, bool readOnly)
        : DataStream(name, static_cast<uint16_t>(readOnly ? READ : (READ | WRITE)))
    {
        mSize = inSize;
        mFreeOnClose = freeOnClose;
        mData = (uint8_t*)malloc(sizeof(uint8_t) * mSize);
        mPos = mData;
        mEnd = mData + mSize;
        assert(mEnd >= mPos);
    }

    MemoryDataStream::~MemoryDataStream()
    {
        close();
    }

    size_t MemoryDataStream::read(void* buf, size_t count)
    {
        size_t cnt = count;
        // Read over end of memory?
        if (mPos + cnt > mEnd)
            cnt = mEnd - mPos;
        if (cnt == 0)
            return 0;

        assert (cnt<=count);

        memcpy(buf, mPos, cnt);
        mPos += cnt;
        return cnt;
    }

    size_t MemoryDataStream::write(const void* buf, size_t count)
    {
        size_t written = 0;
        if (isWriteable())
        {
            written = count;
            // we only allow writing within the extents of allocated memory
            // check for buffer overrun & disallow
            if (mPos + written > mEnd)
                written = mEnd - mPos;
            if (written == 0)
                return 0;

            memcpy(mPos, buf, written);
            mPos += written;
        }
        return written;
    }

    bool MemoryDataStream::ok() const
    {
        return (mPos <= mEnd && mPos >= mData) && mData != nullptr;
    }

    void MemoryDataStream::skip(long count)
    {
        size_t newpos = (size_t)( ( mPos - mData ) + count );
        assert( mData + newpos <= mEnd );        

        mPos = mData + newpos;
    }

    void MemoryDataStream::seek( size_t pos )
    {
        assert( mData + pos <= mEnd );
        mPos = mData + pos;
    }

    size_t MemoryDataStream::tell(void) const
    {
        //mData is start, mPos is current location
        return mPos - mData;
    }
    //-----------------------------------------------------------------------
    bool MemoryDataStream::eof(void) const
    {
        return mPos >= mEnd;
    }
    //-----------------------------------------------------------------------
    void MemoryDataStream::close(void)    
    {
        if (mFreeOnClose && mData)
        {
            free(mData);
            mData = 0;
        }

    }

    FileStreamDataStream::FileStreamDataStream(std::ifstream* s, bool freeOnClose)
        : DataStream(), mInStream(s), mFStreamRO(s), mFStream(0), mFreeOnClose(freeOnClose)
    {
        // calculate the size
        mInStream->seekg(0, std::ios_base::end);
        mSize = (size_t)mInStream->tellg();
        mInStream->seekg(0, std::ios_base::beg);
        determineAccess();
    }

    FileStreamDataStream::FileStreamDataStream(const std::string& name, 
        std::ifstream* s, bool freeOnClose)
        : DataStream(name), mInStream(s), mFStreamRO(s), mFStream(0), mFreeOnClose(freeOnClose)
    {
        // calculate the size
        mInStream->seekg(0, std::ios_base::end);
        mSize = (size_t)mInStream->tellg();
        mInStream->seekg(0, std::ios_base::beg);
        determineAccess();
    }
    //-----------------------------------------------------------------------
    FileStreamDataStream::FileStreamDataStream(const std::string& name, 
        std::ifstream* s, size_t inSize, bool freeOnClose)
        : DataStream(name), mInStream(s), mFStreamRO(s), mFStream(0), mFreeOnClose(freeOnClose)
    {
        // Size is passed in
        mSize = inSize;
        determineAccess();
    }
    //---------------------------------------------------------------------
    FileStreamDataStream::FileStreamDataStream(std::fstream* s, bool freeOnClose)
        : DataStream(false), mInStream(s), mFStreamRO(0), mFStream(s), mFreeOnClose(freeOnClose)
    {
        // writeable!
        // calculate the size
        mInStream->seekg(0, std::ios_base::end);
        mSize = (size_t)mInStream->tellg();
        mInStream->seekg(0, std::ios_base::beg);
        determineAccess();

    }
    //-----------------------------------------------------------------------
    FileStreamDataStream::FileStreamDataStream(const std::string& name, 
        std::fstream* s, bool freeOnClose)
        : DataStream(name, false), mInStream(s), mFStreamRO(0), mFStream(s), mFreeOnClose(freeOnClose)
    {
        // writeable!
        // calculate the size
        mInStream->seekg(0, std::ios_base::end);
        mSize = (size_t)mInStream->tellg();
        mInStream->seekg(0, std::ios_base::beg);
        determineAccess();
    }

    FileStreamDataStream::FileStreamDataStream(const std::string& name, 
        std::fstream* s, size_t inSize, bool freeOnClose)
        : DataStream(name, false), mInStream(s), mFStreamRO(0), mFStream(s), mFreeOnClose(freeOnClose)
    {
        // writeable!
        // Size is passed in
        mSize = inSize;
        determineAccess();
    }

    void FileStreamDataStream::determineAccess()
    {
        mAccess = 0;
        if (mInStream)
            mAccess |= READ;
        if (mFStream)
            mAccess |= WRITE;
    }

    FileStreamDataStream::~FileStreamDataStream()
    {
        close();
    }

    size_t FileStreamDataStream::read(void* buf, size_t count)
    {
        mInStream->read(static_cast<char*>(buf), static_cast<std::streamsize>(count));
        return (size_t)mInStream->gcount();
    }

    size_t FileStreamDataStream::write(const void* buf, size_t count)
    {
        size_t written = 0;
        if (isWriteable() && mFStream)
        {
            mFStream->write(static_cast<const char*>(buf), static_cast<std::streamsize>(count));
            written = count;
        }
        return written;
    }

    bool FileStreamDataStream::ok() const
    {
        if (!mInStream)
        {
            return false;
        }
        if (!(*mInStream))
        {
            return false;
        }

        return true;
    }

    size_t FileStreamDataStream::readLine(char* buf, size_t maxCount, 
        const std::string& delim)
    {
        if (delim.length() == 0)
        {
            LOG("No delimiter provided FileStreamDataStream::readLine" << __LINE__ << " " << __FILE__);
            throw (std::exception("Failed to readLine"));
        }
        if (delim.size() > 1)
        {
            LOG("WARNING: FileStreamDataStream::readLine - using only first delimeter");
        }
        // Deal with both Unix & Windows LFs
        bool trimCR = false;
        if (delim.c_str()[0] == '\n') 
        {
            trimCR = true;
        }
        // maxCount + 1 since count excludes terminator in getline
        mInStream->getline(buf, static_cast<std::streamsize>(maxCount+1), delim.c_str()[0]);
        size_t ret = (size_t)mInStream->gcount();
        // three options
        // 1) we had an eof before we read a whole line
        // 2) we ran out of buffer space
        // 3) we read a whole line - in this case the delim character is taken from the stream but not written in the buffer so the read data is of length ret-1 and thus ends at index ret-2
        // in all cases the buffer will be null terminated for us

        if (mInStream->eof()) 
        {
            // no problem
        }
        else if (mInStream->fail())
        {
            // Did we fail because of maxCount hit? No - no terminating character
            // in included in the count in this case
            if (ret == maxCount)
            {
                // clear failbit for next time 
                mInStream->clear();
            }
            else
            {
                throw(std::exception("Streaming error occurred FileStreamDataStream::readLine"));
            }
        }
        else 
        {
            // we need to adjust ret because we want to use it as a
            // pointer to the terminating null character and it is
            // currently the length of the data read from the stream
            // i.e. 1 more than the length of the data in the buffer and
            // hence 1 more than the _index_ of the nullptr character
            --ret;
        }

        // trim off CR if we found CR/LF
        if (trimCR && buf[ret-1] == '\r')
        {
            --ret;
            buf[ret] = '\0';
        }
        return ret;
    }

    void FileStreamDataStream::skip(long count)
    {
        mInStream->clear(); //Clear fail status in case eof was set
        mInStream->seekg(static_cast<std::ifstream::pos_type>(count), std::ios::cur);
    }

    void FileStreamDataStream::seek( size_t pos )
    {
        mInStream->clear(); //Clear fail status in case eof was set
        mInStream->seekg(static_cast<std::streamoff>(pos), std::ios::beg);
    }

    size_t FileStreamDataStream::tell(void) const
    {
        mInStream->clear(); //Clear fail status in case eof was set
        return (size_t)mInStream->tellg();
    }

    bool FileStreamDataStream::eof(void) const
    {
        return mInStream->eof();
    }

    void FileStreamDataStream::close(void)
    {
        if (mInStream)
        {
            // Unfortunately, there is no file-specific shared class hierarchy between fstream and ifstream (!!)
            if (mFStreamRO)
                mFStreamRO->close();
            if (mFStream)
            {
                mFStream->flush();
                mFStream->close();
            }

            if (mFreeOnClose)
            {
                // delete the stream too
                if (mFStreamRO)
                    delete (mFStreamRO);
                if (mFStream)
                    delete (mFStream);
                mInStream = 0;
                mFStreamRO = 0; 
                mFStream = 0; 
            }
        }
    }

    FileHandleDataStream::FileHandleDataStream(FILE* handle, uint16_t accessMode)
        : DataStream(accessMode), mFileHandle(handle)
    {
        // Determine size
        fseek(mFileHandle, 0, SEEK_END);
        mSize = ftell(mFileHandle);
        fseek(mFileHandle, 0, SEEK_SET);
    }

    FileHandleDataStream::FileHandleDataStream(const std::string& name, FILE* handle, uint16_t accessMode)
        : DataStream(name, accessMode), mFileHandle(handle)
    {
        // Determine size
        fseek(mFileHandle, 0, SEEK_END);
        mSize = ftell(mFileHandle);
        fseek(mFileHandle, 0, SEEK_SET);
    }

    FileHandleDataStream::~FileHandleDataStream()
    {
        close();
    }

    bool
    FileHandleDataStream::ok() const
    {
        if (!mFileHandle)
        {
            return false;
        }
        return true;
    }

    size_t FileHandleDataStream::read(void* buf, size_t count)
    {
        return fread(buf, 1, count, mFileHandle);
    }

    size_t FileHandleDataStream::write(const void* buf, size_t count)
    {
        if (!isWriteable())
            return 0;
        else
            return fwrite(buf, 1, count, mFileHandle);
    }

    void FileHandleDataStream::skip(long count)
    {
        fseek(mFileHandle, count, SEEK_CUR);
    }

    void FileHandleDataStream::seek( size_t pos )
    {
        fseek(mFileHandle, static_cast<long>(pos), SEEK_SET);
    }

    size_t FileHandleDataStream::tell(void) const
    {
        return ftell( mFileHandle );
    }

    bool FileHandleDataStream::eof(void) const
    {
        return feof(mFileHandle) != 0;
    }

    void FileHandleDataStream::close(void)
    {
        if (mFileHandle != 0)
        {
            fclose(mFileHandle);
            mFileHandle = 0;
        }
    }

}
