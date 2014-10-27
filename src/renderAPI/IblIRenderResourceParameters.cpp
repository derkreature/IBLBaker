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
#include <IblIRenderResourceParameters.h>
#include <IblApplication.h>

namespace Ibl
{

RenderResourceParameters::RenderResourceParameters(){};
RenderResourceParameters::~RenderResourceParameters(){};

DeviceParameters::DeviceParameters() : 
    RenderResourceParameters(),
    _windowHandle (false),
    _application (0),
    _useSLIIfAvailable (0),
    _windowed (0),
    _requestedSize (0, 0),
    _nvidiaDebugSession (false)
{
};
  
DeviceParameters::DeviceParameters (const Ibl::Application* application,
                                const std::string& applicationName,
                                WindowHandle windowHandle ,
                                bool useConfig,
                                bool windowed,
                                bool multithreaded,
                                const Ibl::Vector2i & requestedSize,
                                bool nvidiaDebugSession) : 
    RenderResourceParameters(),
    _application (application),
    _applicationName (applicationName),
    _windowHandle (windowHandle),
    _windowed (windowed),
    _multithreaded (multithreaded),
    _requestedSize (requestedSize),
    _nvidiaDebugSession (nvidiaDebugSession),
    _useConfig (useConfig)
{
};

DeviceParameters::~DeviceParameters()
{
};

const std::string&
DeviceParameters::applicationName() const
{
    return _applicationName;
}

void
DeviceParameters::setApplication(const Ibl::Application* application) 
{
    _application = application;
}

const Ibl::Application* 
DeviceParameters::application() const 
{
    return _application;
}

WindowHandle 
DeviceParameters::windowHandle() const
{
    return _windowHandle;    
}

bool 
DeviceParameters::multithreaded() const
{
    return _multithreaded;
}

const Ibl::Vector2i& 
DeviceParameters::size() const
{
    return _requestedSize;
}

bool 
DeviceParameters::windowed() const
{
    return _windowed;
}

bool 
DeviceParameters::useConfig() const
{
    return _useConfig;
}

bool 
DeviceParameters::nvidiaDebugSession() const
{
    return _nvidiaDebugSession;
}


TextureParameters::TextureParameters (const std::vector<std::string>& filenames,
                                      TextureDimension                dimension ,
                                      TextureType                     type ,
                                      PixelFormat                     format ,
                                      const Ibl::Vector3i&             bounds,
                                      bool                            generateMipMaps ,
                                      int                             textureCount ,
                                      int                             multiSampleCount ,
                                      int                             multiSampleQuality,
                                      int                             mipLevels,
                                      bool                            useUAV) : 
    RenderResourceParameters(),
    _filenames (filenames),
    _bounds (bounds),
    _format (format),
    _dimension (dimension),
    _type (type),
    _generateMipMaps (generateMipMaps),
    _textureCount (textureCount),
    _multiSampleCount (multiSampleCount),
    _multiSampleQuality (multiSampleQuality),
    _mipLevels (mipLevels),
    _useUAV (useUAV)
{
};

TextureParameters::TextureParameters(const std::string& filename,
                                     TextureDimension   dimension ,
                                     TextureType        type ,
                                     PixelFormat             format ,
                                     const Ibl::Vector3i&      bounds,
                                     bool               generateMipMaps ,
                                     int                textureCount ,
                                     int                multiSampleCount ,
                                     int                multiSampleQuality,
                                     int                mipLevels,
                                     bool               useUAV) : 
    RenderResourceParameters(),
    _bounds(bounds),
    _format (format),
    _dimension (dimension),
    _type (type),
    _generateMipMaps (generateMipMaps),
    _textureCount (textureCount),
    _multiSampleCount (multiSampleCount),
    _multiSampleQuality (multiSampleQuality),
    _mipLevels (mipLevels),
    _useUAV (useUAV)
{
    _filenames.push_back(filename);
};

size_t
TextureParameters::mipLevels() const
{
    return _mipLevels;
}

bool
TextureParameters::useUAV() const
{
    return _useUAV;
}

void
TextureParameters::setWidth(float value)
{
    _bounds.x = (int32_t)(value);
}

void
TextureParameters::setHeight(float value)
{
    _bounds.y = (int32_t)(value);
}

void
TextureParameters::setDepth(float value)
{
    _bounds.z = (int32_t)(value);
}

void
TextureParameters::setNumMipLevels(size_t value)
{
    _mipLevels = value;
}

TextureParameters::TextureParameters (const TextureParameters& data)
{
    _filenames = data._filenames;
    _bounds = data._bounds;
    _format = data._format;
    _dimension = data._dimension;
    _type = data._type;
    _generateMipMaps = data._generateMipMaps;
    _textureCount = data._textureCount;
    _multiSampleCount = data._multiSampleCount;
    _multiSampleQuality = data._multiSampleQuality;
    _mipLevels = data._mipLevels;
    _useUAV = data._useUAV;
}


TextureParameters::~TextureParameters()
{
};

const std::vector<std::string>&
TextureParameters::filenames() const
{
    return _filenames;
}

uint32_t
TextureParameters::width() const {
    return _bounds.x;
}

uint32_t
TextureParameters::height() const {
    return _bounds.y;
}

uint32_t
TextureParameters::depth() const {
    return _bounds.z;
}

PixelFormat                    
TextureParameters::format() const {
    return _format;
}

void
TextureParameters::setFormat(PixelFormat format) {
    _format = format;
}

int
TextureParameters::type() const {
    return _type;
}

TextureDimension        
TextureParameters::dimension() const {
    return _dimension;
}

bool                    
TextureParameters::generateMipMaps() const {
    return _generateMipMaps;
}

int                     
TextureParameters::textureCount() const {
    return _textureCount;
}

int                
TextureParameters::multiSampleCount() const {
    return _multiSampleCount;
}

int                     
TextureParameters::multiSampleQuality() const {
    return _multiSampleQuality;
}

DepthSurfaceParameters::DepthSurfaceParameters (PixelFormat format ,
                                                uint32_t    width ,
                                                uint32_t    height ,
                                                int         slices,
                                                bool        createFromBackBuffer ,
                                                int         multiSampleCount  ,
                                                int         multiSampleQuality,
                                                bool        bindable) : 
RenderResourceParameters(),
_width (width),
_height (height),
_format (format),
_createFromBackBuffer (createFromBackBuffer),
_multiSampleCount (multiSampleCount),
_multiSampleQuality (multiSampleQuality),
_bindable (bindable),
_slices (slices)
{
};

DepthSurfaceParameters::DepthSurfaceParameters  (const DepthSurfaceParameters & data)
{
    _width = data._width;
    _height = data._height;
    _format = data._format;
    _multiSampleCount = data._multiSampleCount;
    _multiSampleQuality = data._multiSampleQuality;
    _createFromBackBuffer = data._createFromBackBuffer;
    _bindable = data._bindable;
    _slices = data._slices;
}

DepthSurfaceParameters::~DepthSurfaceParameters() {};

uint32_t                        
DepthSurfaceParameters::width() const {
    return _width;
}

uint32_t                        
DepthSurfaceParameters::height() const {
    return _height;
}

PixelFormat
DepthSurfaceParameters::format() const {
    return _format;
}

int                     
DepthSurfaceParameters::slices() const {
    return _slices;
}

bool                    
DepthSurfaceParameters::createFromBackBuffer() const {
    return _createFromBackBuffer;
}

int                     
DepthSurfaceParameters::multiSampleQuality() const {
    return _multiSampleQuality;
}

int                     
DepthSurfaceParameters::multiSampleCount() const {
    return _multiSampleCount;
}

bool                    
DepthSurfaceParameters::bindable() const {
    return _bindable;
}

void                    
DepthSurfaceParameters::setWidth (int width) {
    _width = width;
}

void                    
DepthSurfaceParameters::setHeight (int height) {
    _height = height;
}

IndexBufferParameters::IndexBufferParameters(uint32_t sizeInBytesVal) 
{
    _sizeInBytes = sizeInBytesVal;
};

IndexBufferParameters::IndexBufferParameters (const IndexBufferParameters& in)
{
    _sizeInBytes =     in.sizeInBytes();
};

unsigned int                
IndexBufferParameters::sizeInBytes() const { return _sizeInBytes;}

VertexBufferParameters::VertexBufferParameters(uint32_t sizeInBytesVal, 
                     bool streamOut , 
                     uint32_t vertexStride , 
                     void* vertexPtr , 
                     bool useResource , 
                     bool dynamic,
                     bool bindShaderResource ,
                     bool bindRenderTarget ,
                     Ibl::PixelFormat format) 
{
    _bindUAV = false;
    _sizeInBytes = sizeInBytesVal;
    _streamOut = streamOut;
    _vertexPtr = vertexPtr;
    _vertexStride = vertexStride;
    _useResource = useResource;
    _dynamic = dynamic;
    _bindShaderResource = bindShaderResource;
    _bindRenderTarget = bindRenderTarget;
    _format = format;
};

VertexBufferParameters::VertexBufferParameters() 
{
    _bindUAV = false;
    _sizeInBytes = 0;
    _streamOut = false;
    _vertexPtr = nullptr;
    _vertexStride = 0;
    _useResource = false;
    _dynamic = false;
    _bindShaderResource = false;
    _bindRenderTarget = false;
    _format = Ibl::PF_UNKNOWN;
};

VertexBufferParameters::VertexBufferParameters (const VertexBufferParameters& in) 
{
    _sizeInBytes =     in._sizeInBytes;
    _streamOut = in._streamOut;
    _vertexStride = in._vertexStride;
    _vertexPtr = in._vertexPtr;
    _dynamic = in._dynamic;
    _useResource = in._useResource;
    _bindShaderResource = in._bindShaderResource;
    _bindRenderTarget = in._bindRenderTarget;
    _format = in._format;
    _bindUAV = in._bindUAV;
};

VertexBufferParameters::~VertexBufferParameters() {}

void
VertexBufferParameters::setBindUAV(bool value)
{
    _bindUAV = value;
}

bool
VertexBufferParameters::bindUAV() const
{
    return _bindUAV;
}

uint32_t                      
VertexBufferParameters::vertexStride() const { return _vertexStride; }
void*                       
VertexBufferParameters::vertexPtr() const { return _vertexPtr; }

unsigned int                
VertexBufferParameters::sizeInBytes() const { return _sizeInBytes;}
bool                        
VertexBufferParameters::bindStreamOut () const { return _streamOut; }

bool                        
VertexBufferParameters::dynamic() const { return _dynamic; }

bool                        
VertexBufferParameters::useResource() const { return _useResource; }

bool                        
VertexBufferParameters::bindShaderResource() const { return _bindShaderResource;}
bool                        
VertexBufferParameters::bindRenderTarget() const { return _bindRenderTarget;}

Ibl::PixelFormat           
VertexBufferParameters::format() const { return _format; }



GpuBufferParameters::GpuBufferParameters (PixelFormat format, /* size is implicit based on format_width * count */
                                          uint32_t count, 
                                          void* streamPtr , 
                                          bool dynamic ,
                                          uint32_t elementWidth ,
                                          bool bindVertexBuffer ,
                                          bool bindStreamOutput ,
                                          bool bindRenderTarget ,
                                          bool bindShaderResource ,
                                          bool defaultMemoryUsage ,
                                          bool bindConstantBuffer ,
                                          uint32_t byteWidth) 
{
    _format = format;
    _elementCount = count;
    _streamPtr = streamPtr;
    _dynamic = dynamic;
    _elementWidth = elementWidth;
    _bindVertexBuffer = bindVertexBuffer;
    _bindStreamOutput = bindStreamOutput;
    _bindRenderTarget = bindRenderTarget;
    _bindShaderResource = bindShaderResource;
    _defaultMemoryUsage = defaultMemoryUsage;
    _bindConstantBuffer = bindConstantBuffer;
    _byteWidth = byteWidth;

    // Uav access is explict and occurs outside of the
    // constructor.
    _unorderedAccess = false;
    _bindUnorderedAccessView = false;
    _allowRawView = false;
    _uavCounter = false;
    _uavFlagRaw = false;
    _constructStructured = false;
    _sizeBasedOnBackBuffer = false;
    _formatWidth = 0;
    _elementSizeMultiplier = 1;
    _drawIndirect = false;
    _uavAppend = false;
};

GpuBufferParameters::GpuBufferParameters () 
{
    _format = Ibl::PF_FLOAT32_R;
    _elementCount = 0;
    _streamPtr = 0; 
    _dynamic = 0;
    _elementWidth = 0;
    _bindVertexBuffer = false;
    _bindStreamOutput = false;
    _bindRenderTarget = false;
    _bindShaderResource = true;
    _defaultMemoryUsage = false;
    _bindConstantBuffer = false;
    _byteWidth = 0;

    // TODO, UAV Setup needs it's own helper function
    _unorderedAccess = false;
    _bindUnorderedAccessView = false;
    _allowRawView = false;
    _uavCounter = false;
    _uavFlagRaw = false;
    _constructStructured = false;
    _sizeBasedOnBackBuffer = false;
    _formatWidth = 0;
    _elementSizeMultiplier = 1;
    _drawIndirect = false;
    _uavAppend = false;
};

GpuBufferParameters::GpuBufferParameters (const GpuBufferParameters& in) 
{
    _format = in.format();
    _elementCount = in.elementCount();
    _streamPtr = in.streamPtr();
    _dynamic = in.dynamic();
    _elementWidth = in.elementWidth();
    _bindVertexBuffer = in.bindVertexBuffer();
    _bindStreamOutput = in.bindStreamOutput();
    _bindRenderTarget = in.bindRenderTarget();
    _bindShaderResource = in.bindShaderResource();
    _defaultMemoryUsage = in.defaultMemoryUsage();
    _bindConstantBuffer = in.bindConstantBuffer();
    _byteWidth = in.byteWidth();
    _unorderedAccess = in.unorderedAccess();
    _bindUnorderedAccessView = in.bindUnorderedAccessView();
    _allowRawView = in.allowRawView();
    _uavCounter = in.uavCounter();
    _uavFlagRaw = in.uavFlagRaw();
    _constructStructured = in.constructStructured();
    _sizeBasedOnBackBuffer = in.sizeBasedOnBackBuffer();
    _formatWidth = in.formatWidth();
    _elementSizeMultiplier = in.elementSizeMultiplier();
    _drawIndirect = in.drawIndirect();
    _uavAppend = in.uavAppend();
}; 

GpuBufferParameters::~GpuBufferParameters() 
{
}

GpuBufferParameters
GpuBufferParameters::setupDrawIndirectBuffer(size_t byteWidth, void* streamPtr)
{
    GpuBufferParameters resource;

    resource._format = Ibl::PF_UNKNOWN;
    resource._elementCount = 1;
    resource._streamPtr = streamPtr;
    resource._dynamic = false;
    resource._elementWidth = false;
    resource._bindVertexBuffer = false;
    resource._bindStreamOutput = false;
    resource._bindRenderTarget = false;
    resource._bindShaderResource = false;
    resource._defaultMemoryUsage = true;
    resource._bindConstantBuffer = false;
    resource._byteWidth = uint32_t(byteWidth);
    resource._drawIndirect = true;

    // Uav access 
    resource._unorderedAccess = false;
    resource._bindUnorderedAccessView = false;
    resource._allowRawView = false;
    resource._uavCounter = false;
    resource._uavFlagRaw = false;
    resource._constructStructured = false;
    resource._sizeBasedOnBackBuffer = false;
    resource._formatWidth = uint32_t(byteWidth);
    resource._elementSizeMultiplier = 1;

    return resource;
}

GpuBufferParameters
GpuBufferParameters::setupConstantBuffer(size_t byteWidth)
{
    GpuBufferParameters resource;

    resource._format = Ibl::PF_UNKNOWN;
    resource._elementCount = 1;
    resource._streamPtr = nullptr;
    resource._dynamic = true;
    resource._elementWidth = false;
    resource._bindVertexBuffer = false;
    resource._bindStreamOutput = false;
    resource._bindRenderTarget = false;
    resource._bindShaderResource = false;
    resource._defaultMemoryUsage = false;
    resource._bindConstantBuffer = true;
    resource._byteWidth = uint32_t(byteWidth);
    resource._drawIndirect = false;

    // Uav access 
    resource._unorderedAccess = false;
    resource._bindUnorderedAccessView = false;
    resource._allowRawView = false;
    resource._uavCounter = false;
    resource._uavFlagRaw = false;
    resource._constructStructured = false;
    resource._sizeBasedOnBackBuffer = false;
    resource._formatWidth = uint32_t(byteWidth);
    resource._elementSizeMultiplier = 1;

    return resource;
}

bool
GpuBufferParameters::drawIndirect() const
{
    return _drawIndirect;
}

void*                       
GpuBufferParameters::streamPtr() const 
{ 
    return _streamPtr; 
}

bool                        
GpuBufferParameters::dynamic() const 
{ 
    return _dynamic; 
}

uint32_t                      
GpuBufferParameters::elementCount() const
{ 
    return _elementCount; 
}
uint32_t                      
GpuBufferParameters::elementWidth() const 
{ 
    return _elementWidth; 
}

PixelFormat
GpuBufferParameters::format() const 
{
    return _format; 
}

bool                        
GpuBufferParameters::bindVertexBuffer() const 
{ 
    return _bindVertexBuffer; 
}

bool                        
GpuBufferParameters::bindRenderTarget() const 
{
    return _bindRenderTarget; 
}
bool                        
GpuBufferParameters::bindStreamOutput() const
{ 
    return _bindStreamOutput; 
}
bool                        
GpuBufferParameters::bindShaderResource() const 
{ 
    return _bindShaderResource; 
}

bool                        
GpuBufferParameters::defaultMemoryUsage() const
{
    return _defaultMemoryUsage; 
}

bool                        
GpuBufferParameters::bindConstantBuffer() const 
{
    return _bindConstantBuffer; 
};

uint32_t                      
GpuBufferParameters::byteWidth() const
{ 
    return _byteWidth; 
}

bool                        
GpuBufferParameters::unorderedAccess() const 
{ 
    return _unorderedAccess; 
}

bool                        
GpuBufferParameters::bindUnorderedAccessView() const
{ 
    return _bindUnorderedAccessView; 
}

bool                        
GpuBufferParameters::allowRawView() const
{ 
    return _allowRawView; 
}

bool                        
GpuBufferParameters::uavCounter() const 
{ 
    return _uavCounter;
}

bool                        
GpuBufferParameters::uavAppend() const 
{ 
    return _uavAppend;
}

bool                        
GpuBufferParameters::uavFlagRaw() const
{
    return _uavFlagRaw;
}

bool                        
GpuBufferParameters::constructStructured() const 
{
    return _constructStructured; 
}

uint32_t                      
GpuBufferParameters::formatWidth() const
{ 
    return _formatWidth; 
}

bool                        
GpuBufferParameters::sizeBasedOnBackBuffer() const
{
    return _sizeBasedOnBackBuffer;
}

uint32_t                      
GpuBufferParameters::elementSizeMultiplier() const
{ 
    return _elementSizeMultiplier; 
}

void                        
GpuBufferParameters::setUnorderedAccess(bool value) 
{ 
    _unorderedAccess = value;
}

void                        
GpuBufferParameters::setBindUnorderedAccessView(bool value)
{ 
    _bindUnorderedAccessView = value; 
}

void                        
GpuBufferParameters::setAllowRawView(bool value)
{
    _allowRawView = value; 
}

void
GpuBufferParameters::setUavAppend(bool value)
{ 
    _uavAppend = value;
}

void                        
GpuBufferParameters::setUavCounter(bool value)
{ 
    _uavCounter = value; 
}

void                        
GpuBufferParameters::setUavFlagRaw(bool value) 
{ 
    _uavFlagRaw = value;
}

void                        
GpuBufferParameters::setConstructStructured(bool value) 
{
    _constructStructured = value; 
}

void                        
GpuBufferParameters::setSizeBasedOnBackBuffer(bool value) 
{ 
    _sizeBasedOnBackBuffer = value;
}

void                        
GpuBufferParameters::setFormatWidth(uint32_t value)
{ 
    _formatWidth = value; 
}

void                        
GpuBufferParameters::setElementSizeMultiplier (uint32_t value) 
{ 
    _elementSizeMultiplier = value; 
}

VertexDeclarationParameters::VertexDeclarationParameters(const std::vector<VertexElement>& vertexElements)        
{
        for (uint32_t i = 0; i < vertexElements.size(); i++)
        {
            _elements.push_back (VertexElement(vertexElements[i]));
        }
}

VertexDeclarationParameters::VertexDeclarationParameters (const VertexDeclarationParameters& in) 
{
        for (uint32_t i = 0; i < in._elements.size(); i++)
        {
            _elements.push_back (VertexElement(in._elements[i]));
        }
};

VertexDeclarationParameters::~VertexDeclarationParameters()
{
    _elements.clear();
}

const std::vector<VertexElement>& 
VertexDeclarationParameters::elements() const 
{
        return _elements;
}

}