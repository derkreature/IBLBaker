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

#ifndef INCLUDED_BB_DATA_STREAM
#define INCLUDED_BB_DATA_STREAM

#include <IblPlatform.h>
#include <IblMath.h>
#include <limits.h>
#include <memory>
#include <istream>

namespace Ibl
{    
template <size_t cacheSize>
class StaticCache
{
  protected:
    char mBuffer[cacheSize];
    size_t mValidBytes;
    size_t mPos;
    
  public:
    StaticCache()
    {
        mValidBytes = 0;
        mPos = 0;
    }
    
    size_t cacheData(const void* buf, size_t count)
    {
        assert(avail() == 0 && "It is assumed that you cache data only after you have read everything.");
        
        if (count < cacheSize)
        {
            if (count + mValidBytes <= cacheSize)
            {
                memcpy(mBuffer + mValidBytes, buf, count);
                mValidBytes += count;
            }
            else
            {
                size_t begOff = count - (cacheSize - mValidBytes);
                memmove(mBuffer, mBuffer + begOff, mValidBytes - begOff);
                memcpy(mBuffer + cacheSize - count, buf, count);
                mValidBytes = cacheSize;
            }
            mPos = mValidBytes;
            return count;
        }
        else
        {
            memcpy(mBuffer, (const char*)buf + count - cacheSize, cacheSize);
            mValidBytes = mPos = cacheSize;
            return cacheSize;
        }
    }

    size_t read(void* buf, size_t count)
    {
        size_t rb = avail();
        rb = (rb < count) ? rb : count;
        memcpy(buf, mBuffer + mPos, rb);
        mPos += rb;
        return rb;
    }
    
    bool rewind(size_t count)
    {
        if (mPos < count)
        {
            clear();
            return false;
        }
        else
        {
            mPos -= count;
            return true;
        }
    }
    bool ff(size_t count)
    {
        if (avail() < count)
        {
            clear();
            return false;
        }
        else
        {
            mPos += count;
            return true;
        }
    }
    
    size_t avail() const
    {
        return mValidBytes - mPos;
    }
    
    void clear()
    {
        mValidBytes = 0;
        mPos = 0;
    }
};


class DataStream 
{
public:
    enum AccessMode
    {
        READ = 1, 
        WRITE = 2
    };
  protected:
    std::string mName;        
    size_t mSize;
    uint16_t mAccess;

    #define BB_STREAM_BUFFER_SIZE 128
  public:
    DataStream(uint16_t accessMode = READ) : mSize(0), mAccess(accessMode) {}
    DataStream(const std::string& name, uint16_t accessMode = READ) 
        : mName(name), mSize(0), mAccess(accessMode) {}
    const std::string& getName(void) { return mName; }
    uint16_t getAccessMode() const { return mAccess; }
    virtual bool isReadable() const { return (mAccess & READ) != 0; }
    virtual bool isWriteable() const { return (mAccess & WRITE) != 0; }
    virtual ~DataStream() {}
    template<typename T> DataStream& operator>>(T& val);

    
    virtual bool ok() const = 0;
    void setError() const;
    bool readBytes(void* pBuffer, size_t length);
    bool readFloat(float& value);
    bool readInteger(int& value);
    bool readUnsignedInteger(unsigned int& value);
    bool readString(std::string& strValue);

    virtual size_t read(void* buf, size_t count) = 0;

    virtual size_t write(const void* buf, size_t count)
    {
                    (void)buf;
                    (void)count;
        // default to not supported
        return 0;
    }
    
    virtual void skip(long count) = 0;

    virtual void seek( size_t pos ) = 0;
    
    virtual size_t tell(void) const = 0;

    virtual bool eof(void) const = 0;

    size_t size(void) const;

    virtual void close(void) = 0;
};

typedef std::unique_ptr<DataStream> DataStreamPtr;
typedef std::list<DataStreamPtr> DataStreamList;

class MemoryDataStream : public DataStream
{
  protected:
    uint8_t* mData;
    uint8_t* mPos;
    uint8_t* mEnd;
    bool mFreeOnClose;            
  public:
    
    MemoryDataStream(void* pMem, size_t size, bool freeOnClose = false, bool readOnly = false);    
    MemoryDataStream(const std::string& name, void* pMem, size_t size, 
            bool freeOnClose = false, bool readOnly = false);
    MemoryDataStream(DataStream& sourceStream, 
            bool freeOnClose = true, bool readOnly = false);    
    MemoryDataStream(DataStreamPtr& sourceStream, 
            bool freeOnClose = true, bool readOnly = false);
    MemoryDataStream(const std::string& name, DataStream& sourceStream, 
            bool freeOnClose = true, bool readOnly = false);
    MemoryDataStream(const std::string& name, const DataStreamPtr& sourceStream, 
        bool freeOnClose = true, bool readOnly = false);
    MemoryDataStream(size_t size, bool freeOnClose = true, bool readOnly = false);
    MemoryDataStream(const std::string& name, size_t size, 
            bool freeOnClose = true, bool readOnly = false);

    virtual ~MemoryDataStream();

    uint8_t *                  getPtr(void) { return mData; }    
    uint8_t *                  getCurrentPtr(void) { return mPos; }
    size_t                     read(void* buf, size_t count);
    size_t                     write(const void* buf, size_t count);
    size_t                     readLine(char* buf, size_t maxCount, const std::string& delim = "\n");
    
    virtual bool               ok() const;
    size_t                     skipLine(const std::string& delim = "\n");
    void                       skip(long count);

    void                       seek( size_t pos );
    
    size_t                     tell(void) const;

    bool                       eof(void) const;

    void                       close(void);

    void                       setFreeOnClose(bool free) { mFreeOnClose = free; }
};

typedef std::shared_ptr<MemoryDataStream> MemoryDataStreamPtr;
typedef std::list<DataStreamPtr> DataStreamList;

class FileStreamDataStream : public DataStream
{
  protected:
    /// Reference to source stream (read)
    std::istream* mInStream;
    /// Reference to source file stream (read-only)
    std::ifstream* mFStreamRO;
    /// Reference to source file stream (read-write)
    std::fstream* mFStream;
    bool mFreeOnClose;    

    void determineAccess();
  public:
    FileStreamDataStream(std::ifstream* s, 
        bool freeOnClose = true);
    FileStreamDataStream(std::fstream* s, 
        bool freeOnClose = true);

    FileStreamDataStream(const std::string& name, 
        std::ifstream* s, 
        bool freeOnClose = true);
    FileStreamDataStream(const std::string& name, 
        std::fstream* s, 
        bool freeOnClose = true);

    
    FileStreamDataStream(const std::string& name, 
        std::ifstream* s, 
        size_t size, 
        bool freeOnClose = true);

    FileStreamDataStream(const std::string& name, 
        std::fstream* s, 
        size_t size, 
        bool freeOnClose = true);

    virtual ~FileStreamDataStream();

    virtual bool   ok() const;

    size_t read(void* buf, size_t count);
    size_t write(const void* buf, size_t count);
    size_t readLine(char* buf, size_t maxCount, const std::string& delim = "\n");
    void skip(long count);
    void seek( size_t pos );
    size_t tell(void) const;
    bool eof(void) const;
    void close(void);

};

class FileHandleDataStream : public DataStream
{
  protected:
    FILE* mFileHandle;

  public:
    FileHandleDataStream(FILE* handle, uint16_t accessMode = READ);
    FileHandleDataStream(const std::string& name, FILE* handle, uint16_t accessMode = READ);
    virtual ~FileHandleDataStream();
    virtual bool   ok() const;
    size_t read(void* buf, size_t count);
    size_t write(const void* buf, size_t count);
    void skip(long count);    
    void seek( size_t pos );
    size_t tell(void) const;
    bool eof(void) const;
    void close(void);

};
}
#endif

