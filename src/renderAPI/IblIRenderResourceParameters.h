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

#ifndef INCLUDED_RENDERRESOURCE_INIT_DATA
#define INCLUDED_RENDERRESOURCE_INIT_DATA

#include <IblPlatform.h>
#include <IblRenderEnums.h>
#include <IblVertexElement.h>
#include <IblVector2.h>
#include <IblVector3.h>

namespace Ibl
{
class Application;

class RenderResourceParameters
{
  public:
    RenderResourceParameters();
    virtual ~RenderResourceParameters();
};

class DeviceParameters : public RenderResourceParameters
{
  public:
    DeviceParameters();      
    DeviceParameters(const Ibl::Application* application,
                     const std::string& applicationName,
                     WindowHandle windowHandle = 0,
                     bool useConfig = false,
                     bool windowed = false,
                     bool multithreaded = false,
                     const Ibl::Vector2i & requestedSize = Ibl::Vector2i (0, 0),
                     bool nvidiaDebugSession = false);
    
    virtual ~DeviceParameters();

    void                       setApplication(const Ibl::Application* application);
    const Ibl::Application*     application() const;
    
    WindowHandle               windowHandle() const;
    bool                       multithreaded() const;

    const Ibl::Vector2i&       size() const;    
    bool                       windowed() const;
    
    bool                       nvidiaDebugSession() const;
    bool                       useConfig() const;

    const std::string&         applicationName() const;

  private:  
    const Ibl::Application*     _application;
    WindowHandle               _windowHandle;
    bool                       _useConfig;
    bool                       _windowed;
    Ibl::Vector2i               _requestedSize;
    bool                       _nvidiaDebugSession;
    bool                       _useSLIIfAvailable;
    bool                       _multithreaded;
    std::string                _applicationName;
};

class TextureParameters : public RenderResourceParameters
{
  public:
    TextureParameters (const std::string& filename,
                       TextureDimension   dimension ,
                       TextureType        type = Ibl::FromFile,
                       PixelFormat             format = Ibl::PF_A8R8G8B8,
                       const Ibl::Vector3i & bounds = Ibl::Vector3i(0,0,0),
                       bool               generateMipMaps = false,
                       int                textureCount = 1,
                       int                multiSampleCount = 1,
                       int                multiSampleQuality = 0,
                       int                mipLevels = 1,
                       bool               useUAV = false);

    TextureParameters (const std::vector<std::string>& filenames,
                       TextureDimension   dimension ,
                       TextureType        type = Ibl::FromFile,
                       PixelFormat             format = Ibl::PF_A8R8G8B8,
                       const Ibl::Vector3i & bounds = Ibl::Vector3i(0,0,0),
                       bool               generateMipMaps = false,
                       int                textureCount = 1,
                       int                multiSampleCount = 1,
                       int                multiSampleQuality = 0,
                       int                mipLevels = 1,
                       bool               useUAV = false);

    TextureParameters (const TextureParameters& data);
    virtual ~TextureParameters();

    const std::vector<std::string>&    filenames() const;

    uint32_t                width() const;
    uint32_t                height() const;
    uint32_t                depth() const;
    PixelFormat             format() const;
    void                    setFormat(PixelFormat format) ;

    int                     type () const;

    TextureDimension        dimension() const;

    bool                    generateMipMaps() const;

    int                     textureCount() const;

    int                     multiSampleCount() const;

    int                     multiSampleQuality() const;

    size_t                  mipLevels() const;
    bool                    useUAV() const;

    void                    setWidth(float);
    void                    setHeight(float);
    void                    setDepth(float);
    void                    setNumMipLevels(size_t);

  private:
    std::vector<std::string> _filenames;
    Ibl::Vector3i             _bounds;
    PixelFormat              _format;
    TextureDimension         _dimension;    
    TextureType              _type;
    int                      _textureCount;
    size_t                   _mipLevels;

    int                      _multiSampleCount;
    int                      _multiSampleQuality;
    bool                     _generateMipMaps;
    bool                     _useUAV;
};

class DepthSurfaceParameters  : public RenderResourceParameters
{
  public:
    DepthSurfaceParameters(PixelFormat format = Ibl::PF_DEPTH32,
                           uint32_t     width = 0,
                           uint32_t     height = 0,
                           int    slices = 1,
                           bool    createFromBackBuffer = true,
                           int    multiSampleCount  = 1,
                           int    multiSampleQuality = 0,
                           bool   bindable = false);
    DepthSurfaceParameters  (const DepthSurfaceParameters & data);

    virtual ~DepthSurfaceParameters();

    uint32_t                width() const;
    uint32_t                height() const;

    PixelFormat             format() const;

    int                     slices() const;

    bool                    createFromBackBuffer() const;

    int                     multiSampleQuality() const;

    int                     multiSampleCount() const;

    bool                    bindable() const;

    void                    setWidth (int width);

    void                    setHeight (int height);

  private:
    uint32_t                _width;
    uint32_t                _height;
    PixelFormat             _format;
    bool                    _createFromBackBuffer;
    int                     _multiSampleQuality;
    int                     _multiSampleCount;
    bool                    _bindable;
    int                     _slices;
};

class IndexBufferParameters : public RenderResourceParameters
{    
  public:
    IndexBufferParameters(uint32_t sizeInBytesVal);

    IndexBufferParameters (const IndexBufferParameters& in);

    unsigned int                sizeInBytes() const;
  private:
    unsigned                    _sizeInBytes;
};

class VertexBufferParameters : public RenderResourceParameters
{  
  public:
    VertexBufferParameters(uint32_t sizeInBytesVal, 
                         bool streamOut = false, 
                         uint32_t vertexStride = 0, 
                         void* vertexPtr = 0, 
                         bool useResource = false, 
                         bool dynamic = true,
                         bool bindShaderResource = false,
                         bool bindRenderTarget = false,
                         Ibl::PixelFormat format = Ibl::PF_UNKNOWN);
    VertexBufferParameters();

    VertexBufferParameters (const VertexBufferParameters& in);

    virtual ~VertexBufferParameters();

    uint32_t                      vertexStride() const;
    void*                       vertexPtr() const;
    unsigned int                sizeInBytes() const;
    bool                        bindStreamOut () const;
    bool                        dynamic() const;
    bool                        useResource() const;

    bool                        bindShaderResource() const;
    bool                        bindRenderTarget() const;

    bool                        bindUAV() const;
    void                        setBindUAV(bool);


    Ibl::PixelFormat           format() const;
  private:
    unsigned int                _sizeInBytes;
    bool                        _streamOut;
    uint32_t                    _vertexStride;
    void*                       _vertexPtr;
    bool                        _dynamic;
    bool                        _useResource;
    bool                        _bindShaderResource;
    bool                        _bindRenderTarget;
    Ibl::PixelFormat           _format;
    bool                        _bindUAV;
};

class GpuBufferParameters : public RenderResourceParameters
{  
  public:
    // Catch all constructor
    GpuBufferParameters (PixelFormat format, /* size is implicit based on format_width * count */
                         uint32_t count, 
                         void* streamPtr = 0, 
                         bool dynamic = true,
                         uint32_t elementWidth = 1,
                         bool bindVertexBuffer = false,
                         bool bindStreamOutput = false,
                         bool bindRenderTarget = false,
                         bool bindShaderResource = true,
                         bool defaultMemoryUsage = false,
                         bool bindConstantBuffer = false,
                         uint32_t byteWidth = 0);

    GpuBufferParameters ();
    GpuBufferParameters (const GpuBufferParameters& in); 
    virtual ~GpuBufferParameters();

    static GpuBufferParameters  setupConstantBuffer(size_t byteWidth);    
    static GpuBufferParameters setupDrawIndirectBuffer(size_t byteWidth, void* streamPtr);

    void*                       streamPtr() const ;
    bool                        dynamic() const ;
    uint32_t                    elementCount() const ;
    uint32_t                    elementWidth() const ;
    PixelFormat                 format() const ;

    bool                        bindVertexBuffer() const;
    bool                        bindRenderTarget() const;
    bool                        bindStreamOutput() const;
    bool                        bindShaderResource() const;
    bool                        defaultMemoryUsage() const;
    bool                        bindConstantBuffer() const;
    bool                        drawIndirect() const;    
    uint32_t                    byteWidth() const ;

    bool                        unorderedAccess() const ;
    bool                        bindUnorderedAccessView() const ;
    bool                        allowRawView() const ;
    bool                        uavCounter() const ;
    bool                        uavFlagRaw() const ;
    bool                        uavAppend() const;

    bool                        constructStructured() const ;
    uint32_t                      formatWidth() const ;
    bool                        sizeBasedOnBackBuffer() const ;
    uint32_t                      elementSizeMultiplier() const ;

    void                        setUnorderedAccess(bool value);
    void                        setBindUnorderedAccessView(bool value) ;
    void                        setAllowRawView(bool value) ;
    void                        setUavCounter(bool value) ;
    void                        setUavFlagRaw(bool value) ;
    void                        setUavAppend(bool);
    void                        setConstructStructured(bool value);
    void                        setSizeBasedOnBackBuffer(bool value) ;
    void                        setFormatWidth(uint32_t value) ;
    void                        setElementSizeMultiplier (uint32_t value);



  private:
    PixelFormat                 _format;
    uint32_t                    _elementCount;
    uint32_t                    _elementWidth;

    void*                       _streamPtr;
    bool                        _dynamic;
    bool                        _bindVertexBuffer;
    bool                        _bindShaderResource;
    bool                        _bindRenderTarget;
    bool                        _bindStreamOutput;
    bool                        _defaultMemoryUsage;
    bool                        _bindConstantBuffer;
    bool                        _drawIndirect;
    
    uint32_t                    _byteWidth;

    
    // Unordered access view flags
    bool                        _unorderedAccess;
    bool                        _bindUnorderedAccessView;
    bool                        _allowRawView;
    bool                        _uavCounter;
    bool                        _uavFlagRaw;
    bool                        _constructStructured;
    bool                        _uavAppend;

    // If format is unknown, then this width is used.
    uint32_t                    _formatWidth; 
    uint32_t                    _elementSizeMultiplier;
    // Screen space unordered access.
    bool                        _sizeBasedOnBackBuffer;
};

class VertexDeclarationParameters : public RenderResourceParameters
{
  public:
    VertexDeclarationParameters(const std::vector<VertexElement>& vertexElements);
    VertexDeclarationParameters (const VertexDeclarationParameters& in);

    virtual ~VertexDeclarationParameters();

    const std::vector<VertexElement>& elements() const;

  private:
    std::vector<VertexElement> _elements;
};

}

#endif