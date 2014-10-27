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

#include <IblFormatConversionD3D11.h>

namespace Ibl
{
PixelFormat findFormat(DXGI_FORMAT format)
{
    switch (format)
    {
        case DXGI_FORMAT_R8_UNORM:
            return PF_L8;
        case DXGI_FORMAT_R16_UNORM:
            return PF_L16;
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            return PF_A8B8G8R8;
        case DXGI_FORMAT_B8G8R8A8_UNORM:
            return PF_A8R8G8B8;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
            return PF_A2B10G10R10;
        case DXGI_FORMAT_R16_FLOAT:
            return PF_FLOAT16_R;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            return PF_FLOAT16_RGBA;
        case DXGI_FORMAT_R32_FLOAT:
            return PF_FLOAT32_R;
        case DXGI_FORMAT_R32G32B32_FLOAT:
            return PF_FLOAT32_RGB;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            return PF_FLOAT32_RGBA;
        case DXGI_FORMAT_R16G16B16A16_UNORM:
            return PF_SHORT_RGBA;
        case DXGI_FORMAT_BC1_UNORM:
            return PF_DXT1;
        case DXGI_FORMAT_BC2_UNORM:
            return PF_DXT2;
        case DXGI_FORMAT_BC3_UNORM:
            return PF_DXT4;
        case DXGI_FORMAT_R16_TYPELESS:
            return PF_DEPTH16;
        case DXGI_FORMAT_R32_TYPELESS:
            return PF_DEPTH32;
        case DXGI_FORMAT_R24G8_TYPELESS:
            return  PF_DEPTH24S8;

        case DXGI_FORMAT_UNKNOWN:
        default:
            return PF_UNKNOWN;
    }
}

DXGI_FORMAT findFormat(PixelFormat format)
{
    switch(format)
    {
        case PF_L8:
            return DXGI_FORMAT_R8_UNORM;
        case PF_L16:
            return DXGI_FORMAT_R16_UNORM;
        case PF_A8:
            return DXGI_FORMAT_UNKNOWN;
        case PF_A4L4:
            return DXGI_FORMAT_UNKNOWN;
        case PF_BYTE_LA:
            return DXGI_FORMAT_UNKNOWN; 
        case PF_R3G3B2:
            return DXGI_FORMAT_UNKNOWN;
        case PF_A1R5G5B5:
            return DXGI_FORMAT_UNKNOWN;
        case PF_R5G6B5:
            return DXGI_FORMAT_UNKNOWN;
        case PF_A4R4G4B4:
            return DXGI_FORMAT_UNKNOWN;
        case PF_R8G8B8:
            return DXGI_FORMAT_UNKNOWN;
        case PF_A8R8G8B8:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case PF_A8B8G8R8:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        case PF_X8R8G8B8:
            return DXGI_FORMAT_UNKNOWN;
        case PF_X8B8G8R8:
            return DXGI_FORMAT_UNKNOWN;
        case PF_A2B10G10R10:
            return DXGI_FORMAT_R10G10B10A2_TYPELESS;
        case PF_A2R10G10B10:
            return DXGI_FORMAT_UNKNOWN;
        case PF_FLOAT16_R:
            return DXGI_FORMAT_R16_FLOAT;
        case PF_FLOAT16_RGBA:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case PF_FLOAT32_R:
            return DXGI_FORMAT_R32_FLOAT;
        case PF_FLOAT32_RGB:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case PF_FLOAT32_RGBA:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case PF_SHORT_RGBA:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case PF_DXT1:
            return DXGI_FORMAT_BC1_UNORM;
        case PF_DXT2:
            return DXGI_FORMAT_BC2_UNORM;
        case PF_DXT3:
            return DXGI_FORMAT_BC2_UNORM;
        case PF_DXT4:
            return DXGI_FORMAT_BC3_UNORM;
        case PF_DXT5:
            return DXGI_FORMAT_BC3_UNORM;
        case PF_DEPTH16:
            return DXGI_FORMAT_R16_TYPELESS;
        case PF_DEPTH32:
            return DXGI_FORMAT_R32_TYPELESS;
        case PF_DEPTH24S8:
            return  DXGI_FORMAT_R24G8_TYPELESS;
        case PF_UNKNOWN:
        default:
            return DXGI_FORMAT_UNKNOWN;
        }
}


DXGI_FORMAT
findTypelessFormat(const Ibl::PixelFormat& format)
{
    if (format == Ibl::PF_FLOAT32_R)
    {
        return DXGI_FORMAT_R32_TYPELESS;
    }
    else if (format == Ibl::PF_FLOAT16_R)
    {
        return DXGI_FORMAT_R16_TYPELESS;
    }
    return DXGI_FORMAT_UNKNOWN;
}

//---------------------------------------
// Return the BPP for a particular format
//---------------------------------------
size_t bitsPerPixel(DXGI_FORMAT fmt)
{
    switch (fmt)
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return 128;

    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        return 96;

    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return 64;

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        return 32;

    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:

#ifdef DXGI_1_2_FORMATS
    case DXGI_FORMAT_B4G4R4A4_UNORM:
#endif
        return 16;

    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
        return 8;

    case DXGI_FORMAT_R1_UNORM:
        return 1;

    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        return 4;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return 8;

    default:
        return 0;
    }
}

size_t
formatByteStride(const Ibl::PixelFormat& format)
{
    return bitsPerPixel(findFormat(format)) / 8;
}

}
