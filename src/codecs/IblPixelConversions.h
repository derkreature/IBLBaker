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
-----------------------------------------------------------------------------*/

#define FMTCONVERTERID(from,to) (((from)<<8)|(to))

template <class U> struct PixelBoxConverter 
{
    static const int ID = U::ID;
    static void conversion(const Ibl::PixelBox &src, const Ibl::PixelBox &dst)
    {
        typename U::SrcType *srcptr = static_cast<typename U::SrcType*>(src.data)
            + (src.minExtent.x + src.minExtent.y * src.rowPitch + src.minExtent.z * src.slicePitch);
        typename U::DstType *dstptr = static_cast<typename U::DstType*>(dst.data)
            + (dst.minExtent.x + dst.minExtent.y * dst.rowPitch + dst.minExtent.z * dst.slicePitch);
        const size_t srcSliceSkip = src.getSliceSkip();
        const size_t dstSliceSkip = dst.getSliceSkip();
        const size_t k = src.maxExtent.x - src.minExtent.x;
        for (size_t z = src.minExtent.z; z<src.maxExtent.z; z++)
        {
            for (size_t y = src.minExtent.y; y<src.maxExtent.y; y++)
            {
                for(size_t x=0; x<k; x++)
                {
                    dstptr[x] = U::pixelConvert(srcptr[x]);
                }
                srcptr += src.rowPitch;
                dstptr += dst.rowPitch;
            }
            srcptr += srcSliceSkip;
            dstptr += dstSliceSkip;
        }    
    }
};

template <typename T, typename U, int id> struct PixelConverter {
    static const int ID = id;
    typedef T SrcType;
    typedef U DstType;    
    
    //inline static DstType pixelConvert(const SrcType &inp);
};


/** Type for PF_R8G8B8/PF_B8G8R8 */
struct Col3b {
    Col3b(unsigned int a, unsigned int b, unsigned int c): 
        x((uint8_t)a), y((uint8_t)b), z((uint8_t)c) { }
    uint8_t x,y,z;
};
/** Type for PF_FLOAT32_RGB */
struct Col3f {
    Col3f(float inR, float inG, float inB):
        r(inR), g(inG), b(inB) { }
    float r,g,b;
};
/** Type for PF_FLOAT32_RGBA */
struct Col4f {
    Col4f(float inR, float inG, float inB, float inA):
        r(inR), g(inG), b(inB), a(inA) { }
    float r,g,b,a;
};

struct A8R8G8B8toA8B8G8R8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_A8R8G8B8, Ibl::PF_A8B8G8R8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<16)|(inp&0xFF00FF00)|((inp&0x00FF0000)>>16);
    }
};

struct A8R8G8B8toB8G8R8A8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_A8R8G8B8, Ibl::PF_B8G8R8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
    }
};

struct A8R8G8B8toR8G8B8A8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_A8R8G8B8, Ibl::PF_R8G8B8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x00FFFFFF)<<8)|((inp&0xFF000000)>>24);
    }
};

struct A8B8G8R8toA8R8G8B8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_A8B8G8R8, Ibl::PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<16)|(inp&0xFF00FF00)|((inp&0x00FF0000)>>16);
    }
};

struct A8B8G8R8toB8G8R8A8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_A8B8G8R8, Ibl::PF_B8G8R8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x00FFFFFF)<<8)|((inp&0xFF000000)>>24);
    }
};

struct A8B8G8R8toR8G8B8A8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_A8B8G8R8, Ibl::PF_R8G8B8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
    }
};

struct B8G8R8A8toA8R8G8B8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_B8G8R8A8, Ibl::PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
    }
};

struct B8G8R8A8toA8B8G8R8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_B8G8R8A8, Ibl::PF_A8B8G8R8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<24)|((inp&0xFFFFFF00)>>8);
    }
};

struct B8G8R8A8toR8G8B8A8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_B8G8R8A8, Ibl::PF_R8G8B8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x0000FF00)<<16)|(inp&0x00FF00FF)|((inp&0xFF000000)>>16);
    }
};

struct R8G8B8A8toA8R8G8B8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_R8G8B8A8, Ibl::PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<24)|((inp&0xFFFFFF00)>>8);
    }
};

struct R8G8B8A8toA8B8G8R8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_R8G8B8A8, Ibl::PF_A8B8G8R8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
    }
};

struct R8G8B8A8toB8G8R8A8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_R8G8B8A8, Ibl::PF_B8G8R8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x0000FF00)<<16)|(inp&0x00FF00FF)|((inp&0xFF000000)>>16);
    }
};

struct A8B8G8R8toL8: public PixelConverter <uint32_t, uint8_t, FMTCONVERTERID(Ibl::PF_A8B8G8R8, Ibl::PF_L8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return (uint8_t)(inp&0x000000FF);
    }
};

struct L8toA8B8G8R8: public PixelConverter <uint8_t, uint32_t, FMTCONVERTERID(Ibl::PF_L8, Ibl::PF_A8B8G8R8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return 0xFF000000|(((unsigned int)inp)<<0)|(((unsigned int)inp)<<8)|(((unsigned int)inp)<<16);
    }
};

struct A8R8G8B8toL8: public PixelConverter <uint32_t, uint8_t, FMTCONVERTERID(Ibl::PF_A8R8G8B8, Ibl::PF_L8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return (uint8_t)((inp&0x00FF0000)>>16);
    }
};

struct L8toA8R8G8B8: public PixelConverter <uint8_t, uint32_t, FMTCONVERTERID(Ibl::PF_L8, Ibl::PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return 0xFF000000|(((unsigned int)inp)<<0)|(((unsigned int)inp)<<8)|(((unsigned int)inp)<<16);
    }
};

struct B8G8R8A8toL8: public PixelConverter <uint32_t, uint8_t, FMTCONVERTERID(Ibl::PF_B8G8R8A8, Ibl::PF_L8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return (uint8_t)((inp&0x0000FF00)>>8);
    }
};

struct L8toB8G8R8A8: public PixelConverter <uint8_t, uint32_t, FMTCONVERTERID(Ibl::PF_L8, Ibl::PF_B8G8R8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return 0x000000FF|(((unsigned int)inp)<<8)|(((unsigned int)inp)<<16)|(((unsigned int)inp)<<24);
    }
};

struct L8toL16: public PixelConverter <uint8_t, uint16_t, FMTCONVERTERID(Ibl::PF_L8, Ibl::PF_L16)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return (uint16_t)((((unsigned int)inp)<<8)|(((unsigned int)inp)));
    }
};

struct L16toL8: public PixelConverter <uint16_t, uint8_t, FMTCONVERTERID(Ibl::PF_L16, Ibl::PF_L8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return (uint8_t)(inp>>8);
    }
};

struct R8G8B8toB8G8R8: public PixelConverter <Col3b, Col3b, FMTCONVERTERID(Ibl::PF_R8G8B8, Ibl::PF_B8G8R8)>
{
    inline static DstType pixelConvert(const SrcType &inp)
    {
        return Col3b(inp.z, inp.y, inp.x);
    }  
};

struct B8G8R8toR8G8B8: public PixelConverter <Col3b, Col3b, FMTCONVERTERID(Ibl::PF_B8G8R8, Ibl::PF_R8G8B8)>
{
    inline static DstType pixelConvert(const SrcType &inp)
    {
        return Col3b(inp.z, inp.y, inp.x);
    }  
};

// X8Y8Z8 ->  X8<<xshift Y8<<yshift Z8<<zshift A8<<ashift
template <int id, unsigned int xshift, unsigned int yshift, unsigned int zshift, unsigned int ashift> struct Col3btoUint32swizzler:
    public PixelConverter <Col3b, uint32_t, id>
{
    inline static uint32_t pixelConvert(const Col3b &inp)
    {
        return (0xFF<<ashift) | (((unsigned int)inp.x)<<zshift) | (((unsigned int)inp.y)<<yshift) | (((unsigned int)inp.z)<<xshift);
    }
};

struct R8G8B8toA8R8G8B8: public Col3btoUint32swizzler<FMTCONVERTERID(Ibl::PF_R8G8B8, Ibl::PF_A8R8G8B8), 16, 8, 0, 24> { };
struct B8G8R8toA8R8G8B8: public Col3btoUint32swizzler<FMTCONVERTERID(Ibl::PF_B8G8R8, Ibl::PF_A8R8G8B8), 0, 8, 16, 24> { };
struct R8G8B8toA8B8G8R8: public Col3btoUint32swizzler<FMTCONVERTERID(Ibl::PF_R8G8B8, Ibl::PF_A8B8G8R8), 0, 8, 16, 24> { };
struct B8G8R8toA8B8G8R8: public Col3btoUint32swizzler<FMTCONVERTERID(Ibl::PF_B8G8R8, Ibl::PF_A8B8G8R8), 16, 8, 0, 24> { };
struct R8G8B8toB8G8R8A8: public Col3btoUint32swizzler<FMTCONVERTERID(Ibl::PF_R8G8B8, Ibl::PF_B8G8R8A8), 8, 16, 24, 0> { };
struct B8G8R8toB8G8R8A8: public Col3btoUint32swizzler<FMTCONVERTERID(Ibl::PF_B8G8R8, Ibl::PF_B8G8R8A8), 24, 16, 8, 0> { };

struct A8R8G8B8toR8G8B8: public PixelConverter <uint32_t, Col3b, FMTCONVERTERID(Ibl::PF_A8R8G8B8, Ibl::PF_BYTE_RGB)>
{
    inline static DstType pixelConvert(uint32_t inp)
    {
        return Col3b((uint8_t)((inp>>16)&0xFF), (uint8_t)((inp>>8)&0xFF), (uint8_t)((inp>>0)&0xFF));
    }
};
struct A8R8G8B8toB8G8R8: public PixelConverter <uint32_t, Col3b, FMTCONVERTERID(Ibl::PF_A8R8G8B8, Ibl::PF_BYTE_BGR)>
{
    inline static DstType pixelConvert(uint32_t inp)
    {
        return Col3b((uint8_t)((inp>>0)&0xFF), (uint8_t)((inp>>8)&0xFF), (uint8_t)((inp>>16)&0xFF));
    }
};

// Only conversions from X8R8G8B8 to formats with alpha need to be defined, the rest is implicitly the same
// as A8R8G8B8
struct X8R8G8B8toA8R8G8B8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_X8R8G8B8, Ibl::PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return inp | 0xFF000000;
    }
};
struct X8R8G8B8toA8B8G8R8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_X8R8G8B8, Ibl::PF_A8B8G8R8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x0000FF)<<16)|((inp&0xFF0000)>>16)|(inp&0x00FF00)|0xFF000000;
    }
};
struct X8R8G8B8toB8G8R8A8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_X8R8G8B8, Ibl::PF_B8G8R8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x0000FF)<<24)|((inp&0xFF0000)>>8)|((inp&0x00FF00)<<8)|0x000000FF;
    }
};
struct X8R8G8B8toR8G8B8A8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_X8R8G8B8, Ibl::PF_R8G8B8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0xFFFFFF)<<8)|0x000000FF;
    }
};

// X8B8G8R8
struct X8B8G8R8toA8R8G8B8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_X8B8G8R8, Ibl::PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x0000FF)<<16)|((inp&0xFF0000)>>16)|(inp&0x00FF00)|0xFF000000;
    }
};
struct X8B8G8R8toA8B8G8R8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_X8B8G8R8, Ibl::PF_A8B8G8R8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return inp | 0xFF000000;
    }
};
struct X8B8G8R8toB8G8R8A8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_X8B8G8R8, Ibl::PF_B8G8R8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0xFFFFFF)<<8)|0x000000FF;
    }
};
struct X8B8G8R8toR8G8B8A8: public PixelConverter <uint32_t, uint32_t, FMTCONVERTERID(Ibl::PF_X8B8G8R8, Ibl::PF_R8G8B8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x0000FF)<<24)|((inp&0xFF0000)>>8)|((inp&0x00FF00)<<8)|0x000000FF;
    }
};


#define CASECONVERTER(type) case type::ID : PixelBoxConverter<type>::conversion(src, dst); return 1;

inline int doOptimizedConversion(const Ibl::PixelBox &src, const Ibl::PixelBox &dst)
{;
    switch(FMTCONVERTERID(src.format, dst.format))
    {
        // Register converters here
        CASECONVERTER(A8R8G8B8toA8B8G8R8);
        CASECONVERTER(A8R8G8B8toB8G8R8A8);
        CASECONVERTER(A8R8G8B8toR8G8B8A8);
        CASECONVERTER(A8B8G8R8toA8R8G8B8);
        CASECONVERTER(A8B8G8R8toB8G8R8A8);
        CASECONVERTER(A8B8G8R8toR8G8B8A8);
        CASECONVERTER(B8G8R8A8toA8R8G8B8);
        CASECONVERTER(B8G8R8A8toA8B8G8R8);
        CASECONVERTER(B8G8R8A8toR8G8B8A8);
        CASECONVERTER(R8G8B8A8toA8R8G8B8);
        CASECONVERTER(R8G8B8A8toA8B8G8R8);
        CASECONVERTER(R8G8B8A8toB8G8R8A8);
        CASECONVERTER(A8B8G8R8toL8);
        CASECONVERTER(L8toA8B8G8R8);
        CASECONVERTER(A8R8G8B8toL8);
        CASECONVERTER(L8toA8R8G8B8);
        CASECONVERTER(B8G8R8A8toL8);
        CASECONVERTER(L8toB8G8R8A8);
        CASECONVERTER(L8toL16);
        CASECONVERTER(L16toL8);
        CASECONVERTER(B8G8R8toR8G8B8);
        CASECONVERTER(R8G8B8toB8G8R8);
        CASECONVERTER(R8G8B8toA8R8G8B8);
        CASECONVERTER(B8G8R8toA8R8G8B8);
        CASECONVERTER(R8G8B8toA8B8G8R8);
        CASECONVERTER(B8G8R8toA8B8G8R8);
        CASECONVERTER(R8G8B8toB8G8R8A8);
        CASECONVERTER(B8G8R8toB8G8R8A8);
        CASECONVERTER(A8R8G8B8toR8G8B8);
        CASECONVERTER(A8R8G8B8toB8G8R8);
        CASECONVERTER(X8R8G8B8toA8R8G8B8);
        CASECONVERTER(X8R8G8B8toA8B8G8R8);
        CASECONVERTER(X8R8G8B8toB8G8R8A8);
        CASECONVERTER(X8R8G8B8toR8G8B8A8);
        CASECONVERTER(X8B8G8R8toA8R8G8B8);
        CASECONVERTER(X8B8G8R8toA8B8G8R8);
        CASECONVERTER(X8B8G8R8toB8G8R8A8);
        CASECONVERTER(X8B8G8R8toR8G8B8A8);

        default:
            return 0;
    }
}
#undef CASECONVERTER
/** @} */
/** @} */
