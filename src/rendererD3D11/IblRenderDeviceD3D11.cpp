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

#include <IblRenderDeviceD3D11.h>
#include <IblApplication.h>
#include <IblGpuTechnique.h>
#include <IblSurfaceD3D11.h>
#include <IblDepthSurfaceD3D11.h>
#include <IblBackBufferSurfaceD3D11.h>
#include <IblRenderTargetTextureD3D11.h>
#include <IblBufferD3D11.h>
#include <IblLog.h>
#include <IblVertexBufferD3D11.h>
#include <IblIndexBufferD3D11.h>
#include <IblRenderWindowD3D11.h>
#include <IblPostEffect.h>
#include <IblVertexDeclarationD3D11.h>
#include <IblShaderD3D11.h>
#include <IblRenderTargetTextureD3D11.h>
#include <IblComputeShaderD3D11.h>
#include <IblTexture2DD3D11.h>
#include <IblColorResolve.h>
#include <IblDepthResolve.h>

namespace Ibl
{
namespace 
{
__inline void logDXGIError (HRESULT error)
{
    switch (error)
    {
    case DXGI_ERROR_DEVICE_HUNG:
        LOG (" DXGI_ERROR_DEVICE_HUNG: The application's device failed due to badly formed commands sent by the application. This is a design-time issue that should be investigated and fixed.");
        break;
    case DXGI_ERROR_DEVICE_REMOVED:
        LOG("DXGI_ERROR_DEVICE_REMOVED: The video card has been physically removed from the system, or a driver upgrade for the video card has occurred. The application should destroy and recreate the device. For help debugging the problem, call ID3D10Device::GetDeviceRemovedReason.");
        break;
    case DXGI_ERROR_DEVICE_RESET:
        LOG ("DXGI_ERROR_DEVICE_RESET: The device failed due to a badly formed command. This is a run-time issue; The application should destroy and recreate the device. ");
        break;
    case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
        LOG ("DXGI_ERROR_DRIVER_INTERNAL_ERROR The driver encountered a problem and was put into the device removed state. ");
        break;
    case DXGI_ERROR_FRAME_STATISTICS_DISJOINT:
        LOG ("DXGI_ERROR_FRAME_STATISTICS_DISJOINT: The requested functionality is not supported by the device or the driver."); 
        break;
    case DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE:
        LOG ("DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE: The requested functionality is not supported by the device or the driver.");
        break;
    case DXGI_ERROR_INVALID_CALL: 
        LOG("DXGI_ERROR_INVALID_CALL: The application provided invalid parameter data; this must be debugged and fixed before the application is released.");
        break;
    case DXGI_ERROR_MORE_DATA:
        LOG ("DXGI_ERROR_MORE_DATA: The buffer supplied by the application is not big enough to hold the requested data.");
        break;
    case DXGI_ERROR_NONEXCLUSIVE: 
        LOG ("DXGI_ERROR_NONEXCLUSIVE: The application attempted to acquire exclusive ownership of an output, but failed because some other application (or device within the application) has already acquired ownership."); 
        break;
    case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE: 
        LOG ("DXGI_ERROR_NOT_CURRENTLY_AVAILABLE: The requested functionality is not supported by the device or the driver.");
        break;
    case DXGI_ERROR_NOT_FOUND: 
        LOG ("DXGI_ERROR_NOT_FOUND: When calling IDXGIObject::GetPrivateData, the GUID passed in is not recognized as one previously passed to IDXGIObject::SetPrivateData or IDXGIObject::SetPrivateDataInterface. When calling IDXGIFactory::EnumAdapters or IDXGIAdapter::EnumOutputs, the enumerated ordinal is out of range.");
        break;
    case DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED: 
        LOG ("DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED: The application's remote device has been removed due to session disconnect or network disconnect. The application should call IDXGIFactory1::IsCurrent to find out when the remote device becomes available again.");
        break;
    case DXGI_ERROR_REMOTE_OUTOFMEMORY: 
        LOG (" DXGI_ERROR_REMOTE_OUTOFMEMORY: The application's remote device has failed due to lack of memory or machine error. The application should destroy and recreate resources using less memory.");
        break;
    case DXGI_ERROR_WAS_STILL_DRAWING: 
        LOG ("DXGI_ERROR_WAS_STILL_DRAWING: The device was busy, and did not schedule the requested task. This error only applies to asynchronous queries in Direct3D 10 (see ID3D10Asynchronous Interface).");
        break;
    case DXGI_ERROR_UNSUPPORTED: 
        LOG ("DXGI_ERROR_UNSUPPORTED: The requested functionality is not supported by the device or the driver.");
        break;
    default:
        LOG ("Unknown DXGI_ERROR");
    }
}
}


DXGIFactory::DXGIFactory() :
    _factory (0),
    _D3D11Available (false)
{
    initialize();
}

DXGIFactory::~DXGIFactory()
{
    saferelease (_factory);
}

void
DXGIFactory::initialize()
{
    IDXGIFactory1 * factory = 0;
    CreateDXGIFactory1( __uuidof( IDXGIFactory1 ), (LPVOID*)&factory );
    setFactory (factory);
    setD3D11Available(factory != 0);
}

void
DXGIFactory::setD3D11Available (bool available)
{
    _D3D11Available = available;
}

void
DXGIFactory::setFactory (IDXGIFactory1* factory)
{
    _factory = factory;
}

bool
DXGIFactory::D3D11Available() const
{
    return _D3D11Available;
}

IDXGIFactory1*
DXGIFactory::factory()
{
    return _factory;
}

DeviceSettingsD3D11::DeviceSettingsD3D11()
{
    memset (this, 0, sizeof (DeviceSettingsD3D11));
}

DeviceSettingsD3D11::DeviceSettingsD3D11(const DeviceSettingsD3D11& other)
{
    memset (this, 0, sizeof (DeviceSettingsD3D11));
    _adapterOrdinal                 = other._adapterOrdinal;               
    _output                         = other._output;                         
    _driverType                     = other._driverType;                     
    _sd.BufferDesc.Width            = other._sd.BufferDesc.Width;            
    _sd.BufferDesc.Height           = other._sd.BufferDesc.Height;           
    _sd.BufferDesc.Format           = other._sd.BufferDesc.Format;           
    _sd.BufferUsage                 = other._sd.BufferUsage ;                
    _sd.BufferCount                 = other._sd.BufferCount ;                
    _sd.SampleDesc.Count            = other._sd.SampleDesc.Count ;           
    _sd.SampleDesc.Quality          = other._sd.SampleDesc.Quality ;         
    _sd.SwapEffect                  = other._sd.SwapEffect;                  
    _sd.OutputWindow                = other._sd.OutputWindow;                
    _sd.Windowed                    = other._sd.Windowed;                    
    _sd.BufferDesc.RefreshRate      = other._sd.BufferDesc.RefreshRate;
    _sd.Flags                       = other._sd.Flags;                  
    _syncInterval                   = other._syncInterval;
    _autoCreateDepthStencil         = other._autoCreateDepthStencil;         
    _autoDepthStencilFormat         = other._autoDepthStencilFormat;
    _createFlags                    = other._createFlags;      
    _sampleDesc.Count               = other._sampleDesc.Count;
    _sampleDesc.Quality             = other._sampleDesc.Quality;
}

DeviceD3D11::~DeviceD3D11()
{
    safedelete(_backbuffer);
    safedelete(_depthbuffer);


    // Free internal managers and shaders.
    free();

    if (_immediateCtx)
    {
        _immediateCtx->ClearState();
        saferelease(_immediateCtx);
    }

    saferelease (_adapter);
    saferelease (_swapChain);
    saferelease (_direct3d);

    saferelease(_currentRasterState);
    saferelease(_currentDepthState);
    saferelease(_currentBlendState);
    
    safedelete (_factory);
    safedelete (_window);
}

bool
DeviceD3D11::initialize (const Ibl::ApplicationRenderParameters& deviceParameters)
{
    IDevice::initialize(deviceParameters);

    Ibl::RenderWindowD3D11* window = 
        new Ibl::RenderWindowD3D11 (_application, this);

    if (window->create (deviceParameters.application(),
                        deviceParameters.applicationName(),
                        deviceParameters.windowed(),
                        deviceParameters.size().x,
                        deviceParameters.size().y))
    {
        _window = window;
        _creationParameters._sd.OutputWindow = _window->windowHandle();
    }
    else
    {
        safedelete (window);
        THROW ("Failed to create render window");
    }

    _creationParameters._adapterOrdinal = 0;
    _creationParameters._autoCreateDepthStencil = true;
    _creationParameters._autoDepthStencilFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    _creationParameters._driverType = D3D_DRIVER_TYPE_HARDWARE;
    _creationParameters._createFlags = 0;
    _creationParameters._output = 0;
    _creationParameters._sampleDesc.Count = 1;
    _creationParameters._sampleDesc.Quality = 0;      
    _creationParameters._sd.BufferCount = 3;
    _creationParameters._sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    _creationParameters._sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    _creationParameters._sd.BufferDesc.RefreshRate.Numerator = 60;                    
    _creationParameters._sd.BufferDesc.RefreshRate.Denominator = 1;                    
    _creationParameters._sd.BufferDesc.Width = window->clientRect().size().x;
    _creationParameters._sd.BufferDesc.Height = window->clientRect().size().y;        
    _creationParameters._sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    _creationParameters._sd.Windowed = deviceParameters.windowed();
    
    if (!_window)
    {
        THROW ("Could not create render window");
    }

    //----------------------------
    // Create the direct3d device.
    //----------------------------
    HRESULT hr = S_OK;
    IDXGIFactory1* pDXGIFactory = _factory->factory();
    pDXGIFactory->MakeWindowAssociation(_window->windowHandle(), DXGI_MWA_NO_WINDOW_CHANGES);

    // Try to create the device with the chosen settings
    hr = S_OK;
    if((pDXGIFactory->EnumAdapters1 (_creationParameters._adapterOrdinal, &_adapter )) != S_OK)
    {
        THROW ("Failed to enum D3D11 device...\n");
    }
    
    _level = D3D_FEATURE_LEVEL_10_0;
    //_multiSampleCount = _creationParameters->SampleDesc.Count;
    //_multiSampleQuality = _creationParameters->SampleDesc.Quality;

    //LOG ("multisampleCount = " << multiSampleCount() << " " <<
    //    "multiSampleQuality = " << multiSampleQuality() << "\n");

    _creationParameters._sd.SampleDesc.Count = 1;
    _creationParameters._sd.SampleDesc.Quality = 0;
    if (D3D11CreateDeviceAndSwapChain (_adapter, /*pAdapter nullptr*/ 
                                       D3D_DRIVER_TYPE_UNKNOWN, 
                                       0,
                                       0,//_creationParameters->CreateFlags,
                                       0, 0, D3D11_SDK_VERSION, 
                                       &_creationParameters._sd, &_swapChain, &_direct3d, &_level, &_immediateCtx) != S_OK)
    {
        THROW ("Failed to create D3D11 device...\n");
    }
    else
    {
        std::string d3dFeatureLevel = "UKNOWN";
        switch (_level)
        {
            case D3D_FEATURE_LEVEL_9_1:
                d3dFeatureLevel = "Direct3d 9.1";
                break;
	        case D3D_FEATURE_LEVEL_9_2:
                d3dFeatureLevel = "Direct3d 9.2";
                break;
	        case D3D_FEATURE_LEVEL_9_3:
                d3dFeatureLevel = "Direct3d 9.3";
                break;
            case D3D_FEATURE_LEVEL_10_0:
                d3dFeatureLevel = "Direct3d 10";
                break;
                break;
            case D3D_FEATURE_LEVEL_10_1:
                d3dFeatureLevel = "Direct3d 10.1";
                break;
            case D3D_FEATURE_LEVEL_11_0:
                d3dFeatureLevel = "Direct3d 11";
                break;
                break;
        };


        LOG ("Created D3D11 device with feature level " << d3dFeatureLevel);

        _level = _direct3d->GetFeatureLevel();
        if (!_creationParameters._sd.Windowed)
        {
            hr = _swapChain->SetFullscreenState( TRUE, nullptr );
        }
    }
    
    if (SUCCEEDED(hr))
    {
        _backbuffer = new BackbufferSurfaceD3D11 (this, _swapChain);
        _depthbuffer = new DepthSurfaceD3D11 (this);
        _depthbuffer->initialize(&Ibl::DepthSurfaceParameters(Ibl::PF_DEPTH24S8,
                                                               _backbuffer->width(),
                                                               _backbuffer->height()));

        _backbuffer->bind (0);
        _depthbuffer->bind();

        _deviceFrameBuffer = Ibl::FrameBuffer(_backbuffer, _depthbuffer);
        setupViewport (_deviceFrameBuffer);

        initializeDeviceStates();    
        _initialized = true;
    }
    else
    {
        _initialized = false;
    }

    // Setup managers and default post effects.
    postInitialize(deviceParameters);

    return _initialized;
}

const Ibl::FrameBuffer& 
DeviceD3D11::deviceFrameBuffer() const
{
    return _deviceFrameBuffer;
}

bool
DeviceD3D11::beginRender()
{
    float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //red,green,blue,alpha
    _immediateCtx->ClearRenderTargetView (_backbuffer->surface(), ClearColor );
    if (_depthbuffer)
    {
        _immediateCtx->ClearDepthStencilView (_depthbuffer->surface(), 
                                              D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 1);
    }

    return true;
}

bool
DeviceD3D11::present()
{
    Ibl::FrameBuffer frameBuffer(_backbuffer,
                                _depthbuffer);
    bindFrameBuffer (frameBuffer);

    HRESULT hr = hr = _swapChain->Present( 0, 0 );
    if (FAILED(hr))
    {
        switch(hr)
        {
            case DXGI_ERROR_DEVICE_RESET:
                LOG ("Device was reset\n");
                break;
            case DXGI_ERROR_DEVICE_REMOVED:
                LOG ("Device was removed\n");
                break;
            case DXGI_STATUS_OCCLUDED:
                LOG ("Device occluded\n");
                break;
            default:
                LOG ("Unknown d3d error \n");
        };
        throw(std::runtime_error("D3D11 crapped out"));
        return false;
    }
    else
    {
        return true;
    }
    return true;
}

inline D3D11_BLEND
D3D11AlphaFunction (const Ibl::AlphaFunction& alphaFunc)
{
    D3D11_BLEND state = D3D11_BLEND_ZERO;
    switch (alphaFunc)
    {
        case Ibl::BlendZero:
            state = D3D11_BLEND_ZERO;
            break;
        case Ibl::BlendOne:
            state = D3D11_BLEND_ONE;
            break;
        case Ibl::SourceColor:
            state = D3D11_BLEND_SRC_COLOR;
            break;
        case Ibl::InverseSourceColor:
            state = D3D11_BLEND_INV_SRC_COLOR;
            break;
        case Ibl::SourceAlpha:
            state = D3D11_BLEND_SRC_ALPHA;
            break;
        case Ibl::InverseSourceAlpha:
            state = D3D11_BLEND_INV_SRC_ALPHA;
            break;
        case Ibl::DestinationAlpha:    
            state = D3D11_BLEND_DEST_ALPHA;
            break;
        case Ibl::InverseDestinationAlpha:
            state = D3D11_BLEND_INV_DEST_ALPHA;
            break;
        case Ibl::DestinationColor:
            state = D3D11_BLEND_DEST_COLOR;
            break;
        case Ibl::InverseDestinationColor:
            state = D3D11_BLEND_INV_DEST_COLOR;
            break;
    }
    return state;
}


DeviceD3D11::DeviceD3D11() 
{    
    _window = nullptr;
    _factory = new DXGIFactory();

    _useMultiSampleAntiAliasing = true;
    _multiSampleCount = 1;
    _multiSampleQuality = 0;

    _cullMode = Ibl::CCW;

    _currentRasterState = nullptr;
    memset (&_currentRasterStateDesc, 0, sizeof (_currentRasterStateDesc));

    _currentDepthState = nullptr;
    memset (&_currentDepthStateDesc, 0, sizeof (_currentDepthStateDesc));  

    _currentBlendState = nullptr;
    memset (&_currentBlendStateDesc, 0, sizeof (_currentBlendStateDesc));  


    _adapter = nullptr;
    _direct3d = nullptr;
    _swapChain = nullptr;
    _immediateCtx = nullptr;
    _currentDepthSurface = nullptr;
    _initialized = false;
    _scissorEnabled = false;
    _currentSurfaceCount = 0;
    _currentUAVCount = 0;        
    _blendFactor = Ibl::Vector4f(1,1,1,1);

    for (uint32_t i = 0; i < MAX_RENDER_TARGETS; i++)
    {
        _currentSurfaces[i] = nullptr;
        _currentUnorderedSurfaces [i] = nullptr;
    }
}

void
DeviceD3D11::copyStructureCount(const Ibl::IGpuBuffer* dst, const Ibl::IGpuBuffer* src)
{
    const Ibl::BufferD3D11* bufferDst = 
        dynamic_cast<const Ibl::BufferD3D11*>(dst);
    const Ibl::BufferD3D11* bufferSrc = 
        dynamic_cast<const Ibl::BufferD3D11*>(src);

    if (bufferDst && bufferSrc)
    {
        _immediateCtx->CopyStructureCount(bufferDst->buffer(), 0, bufferSrc->unorderedView());
    }
}

bool
DeviceD3D11::supportsHardwareTessellationStage() const
{
    return _level == D3D_FEATURE_LEVEL_11_0;
}

void
DeviceD3D11::printState()
{
    ID3D11BlendState* currentBlendState = 0;
    // TODO:
    saferelease(currentBlendState);
}

void
DeviceD3D11::syncState()
{
    _immediateCtx->RSSetState(_currentRasterState);
    _immediateCtx->OMSetDepthStencilState (_currentDepthState, 0xff);
    _immediateCtx->OMSetBlendState(_currentBlendState, &_blendFactor.x, 0xffffffff);
}

ID3D11DepthStencilView*
DeviceD3D11::primaryDepthStencilView()
{
    return 0;
}

ID3D11RenderTargetView*
DeviceD3D11::primaryBackBufferView()
{
    return 0;
}

Window*    
DeviceD3D11::renderWindow()
{
    return _window;
}


void
DeviceD3D11::setViewport(const Viewport* viewport)
{
    _currentViewport.Width = viewport->_width;
    _currentViewport.Height = viewport->_height;
    _currentViewport.MinDepth = viewport->_minZ;
    _currentViewport.MaxDepth = viewport->_maxZ;
    _currentViewport.TopLeftX = viewport->_x;
    _currentViewport.TopLeftY = viewport->_y;

    _immediateCtx->RSSetViewports( 1, &_currentViewport );
}

void
DeviceD3D11::getViewport(Viewport* viewport) const
{
    

    *viewport = Ibl::Viewport (_currentViewport.TopLeftX, 
                                     _currentViewport.TopLeftY, 
                                     _currentViewport.Width, 
                                     _currentViewport.Height, 
                                     _currentViewport.MinDepth, 
                                     _currentViewport.MaxDepth);

}

bool
DeviceD3D11::setColorWriteState (bool r, bool g, bool b, bool a) 
{
      
    uint8_t red = (r ? D3D10_COLOR_WRITE_ENABLE_RED : 0);
    uint8_t green = (g ? D3D10_COLOR_WRITE_ENABLE_GREEN : 0);
    uint8_t blue = (b ? D3D10_COLOR_WRITE_ENABLE_BLUE : 0);
    uint8_t alpha = (a ? D3D10_COLOR_WRITE_ENABLE_ALPHA : 0);
    _currentBlendStateDesc.RenderTarget[0].RenderTargetWriteMask =  red | green | blue | alpha;

    bindBlendState();

    return true;
}

void*
DeviceD3D11::rawDevice()
{
    return _direct3d;
}

const Ibl::ISurface*    
DeviceD3D11::backbuffer() const
{
    return _backbuffer;
}

const Ibl::IDepthSurface*
DeviceD3D11::depthbuffer() const
{
    return _depthbuffer;
}

bool
DeviceD3D11::initialized() const
{
    return _initialized;
}



void
DeviceD3D11::initializeDeviceStates()
{
     // Initialize the rasterizer state defaults.
    _currentRasterStateDesc.FillMode = D3D11_FILL_SOLID;
    _currentRasterStateDesc.CullMode = D3D11_CULL_BACK;
    _currentRasterStateDesc.FrontCounterClockwise = true;
    _currentRasterStateDesc.DepthBias = false;
    _currentRasterStateDesc.DepthBiasClamp = 0.0f;
    _currentRasterStateDesc.SlopeScaledDepthBias = 0;
    _currentRasterStateDesc.DepthClipEnable = true;
    _currentRasterStateDesc.ScissorEnable = _scissorEnabled;
    _currentRasterStateDesc.MultisampleEnable = _useMultiSampleAntiAliasing;
    _currentRasterStateDesc.AntialiasedLineEnable = true;

    // Initialize the blend state defaults
    _currentBlendStateDesc.AlphaToCoverageEnable = false;
    _currentBlendStateDesc.IndependentBlendEnable = true;
    for (int i = 0; i < 8; i++)
    {
        _currentBlendStateDesc.RenderTarget[i].BlendEnable = false;
        _currentBlendStateDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
        _currentBlendStateDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
        _currentBlendStateDesc.RenderTarget[i].DestBlend = D3D11_BLEND_ONE;
        _currentBlendStateDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        _currentBlendStateDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        _currentBlendStateDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
        _currentBlendStateDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
    }

    _currentDepthStateDesc.DepthEnable = true;
    _currentDepthStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
    _currentDepthStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    _currentDepthStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    _currentDepthStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    _currentDepthStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    _currentDepthStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    _currentDepthStateDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    _currentDepthStateDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    _currentDepthStateDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    _currentDepthStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    _currentDepthStateDesc.StencilEnable = false;
    _currentDepthStateDesc.StencilReadMask = 0xFF;
    _currentDepthStateDesc.StencilWriteMask = 0xFF;

    bindBlendState();
    bindDepthState();
    bindRasterState();

    setupBlendPipeline(Ibl::BlendOver);
}

void
DeviceD3D11::setupStencil(uint8_t readMask, 
                          uint8_t writeMask,
                          Ibl::CompareFunction frontCompare,
                          Ibl::StencilOp frontStencilFailOp, 
                          Ibl::StencilOp frontStencilPassOp,
                          Ibl::StencilOp frontZFailOp)
{
    _currentDepthStateDesc.FrontFace.StencilFailOp = (D3D11_STENCIL_OP)(frontStencilFailOp);
    _currentDepthStateDesc.FrontFace.StencilPassOp = (D3D11_STENCIL_OP)(frontStencilPassOp);
    _currentDepthStateDesc.FrontFace.StencilDepthFailOp = (D3D11_STENCIL_OP)(frontZFailOp);
    _currentDepthStateDesc.FrontFace.StencilFunc = (D3D11_COMPARISON_FUNC)(frontCompare);

    _currentDepthStateDesc.BackFace.StencilFailOp = (D3D11_STENCIL_OP)(frontStencilPassOp);
    _currentDepthStateDesc.BackFace.StencilPassOp = (D3D11_STENCIL_OP)(frontStencilFailOp);
    _currentDepthStateDesc.BackFace.StencilDepthFailOp = (D3D11_STENCIL_OP)(frontZFailOp);
    _currentDepthStateDesc.BackFace.StencilFunc = (D3D11_COMPARISON_FUNC)(frontCompare);

    _currentDepthStateDesc.StencilReadMask = readMask;
    _currentDepthStateDesc.StencilWriteMask = writeMask;

    bindDepthState();
}

void
DeviceD3D11::setupStencil(uint8_t readMask,
                          uint8_t writeMask,
                          Ibl::CompareFunction frontCompare,
                          Ibl::StencilOp frontStencilFailOp,
                          Ibl::StencilOp frontStencilPassOp,
                          Ibl::StencilOp frontZFailOp,
                          Ibl::CompareFunction backCompare,
                          Ibl::StencilOp backStencilFailOp,
                          Ibl::StencilOp backStencilPassOp,
                          Ibl::StencilOp backZFailOp)
{
    _currentDepthStateDesc.FrontFace.StencilFailOp = (D3D11_STENCIL_OP)(frontStencilFailOp);
    _currentDepthStateDesc.FrontFace.StencilPassOp = (D3D11_STENCIL_OP)(frontStencilPassOp);
    _currentDepthStateDesc.FrontFace.StencilDepthFailOp = (D3D11_STENCIL_OP)(frontZFailOp);
    _currentDepthStateDesc.FrontFace.StencilFunc = (D3D11_COMPARISON_FUNC)(frontCompare);

    _currentDepthStateDesc.BackFace.StencilFailOp = (D3D11_STENCIL_OP)(backStencilPassOp);
    _currentDepthStateDesc.BackFace.StencilPassOp = (D3D11_STENCIL_OP)(backStencilFailOp);
    _currentDepthStateDesc.BackFace.StencilDepthFailOp = (D3D11_STENCIL_OP)(backZFailOp);
    _currentDepthStateDesc.BackFace.StencilFunc = (D3D11_COMPARISON_FUNC)(backCompare);

    _currentDepthStateDesc.StencilReadMask = readMask;
    _currentDepthStateDesc.StencilWriteMask = writeMask;

    bindDepthState();
}

void
DeviceD3D11::bindBlendState()
{
    // Todo, this state should be hashed and stored
   saferelease(_currentBlendState);
   _direct3d->CreateBlendState ( &_currentBlendStateDesc, &_currentBlendState);
      
   _immediateCtx->OMSetBlendState(_currentBlendState, &_blendFactor.x, 0xffffffff);
}

void
DeviceD3D11::bindDepthState()
{
    // Todo, this state should be hashed and stored
    saferelease(_currentDepthState);
    _direct3d->CreateDepthStencilState (&_currentDepthStateDesc, &_currentDepthState);
    _immediateCtx->OMSetDepthStencilState (_currentDepthState, 0xff);
}

void
DeviceD3D11::bindRasterState()
{
    // Todo, this stuff should be hashed and stored.
    saferelease(_currentRasterState);    
    _direct3d->CreateRasterizerState( &_currentRasterStateDesc, &_currentRasterState);
    _immediateCtx->RSSetState(_currentRasterState);

}

void
DeviceD3D11::bindSurface (int level, 
                             const Ibl::ISurface* surface) 
{
    _currentSurfaces [level] = surface;
    bindSurfaceAndTargets();
}


void
DeviceD3D11::setupViewport (const Ibl::FrameBuffer& frameBuffer)
{
    if (frameBuffer.colorSurface(0))
    {
        _currentViewport.Width = (float)(frameBuffer.colorSurface(0)->width());
        _currentViewport.Height = (float)(frameBuffer.colorSurface(0)->height());
        _currentViewport.MinDepth = 0.0f;
        _currentViewport.MaxDepth = 1.0f;
        _currentViewport.TopLeftX = 0.0f;
        _currentViewport.TopLeftY = 0.0f;

        _immediateCtx->RSSetViewports( 1, &_currentViewport );
    }
    else if (frameBuffer.depthSurface())
    {
        _currentViewport.Width = float(frameBuffer.depthSurface()->width());
        _currentViewport.Height = float(frameBuffer.depthSurface()->height());
        _currentViewport.MinDepth = 0.0f;
        _currentViewport.MaxDepth = 1.0f;
        _currentViewport.TopLeftX = 0.0f;
        _currentViewport.TopLeftY = 0.0f;

        _immediateCtx->RSSetViewports( 1, &_currentViewport );
    }
    else
    {
        LOG ("Failure, no surface to determine size from ");
    }
}

bool
DeviceD3D11::bindFrameBuffer (const Ibl::FrameBuffer& framebuffer)
{
    bindNullFrameBuffer();

    // Performing this bind will update the current frame buffer.
    for (int i = 0;i < 4; i++)
    {
        if (const Ibl::ISurface* colorSurface = 
            framebuffer.colorSurface (i))
        {
            _currentSurfaces[i] = colorSurface;
        }
    }

    for (int i = 0;i < 4; i++)
    {
        if (const Ibl::IRenderResource* unorderedSurface = 
            dynamic_cast<const Ibl::IRenderResource*>(framebuffer.unorderedSurface(i)))
        {
            _currentUnorderedSurfaces[i] = unorderedSurface;
        }
    }

    if (const Ibl::IDepthSurface * depthSurface =
        framebuffer.depthSurface())
    {
        _currentDepthSurface = depthSurface;
    }

    bindSurfaceAndTargets();
    return true;
}

void
DeviceD3D11::bindNullFrameBuffer()
{
    _currentSurfaces[0] = nullptr;
    _currentSurfaces[1] = nullptr;
    _currentSurfaces[2] = nullptr;
    _currentSurfaces[3] = nullptr; 

    _currentUnorderedSurfaces[0] = nullptr;
    _currentUnorderedSurfaces[1] = nullptr;
    _currentUnorderedSurfaces[2] = nullptr;
    _currentUnorderedSurfaces[3] = nullptr; 

    _currentDepthSurface = nullptr;

    bindSurfaceAndTargets();
}

void
DeviceD3D11::bindDepthSurface (const Ibl::IDepthSurface* surface)
{
    _currentDepthSurface = surface;
    bindSurfaceAndTargets();
}

void
DeviceD3D11::resetShaderPipeline()
{
/*
    _immediateCtx->VSSetShader(nullptr, nullptr, 0);
    _immediateCtx->HSSetShader(nullptr, nullptr, 0);
    _immediateCtx->GSSetShader(nullptr, nullptr, 0);
    _immediateCtx->DSSetShader(nullptr, nullptr, 0);
    _immediateCtx->PSSetShader(nullptr, nullptr, 0);
    */
}

void
DeviceD3D11::clearShaderResources() const
{
/*
    static ID3D11ShaderResourceView * const srViews[16] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, 
                                                           nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    _immediateCtx->VSSetShaderResources(0, 16, srViews);
    _immediateCtx->DSSetShaderResources(0, 16, srViews);
    _immediateCtx->HSSetShaderResources(0, 16, srViews);
    _immediateCtx->GSSetShaderResources(0, 16, srViews);
    _immediateCtx->PSSetShaderResources(0, 16, srViews);
    */
}


Ibl::BlendPipelineType 
DeviceD3D11::blendPipeline() const
{
    return _blendPipelineType;
}

void
DeviceD3D11::setupBlendPipeline(Ibl::BlendPipelineType blendPipelineType) 
{
    //if (_blendPipelineType == blendPipelineType)
    //    return;

    if (blendPipelineType == Ibl::BlendOver)
    {
        setSrcFunction (Ibl::SourceAlpha);
        setDestFunction (Ibl::BlendOne);
        setBlendProperty (Ibl::OpAdd);
        setAlphaSrcFunction (Ibl::BlendOne);
        setAlphaDestFunction (Ibl::BlendOne);
        setAlphaBlendProperty(Ibl::OpAdd);

        _blendPipelineType = blendPipelineType;
    }
    else if (blendPipelineType == Ibl::BlendAlpha)
    {
        setSrcFunction (Ibl::SourceAlpha);
        setDestFunction (Ibl::InverseSourceAlpha);
        setBlendProperty (Ibl::OpAdd);
        setAlphaSrcFunction (Ibl::BlendOne);
        setAlphaDestFunction (Ibl::InverseSourceAlpha);
        setAlphaBlendProperty(Ibl::OpAdd);

        _blendPipelineType = blendPipelineType;
    }
    else if (blendPipelineType == Ibl::BlendModulate)
    {
        setSrcFunction (Ibl::SourceAlpha);
        setDestFunction (Ibl::InverseSourceAlpha);
        setBlendProperty (Ibl::OpAdd);
        setAlphaSrcFunction (Ibl::BlendOne);
        setAlphaDestFunction (Ibl::BlendOne);
        setAlphaBlendProperty(Ibl::OpAdd);

/*
BlendEnable = TRUE;
BlendOp = D3D11_BLEND_OP_ADD;
BlendOpAlpha = D3D11_BLEND_OP_ADD;
SrcBlend = D3D11_BLEND_SRC_ALPHA;
SrcBlendAlpha = D3D11_BLEND_ONE;
DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
DestBlendAlpha = D3D11_BLEND_ONE;
*/

        _blendPipelineType = blendPipelineType;
    }
    else if (blendPipelineType == Ibl::BlendPipelineOne)
    {
        setSrcFunction (Ibl::BlendOne);
        setDestFunction (Ibl::BlendOne);
        setBlendProperty (Ibl::OpAdd);
        setAlphaSrcFunction (Ibl::SourceAlpha);
        setAlphaDestFunction (Ibl::InverseSourceAlpha);
        setAlphaBlendProperty(Ibl::OpAdd);

        _blendPipelineType = blendPipelineType;
    }
    else
    {
        LOG ("Unknown blend pipeline type " << blendPipelineType << __LINE__ << " " << __FILE__);
    }
}

bool
DeviceD3D11::scissorEnabled() const
{
    return _scissorEnabled;
}

void
DeviceD3D11::setScissorEnabled(bool scissorEnabled)
{
    _scissorEnabled = scissorEnabled;
    _currentRasterStateDesc.ScissorEnable = _scissorEnabled;
    bindRasterState();
}

void
DeviceD3D11::setScissorRect(int x, int y, int width, int height)
{
    D3D11_RECT rc;
    rc.left = x;
    rc.top = y;
    rc.right = x + width;
    rc.bottom = y + height;
    _immediateCtx->RSSetScissorRects(1, &rc);
}

void
DeviceD3D11::bindSurfaceAndTargets()
{
    ID3D11RenderTargetView* views[MAX_RENDER_TARGETS];   
    memset (&views[0], 0, sizeof (ID3D11RenderTargetView*) * MAX_RENDER_TARGETS);

    ID3D11UnorderedAccessView * unorderedViews[MAX_RENDER_TARGETS];
    memset (&unorderedViews[0], 0, sizeof (ID3D11UnorderedAccessView*) * MAX_RENDER_TARGETS);

    // Bind and force to nullptr.
    // If we are changing targets we should also unbind all shader resources.
    ID3D11ShaderResourceView * const pSRV[16] = {nullptr};
    _immediateCtx->PSSetShaderResources(0, 16, pSRV);

    UINT initialCounts[D3D11_PS_CS_UAV_REGISTER_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    //_immediateCtx->OMSetRenderTargets (MAX_RENDER_TARGETS, &views[0], nullptr); 
    _immediateCtx->OMSetRenderTargetsAndUnorderedAccessViews(MAX_RENDER_TARGETS,
                                                             &views[0], 
                                                             nullptr, 
                                                             MAX_RENDER_TARGETS,/* start uav slot*/ 
                                                             MAX_RENDER_TARGETS, 
                                                             &unorderedViews[0], 
                                                             initialCounts);

    _currentSurfaceCount = 0;
    uint32_t colorWidth = 0;
    uint32_t colorHeight = 0;

    for (int i = 0; i < MAX_RENDER_TARGETS; i++)
    {
        if (_currentSurfaces [i] == nullptr)
        {
            break;
        }
        else
        {
            if (const Ibl::SurfaceD3D11 * surface = 
                dynamic_cast<const Ibl::SurfaceD3D11*>(_currentSurfaces [i]))
            {
                views [i] = surface->surface();
                _currentSurfaceCount++;

                colorWidth = ((Ibl::SurfaceD3D11*)_currentSurfaces [i])->width();
                colorHeight = ((Ibl::SurfaceD3D11*)_currentSurfaces [i])->height();
            }
        }
    }

    _currentUAVCount = 0;
    for (int i = 0; i < MAX_RENDER_TARGETS; i++)
    {
        if (_currentUnorderedSurfaces [i] == nullptr)
        {
            break;
        }
        else
        {
            if (const Ibl::BufferD3D11* resource = 
                dynamic_cast <const Ibl::BufferD3D11*>
                (_currentUnorderedSurfaces[i]))
            {
                unorderedViews [i] = resource->unorderedView();
                _currentUAVCount++;
            }
            else if (const Ibl::VertexBufferD3D11* vertexBufferResource =
                     dynamic_cast<const Ibl::VertexBufferD3D11*>(_currentUnorderedSurfaces[i]))
            {
                unorderedViews [i] = vertexBufferResource->unorderedResourceView();
                _currentUAVCount++;
            }
        }
    }

    uint32_t depthWidth = 0;
    uint32_t depthHeight = 0;

    ID3D11DepthStencilView* depthSurfaceView = 0;
    if (_currentDepthSurface)
    {
        depthSurfaceView = ((Ibl::DepthSurfaceD3D11*)_currentDepthSurface)->surface();
        depthWidth = ((Ibl::DepthSurfaceD3D11*)_currentDepthSurface)->width();
        depthHeight = ((Ibl::DepthSurfaceD3D11*)_currentDepthSurface)->height();
    }

    //if (_currentUAVCount > 0)
    {
        
        /* initIndices needs to be uavs.size() */
        /* assert that (_currentSurfaceCount + _currentUavCount) < 8 */
        /* TODO: this really needs more error checking */

        /*
        if (_currentSurfaceCount == 0)
        {
            ID3D11RenderTargetView* rtViews[16] = { nullptr };
            context->OMSetRenderTargets(static_cast<UINT>(16), rtViews, nullptr);
        }*/

        _immediateCtx->OMSetRenderTargetsAndUnorderedAccessViews(_currentSurfaceCount,
                                                                 &views[0], 
                                                                 depthSurfaceView, 
                                                                 _currentSurfaceCount,/* start uav slot*/ 
                                                                 MAX_RENDER_TARGETS, 
                                                                 &unorderedViews[0], 
                                                                 initialCounts);
    }
}

bool
DeviceD3D11::resizeDevice (const Ibl::Vector2i& newSize)
{
    bool result = false;

    if (_initialized)
    {        
        if (((newSize.x != _creationParameters._sd.BufferDesc.Width) ||
            (newSize.y != _creationParameters._sd.BufferDesc.Height)))
        {
            // ResizeBuffers
            LOG ("About to resize swap chain");

            // Kill shared textures.
            for (auto it = _temporaryTexturePool.begin(); it != _temporaryTexturePool.end(); it++)
            {
                delete *it;
            }
            _temporaryTexturePool.clear();

            // Clear state and flush.
            _immediateCtx->ClearState();
            _immediateCtx->Flush();

            IRenderResource::beginResize();

            // request release for anything this render target size dependent.
            // Alternate between 0 and DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH when resizing buffers.
            // When in windowed mode, we want 0 since this allows the app to change to the desktop
            // resolution from windowed mode during alt+enter.  However, in fullscreen mode, we want
            // the ability to change display modes from the Device Settings dialog.  Therefore, we
            // want to set the DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH flag.
            uint32_t flags = 0;
            if(!_creationParameters._sd.Windowed)
            {
                flags  = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            }

            _creationParameters._sd.BufferDesc.Width = newSize.x;
            _creationParameters._sd.BufferDesc.Height = newSize.y;

            LOG ("Resizing buffers to " << _creationParameters._sd.BufferDesc.Width << " " << _creationParameters._sd.BufferDesc.Height );

            HRESULT resizeResult = 
                _swapChain->ResizeBuffers (_creationParameters._sd.BufferCount,
                                           _creationParameters._sd.BufferDesc.Width,
                                           _creationParameters._sd.BufferDesc.Height,
                                           _creationParameters._sd.BufferDesc.Format, flags);


            if (resizeResult == S_OK)
            {
                LOG ("Successfully Resized swap chain");

                _depthbuffer->setSize(newSize);

                // Recreate released size dependent items.
                result = IRenderResource::endResize();

                setupViewport (_deviceFrameBuffer);
                bindFrameBuffer(_deviceFrameBuffer);

                // Reset surfaces
                for (uint32_t surfaceId = 0; surfaceId < MAX_RENDER_TARGETS; surfaceId++)
                {
                    _currentSurfaces [surfaceId] = nullptr;
                }

                // We really need to emit something here to resize widgets etc...
            }
            else
            {
                logDXGIError (resizeResult);
                LOG ("Failed to resize swap chain... \n");
                exit(0);
            }
        }
    }

    return true;
}

bool 
DeviceD3D11::reset()
{
    return true;
}

bool
DeviceD3D11::blitSurfaces (const ISurface* destination, 
                           const ISurface* source, 
                           TextureFilter filterType,
                           size_t arrayOffset) const
{
    const SurfaceD3D11* src = (const Ibl::SurfaceD3D11*) (source);
    const SurfaceD3D11* dest = (const Ibl::SurfaceD3D11*) (destination);

    if (src->texture()->multiSampleCount() > 1 &&
        destination->width() == source->width() &&
        destination->height() == source->height())
    {
        _immediateCtx->ResolveSubresource (dest->resource(), 0, src->resource(), 0, src->dxFormat());
    }
    // Else we can just copy the resource over
    else if (destination->width() != source->width() ||
             destination->height() != source->height() )
    {
        
        if (source->texture()->resource()->textureCount() > 1)
        {
            return _colorResolveEffect->render(dest, src->texture(), nullptr, arrayOffset);
        }
        else
        {
            return _colorResolveEffect->render(dest, src->texture(), nullptr);
        }
    }
    else
    {
        _immediateCtx->CopyResource (dest->resource(), src->resource());
        return true;
    }

    return false;
}

bool
DeviceD3D11::blitSurfaces (const IDepthSurface* destination, 
                              const IDepthSurface* source, 
                              TextureFilter filterType) const
{
    const DepthSurfaceD3D11* src = (const Ibl::DepthSurfaceD3D11*) (source);
    const DepthSurfaceD3D11* dest = (const Ibl::DepthSurfaceD3D11*) (destination);
    

    return _depthResolveEffect->render(dest, src, nullptr);
}

bool
DeviceD3D11::clearSurfaces (uint32_t indexToClearTo, 
                               unsigned long clearType, 
                               float redClear, 
                               float greenClear, 
                               float blueClear, 
                               float alphaClear) const
{

   float clearColor[4] = {redClear, greenClear, blueClear, alphaClear}; 

   for (uint32_t i = 0; i < indexToClearTo+1; i++)
   {
       if (const Ibl::SurfaceD3D11* surface = 
           dynamic_cast <const Ibl::SurfaceD3D11*>(_currentSurfaces[i]))
       {
            _immediateCtx->ClearRenderTargetView (surface->surface(), clearColor );
       }
   }

    if (_currentDepthSurface && (clearType & CLEAR_ZBUFFER))
    {
        _immediateCtx->ClearDepthStencilView (((Ibl::DepthSurfaceD3D11*)_currentDepthSurface)->surface(), 
                                          D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
    }

   return false;
}

bool
DeviceD3D11::drawPrimitive (const IVertexDeclaration* vertexDeclaration, 
                               const IVertexBuffer* vertexBuffer, 
                               const GpuTechnique* technique,
                               PrimitiveType primitiveType, 
                               uint32_t primitiveCount,
                               uint32_t vertexOffset) const
{
    _immediateCtx->IASetIndexBuffer (0, DXGI_FORMAT_R32_UINT,0);
    if (vertexBuffer->bind())
    {
        D3D11_PRIMITIVE_TOPOLOGY topology = (D3D11_PRIMITIVE_TOPOLOGY)(primitiveType);

        if (technique->hasTessellationStage())
        {
            // Modify the primitive type.
            switch (topology)
            {
                // This needs to be modified to look at the index buffers
                // points per primitive flag (which doesn't yet exist).
                case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
                    topology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
                    break;    
                default:
                    return false;
            }
        }
        _immediateCtx->IASetPrimitiveTopology (topology);

        uint32_t vertexCount = 0;
        switch (topology)
        {
            case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
                vertexCount = primitiveCount;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
                vertexCount = primitiveCount * 2;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
                vertexCount = primitiveCount + 1;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
                vertexCount = primitiveCount * 3;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
                vertexCount = primitiveCount + 2;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
                vertexCount = primitiveCount * 3;
                break;
            default: 
                break;
        }
        _immediateCtx->Draw (vertexCount, vertexOffset);
    }
    return true;
}

void
DeviceD3D11::resetViewsAndShaders() const
{
/*
    ID3D11ShaderResourceView* srViews[16] = { nullptr };
    _immediateCtx->PSSetShaderResources(0, static_cast<UINT>(16), srViews);

    ID3D11RenderTargetView* rtViews[16] = { nullptr };
    _immediateCtx->OMSetRenderTargets(static_cast<UINT>(MAX_RENDER_TARGETS), rtViews, nullptr);
*/
}

bool
DeviceD3D11::drawIndexedPrimitive (const IVertexDeclaration* vertexDeclaration, 
                                      const IIndexBuffer* indexBuffer, 
                                      const IVertexBuffer* vertexBuffer, 
                                      const GpuTechnique* technique,
                                      PrimitiveType primitiveType, 
                                      uint32_t faceCount,
                                      uint32_t indexOffset,
                                      uint32_t vertexOffset) const
{
    bool result = false;
    if (vertexBuffer->bind() && indexBuffer->bind())
    {        
        D3D11_PRIMITIVE_TOPOLOGY topology = (D3D11_PRIMITIVE_TOPOLOGY)(primitiveType);
        if (technique->hasTessellationStage())
        {
            // Modify the primitive type.
            switch (topology)
            {
                // This needs to be modified to look at the index buffers
                // points per primitive flag (which doesn't yet exist).
                case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
                    topology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
                    break;    
            }
        }

        _immediateCtx->IASetPrimitiveTopology (topology);
        //LOG ("Set topology " << (uint32_t)(topology));
        
        uint32_t indexCount = 0;
        switch (topology)
        {
            case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
                indexCount = faceCount;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
                indexCount = faceCount * 2;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
                indexCount = faceCount + 1;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
                indexCount = faceCount * 3;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
                indexCount = faceCount + 2;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:      
                indexCount = faceCount * 3;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 4;
                break;
           case D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 12;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 16;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 17;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 18;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 19;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 20;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 21;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 22;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 23;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 24;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 25;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 26;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 27;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 28;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 29;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 30;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 31;
                break;
            case D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
                indexCount = faceCount * 32;
                break;
            default: 
                break;
        }        
        _immediateCtx->DrawIndexed(indexCount, indexOffset, vertexOffset);
    }
    return result;
}

bool
DeviceD3D11::setNullTarget (uint32_t index)
{
    _currentSurfaces [index] = nullptr;
    bindSurfaceAndTargets();
    return true;
}

void
DeviceD3D11::setNullStreamOut()
{
    // Set stream out to null
    ID3D11Buffer *buffer[1];
    buffer [0] = nullptr;
    UINT offset[1] = { 0 };
    _immediateCtx->SOSetTargets (1, buffer, offset);
}

bool
DeviceD3D11::writeFrontBufferToFile (const std::string& filename) const
{
    return _backbuffer->writeToFile (filename);
}

void
DeviceD3D11::enableAlphaBlending()
{
/*
    typedef struct D3D11_RENDER_TARGET_BLEND_DESC
    {
    BOOL BlendEnable;
    D3D11_BLEND SrcBlend;
    D3D11_BLEND DestBlend;
    D3D11_BLEND_OP BlendProperty;
    D3D11_BLEND SrcBlendAlpha;
    D3D11_BLEND DestBlendAlpha;
    D3D11_BLEND_OP BlendOpAlpha;
    UINT8 RenderTargetWriteMask;
    }
*/
    _currentBlendStateDesc.RenderTarget[0].BlendEnable = true;
    bindBlendState();

}

void
DeviceD3D11::disableAlphaBlending()
{
    _currentBlendStateDesc.RenderTarget[0].BlendEnable = false;
    bindBlendState();
}

void
DeviceD3D11::setAlphaToCoverageEnable (bool value)
{
    _currentBlendStateDesc.AlphaToCoverageEnable = value;
    bindBlendState();
}
 
void
DeviceD3D11::setBlendProperty (const Ibl::BlendOp& op)
{
    _currentBlendStateDesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)op;    
    bindBlendState();
}

void
DeviceD3D11::setSrcFunction (const Ibl::AlphaFunction& srcFunction)
{
    _currentBlendStateDesc.RenderTarget[0].SrcBlend = D3D11AlphaFunction (srcFunction);    
    bindBlendState();
}

void
DeviceD3D11::setDestFunction (const Ibl::AlphaFunction& alphaFunc)
{

    _currentBlendStateDesc.RenderTarget[0].DestBlend = D3D11AlphaFunction (alphaFunc);    
    bindBlendState();

}
 
void
DeviceD3D11::setAlphaBlendProperty (const Ibl::BlendOp& op)
{
    _currentBlendStateDesc.RenderTarget[0].BlendOpAlpha = (D3D11_BLEND_OP)op;
    bindBlendState();

}

void
DeviceD3D11::setAlphaDestFunction (const Ibl::AlphaFunction& alphaFunc)
{   
    _currentBlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11AlphaFunction (alphaFunc);    
    bindBlendState();
}

void
DeviceD3D11::setAlphaSrcFunction (const Ibl::AlphaFunction& alphaFunc)
{
    _currentBlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11AlphaFunction (alphaFunc);    
    bindBlendState(); 
}

void
DeviceD3D11::setDrawMode (Ibl::DrawMode drawMode)
{
    switch (drawMode)
    {
        case Ibl::Point: // No point mode in D3D11 :(.
        case Ibl::Wireframe:
            _currentRasterStateDesc.FillMode = D3D11_FILL_WIREFRAME;
            break;
        case Ibl::Filled:
            _currentRasterStateDesc.FillMode = D3D11_FILL_SOLID;
            break;

        default:
            break;
    }
    bindRasterState();
}

Ibl::DrawMode 
DeviceD3D11::getDrawMode () const
{
    if (_currentRasterStateDesc.FillMode == D3D11_FILL_WIREFRAME)
    {
        return Ibl::Wireframe;
    }

    return Ibl::Filled;
}

void
DeviceD3D11::fogEnable()
{
    LOG ("NOT IMPLEMENTTED");
}

void
DeviceD3D11::fogDisable()
{
    // This does not exist
}

void
DeviceD3D11::disableDepthWrite()
{
    _currentDepthStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    bindDepthState();
}

void
DeviceD3D11::enableDepthWrite()
{
    _currentDepthStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    bindDepthState();
}

void
DeviceD3D11::disableZTest()
{
    _currentDepthStateDesc.DepthEnable = false;
    bindDepthState();
}

void
DeviceD3D11::enableZTest()
{
    _currentDepthStateDesc.DepthEnable = true;
    bindDepthState();
}

void
DeviceD3D11::setZFunction (Ibl::CompareFunction compareFunc)
{
    switch (compareFunc)
    {
        case Ibl::Less:
            _currentDepthStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
            break;
        case Ibl::LessEqual:
            _currentDepthStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
            break;
        case Ibl::Greater:
            _currentDepthStateDesc.DepthFunc = D3D11_COMPARISON_GREATER;
            break;
        case Ibl::GreaterEqual:
            _currentDepthStateDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
            break;
        case Ibl::Never:
            _currentDepthStateDesc.DepthFunc = D3D11_COMPARISON_NEVER;
            break;
        case Ibl::Equal:
            _currentDepthStateDesc.DepthFunc = D3D11_COMPARISON_EQUAL;
            break;
        case Ibl::NotEqual:
            _currentDepthStateDesc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;
            break;
    }

    bindDepthState();
}

void
DeviceD3D11::setFrontFaceStencilFunction(Ibl::CompareFunction compareFunc)
{
    _currentDepthStateDesc.FrontFace.StencilFunc = (D3D11_COMPARISON_FUNC)(compareFunc);
    bindDepthState();
}

void
DeviceD3D11::setFrontFaceStencilPass(Ibl::StencilOp op)
{
    _currentDepthStateDesc.FrontFace.StencilPassOp = (D3D11_STENCIL_OP)(op);
    bindDepthState();
}

void
DeviceD3D11::enableStencilTest()
{
    _currentDepthStateDesc.StencilEnable = true;
    bindDepthState();
}

void
DeviceD3D11::disableStencilTest()
{
    _currentDepthStateDesc.StencilEnable = false;
    bindDepthState();
}

// Todo: FrontFaceStencilPass = REPLACE etc...
Ibl::CullMode
DeviceD3D11::cullMode() const
{
    return _cullMode;
}

void
DeviceD3D11::setCullMode (Ibl::CullMode cullMode)
{
    _cullMode = cullMode;
    switch (_cullMode)
    {
        case Ibl::CCW:
            _currentRasterStateDesc.CullMode = D3D11_CULL_FRONT;
            break;
        case Ibl::CW:
            _currentRasterStateDesc.CullMode = D3D11_CULL_BACK;
            break;
        case Ibl::CullNone:
            _currentRasterStateDesc.CullMode = D3D11_CULL_NONE;
            break;
        default:
            break;;
    }
   bindRasterState();
}

void
DeviceD3D11::setNullPixelShader()
{
    // N/A (obsolete)
}

void
DeviceD3D11::setNullVertexShader()
{
    // N/A (obsolete)
}

bool
DeviceD3D11::isRenderTextureFormatSupported (const Ibl::PixelFormat& format)
{
    // All formats are now supported
    return true;
}

Ibl::IGpuBuffer *   
DeviceD3D11::createBufferResource (const Ibl::RenderResourceParameters* data)
{
    if (DeviceD3D11* device = dynamic_cast <DeviceD3D11*>(this))
    {
        if (const Ibl::GpuBufferParameters* resource = 
            dynamic_cast<const Ibl::GpuBufferParameters*>(data))
        {
            Ibl::BufferD3D11* bufferResource = new Ibl::BufferD3D11(device);
            if (bufferResource->initialize (resource))
            {
                return bufferResource;
            }
        }
    }
    return nullptr;
}

Ibl::IVertexBuffer *     
DeviceD3D11::createVertexBuffer (const Ibl::RenderResourceParameters* data)
{
    if (DeviceD3D11* device = dynamic_cast <DeviceD3D11*>(this))
    {
        if (const Ibl::VertexBufferParameters* resource = 
            dynamic_cast<const Ibl::VertexBufferParameters*>(data))
        {
            Ibl::VertexBufferD3D11* vertexBuffer = new VertexBufferD3D11 (device);
            if (vertexBuffer->initialize (resource))
            {
                return vertexBuffer;
            }
        }
    }
    return nullptr;
}

Ibl::IIndexBuffer *      
DeviceD3D11::createIndexBuffer (const Ibl::RenderResourceParameters* data)
{
    if (DeviceD3D11* device = dynamic_cast <DeviceD3D11*>(this))
    {
        if (const Ibl::IndexBufferParameters* resource = 
            dynamic_cast<const Ibl::IndexBufferParameters*>(data))
        {
            IndexBufferD3D11* indexBuffer = new IndexBufferD3D11 (device);
            if (indexBuffer->initialize (resource))
            {
                return indexBuffer;
            }
        }
    }
    return nullptr;
}

Ibl::IVertexDeclaration * 
DeviceD3D11::createVertexDeclaration (const Ibl::RenderResourceParameters* data)
{
    if (DeviceD3D11* device = dynamic_cast <DeviceD3D11*>(this))
    {
        if (const Ibl::VertexDeclarationParameters* resource =
            dynamic_cast <const Ibl::VertexDeclarationParameters*>(data))
        {
            VertexDeclarationD3D11* declaration = new VertexDeclarationD3D11 (device);
            if (declaration->initialize (resource))
            {
                return declaration;
            }
        }
    }
    return nullptr;
}

Ibl::IDepthSurface *     
DeviceD3D11::createDepthSurface(const Ibl::RenderResourceParameters* data)
{
    if (DeviceD3D11* device = dynamic_cast <DeviceD3D11*>(this))
    {
        Ibl::DepthSurfaceD3D11* depthSurface = new Ibl::DepthSurfaceD3D11 (device);
        if (const Ibl::DepthSurfaceParameters* resource =
            dynamic_cast<const Ibl::DepthSurfaceParameters*> (data))
        {
            if (depthSurface->initialize (resource))
            {
                return depthSurface;
            }
        }
    }
    return nullptr;
}

Ibl::ITexture *          
DeviceD3D11::createTexture (const Ibl::RenderResourceParameters* data)
{
    if (DeviceD3D11* device = dynamic_cast <DeviceD3D11*>(this))
    {
        Ibl::ITexture* texture = 0;
        if (const Ibl::TextureParameters* textureData = 
            dynamic_cast<const Ibl::TextureParameters*>(data))
        {
            if (textureData->type() == Ibl::FromFile ||
                textureData->type() == Ibl::StagingFromFile ||
                textureData->type() == Ibl::Procedural)
            {
                if (textureData->dimension() == Ibl::TwoD)
                {
                    texture = new Texture2DD3D11(device);
                }
                else if (textureData->dimension() == Ibl::CubeMap)
                {
                    texture = new Texture2DD3D11(device);
                }
                else
                {
                    LOG ("Unsuppored texture initialization type");
                    return 0;
                }
            }
            else if (textureData->type() == Ibl::RenderTarget)
            {
                texture = new Ibl::RenderTargetTextureD3D11 (device);
            }

            if (texture)
            {
                if (texture->initialize (textureData))
                {
                    return texture;
                }
                else
                {
                    safedelete (texture);
                }
            }
        }
    }
    return nullptr;
}

Ibl::IShader *           
DeviceD3D11::createShader (const Ibl::RenderResourceParameters* data)
{
    if (DeviceD3D11* device = dynamic_cast <DeviceD3D11*>(this))
    {
        return new Ibl::ShaderD3D11(device);
    }
    return nullptr;
}

Ibl::IComputeShader *
DeviceD3D11::createComputeShader (const Ibl::RenderResourceParameters* data)
{
    if (DeviceD3D11* device = dynamic_cast <DeviceD3D11*>(this))
    {
        return new Ibl::ComputeShaderD3D11(device);
    }
    return nullptr;
}

void
DeviceD3D11::destroyResource(Ibl::IRenderResource* resource)
{
    delete resource;
}


}