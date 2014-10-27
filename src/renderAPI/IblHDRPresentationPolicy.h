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
#ifndef INCLUDED_BB_HDR_PRESENTATION_POLICY
#define INCLUDED_BB_HDR_PRESENTATION_POLICY

#include <IblPlatform.h>
#include <IblPresentationPolicy.h>

namespace Ibl
{
class HDRPresentationPolicy : public PresentationPolicy
{
  public:
    HDRPresentationPolicy(Ibl::IDevice* device);
    virtual ~HDRPresentationPolicy();

    virtual bool               prepareForBackBuffer(const Camera* camera);
    
    virtual const ITexture*    renderTarget() const;
    virtual const ITexture*    displayTarget() const;

    virtual const ITexture*    lastRenderTarget() const;

    virtual const ISurface*    renderTargetSurface() const;
    virtual const ISurface*    displayTargetSurface() const;

    virtual const ISurface*    fullSceneBackBufferSurface() const;
    virtual const ISurface*    opaqueBackBufferSurface() const;


    virtual bool               free();
    virtual bool               create();
    virtual bool               cache();

    bool                       usePostFXAA() const;
    void                       setUsePostFXAA(bool);

    size_t                     currentFXAAPreset() const;
    void                       setCurrentFXAAPreset(int);

    void                       renderFXAA (ITexture *src,
                                           ITexture *dst,
                                           const Camera* camera);

    virtual const FrameBuffer& sceneFrameBuffer() const;
  protected:

  private:
    bool                       _useAntiAliasing;

    ITexture*                  _renderTargetA;
    ITexture*                  _renderTargetB;
    ITexture*                  _renderTarget;
    ITexture*                  _lastRenderTarget;
    ITexture*                  _sourceTexture;

    std::vector<const Ibl::IShader*>        _fxaaShader;
    bool                       _usePostFXAA;
    size_t                     _currentFXAAPreset;

    const Ibl::IShader* _lumBloomShader;
    const Ibl::IShader* _effectsMergeShader;

};
}

#endif