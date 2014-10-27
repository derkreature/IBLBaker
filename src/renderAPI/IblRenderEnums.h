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
#ifndef INCLUDED_RENDERENUMS_INIT_DATA
#define INCLUDED_RENDERENUMS_INIT_DATA

#include <IblPlatform.h>
#include <IblVector3.h>
#include <IblVector2.h>
#include <IblPixelFormat.h>
#include <stdint.h>
#include <limits.h>


namespace Ibl
{
const uint32_t MIRROR_BACK_BUFFER = UINT32_MAX;
const uint32_t MIRROR_BACK_BUFFER_HALF = UINT32_MAX-1;
const uint32_t MIRROR_BACK_BUFFER_QUARTER = UINT32_MAX-2;
const uint32_t MIRROR_BACK_BUFFER_EIGTH = UINT32_MAX-3;

// for linux this will have to become char.
typedef LPCSTR ShaderHandle;

enum DrawMode
{
    Point = 1,
    Wireframe = 2,
    Filled = 3
};

enum PrimitiveType
{
    UndefinedPrimitiveType           = 0,
    PointList                        = 1,
    LineList                         = 2,
    LineStrip                        = 3,
    TriangleList                     = 4,
    TriangleStrip                    = 5,
    LineListAdj                      = 10,
    LineStripAdj                     = 11,
    TriangleListAdj                  = 12,
    TringleStripAdj                  = 13,

    OneControlPointPatchList         = 33,
    TwoControlPointPatchList         = 34,
    ThreeControlPointPatchList         = 35,
    FourControlPointPatchList         = 36,
    FiveControlPointPatchList         = 37,
    SixControlPointPatchList            = 38,
    SevenControlPointPatchList         = 39,
    EightControlPointPatchList         = 40,
    NineControlPointPatchList         = 41,
    TenControlPointPatchList         = 42,
    ElevenControlPointPatchList         = 43,
    TwelveControlPointPatchList         = 44,
    ThirteenControlPointPatchList     = 45,
    FourteenControlPointPatchList     = 46,
    FifteenControlPointPatchList     = 47,
    SixteenControlPointPatchList     = 48,
    SeventeenControlPointPatchList     = 49,
    EighteenControlPointPatchList     = 50,
    NineteenControlPointPatchList     = 51,
    TwentyControlPointPatchList         = 52,
    TwentyOneControlPointPatchList     = 53,
    TwentyTwoControlPointPatchList     = 54,
    TwentyThreeControlPointPatchList = 55,
    TwentyFourControlPointPatchList     = 56,
    TwentyFiveControlPointPatchList     = 57,
    TwentySixControlPointPatchList     = 58,
    TwentySevenControlPointPatchList = 59,
    TwentyEightControlPointPatchList = 60,
    TwentyNineControlPointPatchList     = 61,
    ThirtyControlPointPatchList         = 62,
    ThirtyOneControlPointPatchList     = 63,
    ThirtyTwoControlPointPatchList   = 64
};


enum BlendOp
{
    OpAdd    = 1,
    OpSubtract = 2,
    OpRevSubtract    = 3,
    OpMin    = 4,
    OpMax    = 5
};

// Below are directx9 format types.
enum AlphaFunction
{
    BlendZero = 1,
    BlendOne = 2,
    SourceColor = 3,
    InverseSourceColor = 4,
    SourceAlpha = 5,
    InverseSourceAlpha = 6,
    DestinationAlpha = 7,
    InverseDestinationAlpha = 8,
    DestinationColor = 9,
    InverseDestinationColor = 10,
    SourceAlphaSaturated = 11,
    BlendFactor = 14,
    InverseBlendFactor = 15,
    Source1Color    = 16,
    InverseSource1Color    = 17,
    BlendSource1Alpha    = 18,
    BlendInverseSource1Alpha    = 19,
    BlendError = -1
};


enum BlendPipelineType
{
     UnknownBlendPipelineType = -1,
     BlendOver = 0,
     BlendModulate = 1,
     BlendAlpha = 2,
     BlendPipelineOne = 3
     // Others go here.
};

enum CullMode
{
    CullNone,
    CW,
    CCW
};

enum CompareFunction
{
    Never    = 1,
    Less    = 2,
    Equal    = 3,
    LessEqual    = 4,
    Greater    = 5,
    NotEqual    = 6,
    GreaterEqual    = 7,
    Always    = 8
};

enum StencilOp
{
    StencilKeep = 1,
    StencilZero = 2,
    StencilReplace = 3,
    StencilIncrementSat = 4,
    StencilDecrementSat = 5,
    StencilInvert = 6,
    StencilIncrement    = 7,
    StencilDecrement    = 8
};

typedef unsigned char byte;

enum TextureDimension
{
    OneD,
    TwoD,
    CubeMap,
    FourD,
    TwoDHDR,
    ThreeD,
    TextureLUT
};

enum TextureType
{
    FromFile,
    RenderTarget,
    Procedural,
    StagingFromFile
};

enum DeclarationMethod
{
    METHOD_DEFAULT = 0,
    METHOD_PARTIALU,
    METHOD_PARTIALV,
    METHOD_CROSSUV,              // Normal
    METHOD_UV,
    METHOD_LOOKUP,               // Lookup a displacement map
    METHOD_LOOKUPPRESAMPLED,     // Lookup a pre-sampled displacement map
};

typedef enum DeclarationType
{
    FLOAT1    =  0,  // 1D float expanded to (value, 0., 0., 1.)
    FLOAT2    =  1,  // 2D float expanded to (value, value, 0., 1.)
    FLOAT3    =  2,  // 3D float expanded to (value, value, value, 1.)
    FLOAT4    =  3,  // 4D float
    D3DCOLOR  =  4,  // 4D packed unsigned bytes mapped to 0. to 1. range
                     // Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
    UBYTE4    =  5,  // 4D unsigned byte
    SHORT2    =  6,  // 2D signed short expanded to (value, value, 0., 1.)
    SHORT4    =  7,  // 4D signed short
    UBYTE4N   =  8,  // Each of 4 bytes is normalized by dividing to 255.0
    SHORT2N   =  9,  // 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
    SHORT4N   = 10,  // 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
    USHORT2N  = 11,  // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
    USHORT4N  = 12,  // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
    UDEC3     = 13,  // 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
    DEC3N     = 14,  // 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
    FLOAT16_2 = 15,  // Two 16-bit floating point values, expanded to (value, value, 0, 1)
    FLOAT16_4 = 16,  // Four 16-bit floating point values
    UNUSED    = 17,  // When the type field in a decl is unused.
    UINT8     = 18,
    UINT32    = 19
};

typedef enum DeclarationUsage
{
    POSITION = 0,
    BLENDWEIGHT,   // 1
    BLENDINDICES,  // 2
    NORMAL,        // 3
    PSIZE,         // 4
    TEXCOORD,      // 5
    TANGENT,       // 6
    BINORMAL,      // 7
    TESSFACTOR,    // 8
    POSITIONT,     // 9
    COLOR,         // 10
    FOG,           // 11
    DEPTH,         // 12
    SAMPLE,        // 13
    SEED,
    SPEED,
    RAND,
    TYPE,
    STATE,
    OLDSTATE
};

typedef enum TextureFilter
{
    // filtering disabled (valid for mip filter only)
    TEXFILTER_NONE            = 0,    
    // nearest
    TEXFILTER_POINT           = 1,    
    // linear interpolation
    TEXFILTER_LINEAR          = 2,    
    // anisotropic
    TEXFILTER_ANISOTROPIC     = 3,    
    // 4-sample tent
    TEXFILTER_PYRAMIDALQUAD   = 6,    
    // 4-sample gaussian
    TEXFILTER_GAUSSIANQUAD    = 7,    
};

typedef enum ClearType
{
    CLEAR_TARGET            = (int)0x00000001l,
    CLEAR_ZBUFFER           = (int)0x00000002l,
    CLEAR_STENCIL           = (int)0x00000004l,
};

}

#endif