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
#ifndef INCLUDED_BB_RENDER_TARGET
#define INCLUDED_BB_RENDER_TARGET

#include <IblPlatform.h>
#include <IblRenderTargetQuad.h>
#include <IblITexture.h>
#include <IblISurface.h>

namespace Ibl
{
class Camera;
class ShaderMgr;
class IShader;
class GpuTechnique;

enum LensDistortionType
{
    NoLensDistortion = 0,
    LensUndistort = 1,
    LensDistort = 2
};

//--------------------------------------------------
// Sensate Render Target
// Base Class for Render Target Systems
// These include realtime reflections, shadowmaps, 
// glow systems etc...
//--------------------------------------------------
class PostEffect  : public Ibl::RenderTargetQuad
{
  public:
    PostEffect(Ibl::IDevice* device, PixelFormat format = Ibl::PF_A8R8G8B8, LensDistortionType distortionType = NoLensDistortion);
    virtual ~PostEffect();
     
    //---------------------------------------
    //release device resources on lost device
    //---------------------------------------
    virtual bool                free();
    virtual bool                create();
    virtual bool                cache();

    //-----------------------------
    //creates devices and resources 
    //-----------------------------
    virtual bool                initialize (const std::string& filename);

    //------------------------
    // vanilla render override
    //------------------------
    virtual bool                render (const Ibl::ITexture* surface, 
                                        const Ibl::Camera* camera);

    virtual bool                render (const Ibl::ITexture* textureIn, 
                                        const Ibl::ITexture* textureOut,
                                        const Ibl::Camera* camera);

    bool                        render (const Ibl::ITexture* texture, 
                                        const Ibl::ISurface* surface,
                                        const Ibl::Camera* camera);

    bool                        composite (const Ibl::ISurface* surface,
                                           std::vector<const Ibl::ITexture*> & inputs,
                                           const Ibl::Camera* camera);

    bool                        composite (const Ibl::GpuTechnique* technique,
                                           const Ibl::ISurface* surface,
                                           std::vector<const Ibl::ITexture*> & inputs,
                                           const Ibl::Camera* camera);

    bool                        composite (const Ibl::GpuTechnique* technique,
                                           const Ibl::ISurface* surface,
                                           std::vector<const Ibl::IGpuBuffer*> & uavs,
                                           std::vector<const Ibl::ITexture*> & inputs,
                                           const Ibl::Camera* camera);

    //------------------------------------------------------------
    // This is mainly here for presentation policies and blurring.
    //------------------------------------------------------------
    virtual const ITexture*     lastRenderTarget() const { return 0; }

    const Ibl::ITexture*  input (uint32_t index) const;

    //-----------------------------
    //get the render target texture
    //-----------------------------
    virtual Ibl::ITexture* texture() const {return _postEffectTexture;}

    //-------------------------------------------------
    // Returns the current source texture to draw with.
    //-------------------------------------------------
    const Ibl::ITexture*  currentSource() const;

    //------------------------------------------------
    //Updates the render target (optional to override)
    //------------------------------------------------
    virtual bool                update (float elapsed);

    //------------------------------------------
    // Gets the client rectangle for the surface
    //------------------------------------------
    const Region2i&                postEffectBounds() const;

    //----------------------------
    // Gets the name of the target
    //----------------------------
    const std::string&          name() const;

    //---------------------------
    //sets the name of the target
    //---------------------------
    void                        name (const std::string& _name);

    uint32_t                    targetHeight() { return _postEffectBounds.size().y; }
    uint32_t                    targetWidth() { return _postEffectBounds.size().x; }

    bool                        verticalPass () const;
    void                        setVerticalPass (bool);

    virtual bool                recreateOnResize() { return true; }

  protected:
    
    //---------------------
    //Render Texture Target
    //---------------------
    Ibl::ITexture*            _postEffectTexture;


    //--------------------------
    // The current input texture
    //--------------------------
    const Ibl::ITexture*      _textureIn;

    //------------------------
    // Current input list
    //------------------------
    std::vector<const Ibl::ITexture*>        _inputs;

    //----------------------------------
    // The shader that does all the work
    //----------------------------------
    const Ibl::IShader*       _shader;

    //---------------------
    // The shader technique
    //---------------------
    const Ibl::GpuTechnique*  _technique;

    //---------------------
    // The client rectangle
    //---------------------
    Ibl::Region2i                      _postEffectBounds;

    //-------------------------------------
    // The name of the render target effect
    //-------------------------------------
    std::string                     _name;
    std::string                     _filename;    
    PixelFormat                     _format;
    bool                            _verticalPass;
};

}

#endif
