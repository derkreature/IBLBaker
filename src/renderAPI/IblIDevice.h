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

#ifndef INCLUDED_BB_DEVICE_INTERFACE
#define INCLUDED_BB_DEVICE_INTERFACE

#include <IblIRenderResource.h>
#include <IblRenderWindow.h>
#include <IblRenderEnums.h>
#include <IblIDepthSurface.h>
#include <IblISurface.h>
#include <IblViewport.h>
#include <IblFrameBuffer.h>

namespace Ibl
{
class Window;
class DeviceParameters;
class IIndexBuffer;
class GpuTechnique;
class IVertexBuffer;
class IVertexDeclaration;
class ISurface;
class Mesh;
class RenderAPIFactory;
class IGpuBuffer;
class IDepthSurface;
class ITexture;
class IShader;
class IComputeShader;
class IUberShaderGraph;
class RenderResourceParameters;
class ShaderMgr;
class VertexDeclarationMgr;
class TextureMgr;
class PostEffectsMgr;
class ShaderParameterValueFactory;
class DepthResolve;
class ColorResolve;


class ApplicationRenderParameters 
{
  public:
    ApplicationRenderParameters ();
    ApplicationRenderParameters (const Ibl::Application* application,
                                 const std::string& applicationName,
                                 const Ibl::Vector2i& windowSize,
                                 bool windowed);
    ApplicationRenderParameters (const Ibl::Application* application);
    ~ApplicationRenderParameters ();

    // Load parameters from xml.
    bool                       load(const std::string& filePathName);

    const Ibl::Application*     application() const;
    WindowHandle               windowHandle() const;
    bool                       multithreaded() const;

    const Ibl::Vector2i&        size() const;    
    bool                       windowed() const;
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

class IDevice 
{
  public:
    IDevice ();
    virtual ~IDevice();

    void                        setSceneDrawMode(Ibl::DrawMode);
    Ibl::DrawMode                sceneDrawMode() const;

    void                        update();

    virtual void                printState() = 0;
    virtual void                syncState() = 0;

    virtual IGpuBuffer *        createBufferResource (const Ibl::RenderResourceParameters* data = 0) = 0;
    virtual IVertexBuffer *     createVertexBuffer (const Ibl::RenderResourceParameters* data = 0) = 0;
    virtual IIndexBuffer *      createIndexBuffer (const Ibl::RenderResourceParameters* data = 0) = 0;
    virtual IVertexDeclaration * createVertexDeclaration (const Ibl::RenderResourceParameters* data = 0) = 0;
    virtual IDepthSurface *     createDepthSurface(const Ibl::RenderResourceParameters* data = 0) = 0;
    virtual ITexture *          createTexture (const Ibl::RenderResourceParameters* data = 0) = 0;
    virtual IComputeShader *    createComputeShader (const Ibl::RenderResourceParameters* data = 0) = 0;
    virtual IShader *           createShader (const Ibl::RenderResourceParameters* data = 0) = 0;

    virtual void                destroyResource(Ibl::IRenderResource* resource) = 0;
    virtual void                setupBlendPipeline(BlendPipelineType blendPipelineType) = 0;

    virtual bool                create() { return true; };
    virtual bool                cache() { return true; };
    virtual bool                free();

    virtual bool                initialize (const Ibl::ApplicationRenderParameters& deviceParameters);
    virtual bool                postInitialize(const Ibl::ApplicationRenderParameters& deviceParameters);

    virtual bool                reset() = 0;
    virtual bool                beginRender() = 0;
    virtual bool                present() = 0;
    virtual bool                supportsHardwareTessellationStage() const = 0;

    virtual void                bindSurface (int level, const ISurface* surface) {};
    virtual void                bindDepthSurface (const IDepthSurface* surface) {};

    // State Management Functions
    virtual void                enableAlphaBlending() = 0;
    virtual void                disableAlphaBlending() = 0;

    virtual void                fogEnable() = 0;
    virtual void                fogDisable() = 0;
    virtual void                disableDepthWrite() = 0;
    virtual void                enableDepthWrite() = 0;

    virtual void                disableZTest() = 0;
    virtual void                enableZTest() = 0;

    virtual void                setZFunction (CompareFunction) = 0;

    virtual void                setDrawMode (DrawMode) = 0;
    virtual DrawMode            getDrawMode () const = 0;

    virtual void                resetShaderPipeline() = 0;

    virtual void                copyStructureCount(const Ibl::IGpuBuffer* dst, const Ibl::IGpuBuffer* src) = 0;

    virtual void                setFrontFaceStencilFunction(Ibl::CompareFunction) = 0;
    virtual void                setFrontFaceStencilPass(Ibl::StencilOp compareFunc) = 0;
    virtual void                enableStencilTest() = 0;
    virtual void                disableStencilTest() = 0;


    
    virtual void                setupStencil(uint8_t readMask,
                                             uint8_t writeMask,
                                             Ibl::CompareFunction frontCompare,
                                             Ibl::StencilOp frontStencilFailOp,
                                             Ibl::StencilOp frontStencilPassOp,
                                             Ibl::StencilOp frontZFailOp,
                                             Ibl::CompareFunction backCompare,
                                             Ibl::StencilOp backStencilFailOp,
                                             Ibl::StencilOp backStencilPassOp,
                                             Ibl::StencilOp backZFailOp) = 0;

    virtual void                setupStencil(uint8_t readMask,
                                             uint8_t writeMask,
                                             Ibl::CompareFunction frontCompare,
                                             Ibl::StencilOp frontStencilFailOp,
                                             Ibl::StencilOp frontStencilPassOp,
                                             Ibl::StencilOp frontZFailOp) = 0;
    
    virtual Ibl::CullMode cullMode() const = 0;
    virtual void                setCullMode (CullMode) = 0;
    virtual void                setNullPixelShader() = 0;
    virtual void                setNullVertexShader() = 0;
    virtual void                setNullStreamOut() {};

    virtual bool                drawPrimitive (const IVertexDeclaration*, 
                                               const IVertexBuffer*, 
                                               const GpuTechnique *,
                                               PrimitiveType, 
                                               uint32_t faceCount,
                                               uint32_t vertexOffset) const = 0;

    virtual bool                drawIndexedPrimitive (const IVertexDeclaration*, 
                                                      const IIndexBuffer*, 
                                                      const IVertexBuffer*, 
                                                      const GpuTechnique *,
                                                      PrimitiveType, 
                                                      uint32_t faceCount, 
                                                      uint32_t indexOffset,
                                                      uint32_t vertexOffset) const = 0;
    
    virtual bool                blitSurfaces (const ISurface* destination, 
                                              const ISurface* src, 
                                              TextureFilter filterType = TEXFILTER_POINT,
                                              size_t arrayOffset = 0) const = 0;

    virtual bool                blitSurfaces (const IDepthSurface* destination, 
                                              const IDepthSurface* src, 
                                              TextureFilter filterType = TEXFILTER_POINT) const = 0;

    virtual bool                clearSurfaces (uint32_t index, unsigned long clearType, 
                                               float redClear = 0.0f, 
                                               float greenClear = 0.0f, 
                                               float blueClear = 0.0f, 
                                               float alphaClear = 0.0f) const = 0;

    virtual bool                setNullTarget (uint32_t index) = 0;

    virtual bool                setColorWriteState (bool r, bool g, bool b, bool a) = 0;

    virtual void*               rawDevice() = 0;
    virtual Window*             renderWindow() = 0;

    virtual bool                isRenderTextureFormatSupported (const Ibl::PixelFormat& format) = 0;

    virtual void                setViewport(const Viewport*) = 0;
    virtual void                getViewport(Viewport*) const = 0;

    virtual const ISurface*     backbuffer() const = 0;
    virtual const IDepthSurface* depthbuffer() const = 0;

    virtual void                setAlphaToCoverageEnable (bool value) = 0;
 
    virtual void                setBlendProperty (const Ibl::BlendOp&) = 0;
    virtual void                setSrcFunction (const Ibl::AlphaFunction&) = 0;
    virtual void                setDestFunction (const Ibl::AlphaFunction&) = 0;
    virtual Ibl::BlendPipelineType blendPipeline() const = 0;

    virtual bool                 scissorEnabled() const = 0;
    virtual void                 setScissorEnabled(bool scissorEnabled) = 0;
    virtual void                 setScissorRect(int x, int y, int width, int height) = 0;

    virtual void                setAlphaBlendProperty (const Ibl::BlendOp&) = 0;
    virtual void                setAlphaDestFunction (const Ibl::AlphaFunction&) = 0;
    virtual void                setAlphaSrcFunction (const Ibl::AlphaFunction&) = 0;

    virtual bool                writeFrontBufferToFile (const std::string& filename) const = 0;

    virtual uint32_t            multiSampleCount() const {return _multiSampleCount; };
    virtual uint32_t            multiSampleQuality() const { return _multiSampleQuality; }
    virtual bool                useMultiSampleAntiAliasing() const { return _useMultiSampleAntiAliasing; }

    virtual bool                resizeDevice (const Ibl::Vector2i& newSize) = 0;
    virtual bool                texelIsCenter() = 0;

    virtual const Ibl::FrameBuffer& deviceFrameBuffer() const = 0;
    virtual bool                bindFrameBuffer (const Ibl::FrameBuffer& framebuffer) = 0;
    virtual void                setupViewport (const Ibl::FrameBuffer& frameBuffer) = 0;

    // Get a disposable render target from the core pool
    virtual Ibl::ITexture* sharedRenderTarget(int width, int height, Ibl::PixelFormat, bool useUAV = false);
    virtual void                resetViewsAndShaders() const = 0;
    virtual void                clearShaderResources() const = 0;

    bool                        usePrecompiledShaders() const;
    void                        setUsePrecompiledShaders(bool);

    ShaderMgr*                   shaderMgr();
    VertexDeclarationMgr*        vertexDeclarationMgr();
    TextureMgr*                  textureMgr();
    ShaderParameterValueFactory* shaderValueFactory();
    PostEffectsMgr *             postEffectsMgr();

  protected:
    bool                         _useMultiSampleAntiAliasing;
    uint32_t                     _multiSampleCount;
    uint32_t                     _multiSampleQuality;
    Ibl::FrameBuffer              _currentFrameBuffer;
    Ibl::DrawMode                 _sceneDrawMode;
    bool                         _usePrecompiledShaders;
    std::vector <Ibl::ITexture*>  _temporaryTexturePool;
    const Ibl::Application*       _application;

    Ibl::ShaderMgr*               _shaderMgr;
    Ibl::VertexDeclarationMgr*    _vertexDeclarationMgr;
    TextureMgr*                  _textureMgr;
    ShaderParameterValueFactory* _shaderValueFactory;
    PostEffectsMgr *             _postEffectsMgr;
    DepthResolve*                _depthResolveEffect;
    ColorResolve*                _colorResolveEffect;
};
}

#endif