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

#ifndef INCLUDED_DIRECT3D11_DEVICE
#define INCLUDED_DIRECT3D11_DEVICE

#include <IblRenderWindowD3D11.h>
#include <IblIDevice.h>
#include <d3d11.h>

namespace Ibl
{
class Application;
class DXGIFactory;
class DepthSurfaceD3D11;
class SurfaceD3D11;
class RenderWindow;
class ISurface;
class IDepthSurface;
class DepthResolve;
class ColorResolve;

#define MAX_RENDER_TARGETS 8

class DXGIFactory
{
  public:
    DXGIFactory();
    ~DXGIFactory();

    bool                       D3D11Available() const;
    IDXGIFactory1*             factory();

  protected:
    void                       initialize();
    void                       setD3D11Available (bool available);
    void                       setFactory (IDXGIFactory1* factory);

  private:
    bool                       _D3D11Available;
    IDXGIFactory1*             _factory;
};
class DeviceSettingsD3D11
{
  public:
    DeviceSettingsD3D11 ();
    DeviceSettingsD3D11 (const DeviceSettingsD3D11& settings);

    uint32_t                   _adapterOrdinal;
    D3D_DRIVER_TYPE            _driverType;
    uint32_t                   _output;
    DXGI_SWAP_CHAIN_DESC       _sd;
    uint32_t                   _createFlags;
    uint32_t                   _syncInterval;
    uint32_t                   _presentFlags;
    bool                       _autoCreateDepthStencil; 
    DXGI_FORMAT                _autoDepthStencilFormat;
    DXGI_SAMPLE_DESC           _sampleDesc;
};

class DeviceD3D11 : public IDevice
{
  public:
    DeviceD3D11();
    virtual ~DeviceD3D11();

    Ibl::Window*                renderWindow();
    bool                       initialize (const Ibl::ApplicationRenderParameters& deviceResource);

    ID3D11DeviceContext*       immediateCtx() { return _immediateCtx;}
    bool                       beginRender();
    bool                       present();


    virtual void               printState();
    virtual void               syncState();

    operator                   ID3D11Device* const&() { return _direct3d; }

    bool                       initialized() const;
    
    virtual bool               reset();


    operator ID3D11Device* const&() const { return _direct3d; }
    ID3D11Device* const& operator->() const { return _direct3d; }

    virtual const Ibl::ISurface*    backbuffer() const;
    virtual const Ibl::IDepthSurface* depthbuffer() const;
    virtual const Ibl::FrameBuffer& deviceFrameBuffer() const;
    // Resource management functions
    virtual IGpuBuffer *        createBufferResource (const Ibl::RenderResourceParameters* data = 0);
    virtual IVertexBuffer *     createVertexBuffer (const Ibl::RenderResourceParameters* data = 0);
    virtual IIndexBuffer *      createIndexBuffer (const Ibl::RenderResourceParameters* data = 0);
    virtual IVertexDeclaration * createVertexDeclaration (const Ibl::RenderResourceParameters* data = 0);
    virtual IDepthSurface *     createDepthSurface(const Ibl::RenderResourceParameters* data = 0);
    virtual ITexture *          createTexture (const Ibl::RenderResourceParameters* data = 0);
    virtual IComputeShader *    createComputeShader (const Ibl::RenderResourceParameters* data = 0);
    virtual IShader *           createShader (const Ibl::RenderResourceParameters* data = 0);

    virtual void                destroyResource(Ibl::IRenderResource* resource);

    virtual void               resetShaderPipeline();

    bool                       setColorWriteState (bool r  = true, bool g  = true, 
                                                   bool b  = true, bool a = true);

    virtual bool               drawPrimitive (const IVertexDeclaration*, 
                                              const IVertexBuffer*, 
                                              const GpuTechnique* technique,
                                              PrimitiveType, 
                                              uint32_t) const;

    virtual bool               drawIndexedPrimitive (const IVertexDeclaration*, 
                                                     const IIndexBuffer*, 
                                                     const IVertexBuffer*, 
                                                     const GpuTechnique* technique,
                                                     PrimitiveType, 
                                                     uint32_t faceCount,
                                                     uint32_t vertexCount) const;

    virtual bool               blitSurfaces (const ISurface* destination, 
                                             const ISurface* src, 
                                             TextureFilter filterType = Ibl::TEXFILTER_POINT,
                                             size_t arrayOffset = 0) const;

    virtual bool                blitSurfaces (const IDepthSurface* destination, 
                                              const IDepthSurface* src, 
                                              TextureFilter filterType =Ibl::TEXFILTER_POINT) const;

    virtual bool               clearSurfaces (uint32_t index, unsigned long clearType, 
                                              float redClear = 0.0f, 
                                              float greenClear = 0.0f, 
                                              float blueClear = 0.0f, 
                                              float alphaClear = 0.0f) const;

    virtual bool               setNullTarget (uint32_t index);

    virtual void               setNullStreamOut();

    virtual void               setViewport (const Viewport*);
    virtual void               getViewport(Viewport*) const;

    virtual void*              rawDevice();

    virtual bool               writeFrontBufferToFile (const std::string& ) const;

    // State Management Functions
    virtual void                enableAlphaBlending();
    virtual void                disableAlphaBlending();

    virtual void                setAlphaToCoverageEnable (bool value);
 
    virtual void                setBlendProperty (const Ibl::BlendOp&);
    virtual void                setSrcFunction (const Ibl::AlphaFunction&);
    virtual void                setDestFunction (const Ibl::AlphaFunction&);
 
    virtual void                setAlphaBlendProperty (const Ibl::BlendOp&);
    virtual void                setAlphaDestFunction (const Ibl::AlphaFunction&);
    virtual void                setAlphaSrcFunction (const Ibl::AlphaFunction&);

    virtual void                fogEnable();
    virtual void                fogDisable();
    
    virtual void                enableZTest();
    virtual void                disableZTest();

    virtual void                disableDepthWrite();
    virtual void                enableDepthWrite();

    virtual void                setZFunction (Ibl::CompareFunction);
    virtual void                setupBlendPipeline(Ibl::BlendPipelineType blendPipelineType);
    virtual Ibl::BlendPipelineType blendPipeline() const;

    virtual void                setFrontFaceStencilFunction(Ibl::CompareFunction);
    virtual void                setFrontFaceStencilPass(Ibl::StencilOp compareFunc);

    virtual void                enableStencilTest();
    virtual void                disableStencilTest();
    virtual void                setCullMode (Ibl::CullMode);
    virtual void                setNullPixelShader();
    virtual void                setNullVertexShader();

    virtual bool                isRenderTextureFormatSupported (const Ibl::PixelFormat& format);

    virtual void                setDrawMode (Ibl::DrawMode);
    virtual Ibl::DrawMode getDrawMode () const;

    virtual Ibl::CullMode cullMode() const;

    virtual void                copyStructureCount(const Ibl::IGpuBuffer* dst, const Ibl::IGpuBuffer* src);

    virtual bool                supportsHardwareTessellationStage() const;
    ID3D11DepthStencilView*     primaryDepthStencilView();
    ID3D11RenderTargetView*     primaryBackBufferView();
    
    virtual void                bindNullFrameBuffer();
    virtual void                bindSurface (int level, const Ibl::ISurface* surface);
    virtual void                bindDepthSurface (const Ibl::IDepthSurface* surface);
    void                        bindSurfaceAndTargets();

    virtual bool                resizeDevice (const Ibl::Vector2i& newSize);

    virtual bool                texelIsCenter() { return false; }

    virtual bool                bindFrameBuffer (const Ibl::FrameBuffer& framebuffer);

    virtual void                setupViewport (const Ibl::FrameBuffer& frameBuffer);

    virtual void                resetViewsAndShaders() const;
    virtual void                clearShaderResources() const;


  protected:

    void                        initializeDeviceStates();
    void                        bindBlendState();
    void                        bindDepthState();
    void                        bindRasterState();

  private:
    D3D_FEATURE_LEVEL          _level;
    DXGIFactory *              _factory;
    ID3D11Device*              _direct3d;
    IDXGISwapChain*            _swapChain;
    IDXGIAdapter1*             _adapter;
    DeviceSettingsD3D11        _creationParameters;
    Ibl::RenderWindow*          _window;
    bool                       _initialized;
    D3D11_VIEWPORT             _currentViewport;
    ID3D11DeviceContext*       _immediateCtx;

    SurfaceD3D11*              _backbuffer;
    DepthSurfaceD3D11*         _depthbuffer;
    Ibl::FrameBuffer            _deviceFrameBuffer;

    const ISurface*             _currentSurfaces [MAX_RENDER_TARGETS];
    uint32_t                    _currentSurfaceCount;
    uint32_t                    _currentUAVCount;
    const IRenderResource*      _currentUnorderedSurfaces[MAX_RENDER_TARGETS];
    const IDepthSurface*        _currentDepthSurface;

    Ibl::DrawMode                 _drawMode;
    Ibl::CullMode                 _cullMode;

    ID3D11RasterizerState *    _currentRasterState;
    D3D11_RASTERIZER_DESC      _currentRasterStateDesc;

    ID3D11DepthStencilState*   _currentDepthState;
    D3D11_DEPTH_STENCIL_DESC   _currentDepthStateDesc;

    ID3D11BlendState*          _currentBlendState;

    
    D3D11_BLEND_DESC           _currentBlendStateDesc;

    Ibl::Vector4f              _blendFactor;
    Ibl::BlendPipelineType     _blendPipelineType;
};

}

#endif