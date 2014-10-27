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
#ifndef INCLUDED_BB_POST_EFFECTS
#define INCLUDED_BB_POST_EFFECTS

#include <IblPlatform.h>
#include <IblFrameBuffer.h>
#include <IblVector2.h>
#include <IblVector3.h>
#include <IblVector4.h>

namespace Ibl
{
class Application;
class IDevice;
class ISurface;
class ITexture;
class PresentationPolicy;
class PostEffect;
class Camera;

//----------------------------------
// Structure for post effect display
//----------------------------------
struct POSTEFFECTVERTEX
{
    Ibl::Vector3f p;
    float tu;
    float tv;
};

enum PostEffectPresentationPolicy
{
    Basic,
    HDR
};

class PostEffectsMgr 
{
  public:
    PostEffectsMgr(const Ibl::Application* application,
                   Ibl::IDevice* device);
    virtual ~PostEffectsMgr();

    //----------------------------------------
    // Release device resources on lost device
    //----------------------------------------
    virtual bool                free();

    virtual bool                create();

    //--------------------------------
    // Updates the post effect pathway
    //--------------------------------
    bool                        update(float delta);

    //--------------------------------
    // Renders the post effect pathway
    //--------------------------------
    void                        render (const Camera* camera) const;

    //------------------------------------------
    // gets whether the system should apply post
    //------------------------------------------
    bool                        usingPostEffects()
        { return _usingPostEffects; }

    //------------------------------------------
    // sets whether the system should apply post
    //------------------------------------------
    void                        setUsingPostEffects(bool in)
        { _usingPostEffects = in; }

    const ITexture*             presentationBackBuffer();

    //------------------------------------------------
    // clears all post effects from the render pathway
    //------------------------------------------------
    bool                        clearPathway();

    //-------------------------------------------
    // adds a post effect to the chain
    //-------------------------------------------        
    void                        addPostEffect(PostEffect* postEffect);

    //----------------------------------------------
    // removes a post effect from the render pathway
    //----------------------------------------------
    void                        removePostEffect (const PostEffect* rt = 0);

    //----------------
    // Finds an effect
    //----------------
    bool                        findEffect (PostEffect*& target, 
                                            const std::string& name);
    bool                        findEffect (const PostEffect*& target, 
                                            const std::string& name) const;

    uint32_t                    postEffectCount() const;
    const PostEffect*           postEffect (const uint32_t& i) const;

    bool                        removePostEffect (const uint32_t& index);

    PresentationPolicy*         activePolicy ();
    bool                        setActivePolicy (PostEffectPresentationPolicy policyType);

    const FrameBuffer&          sceneFrameBuffer() const;

  protected:
    typedef std::map <PostEffectPresentationPolicy, 
                      PresentationPolicy*>       PresentationPolicyMap;

    //-----------------------------------
    // Manufactures a complex post effect
    //-----------------------------------
    PostEffect*                    manufacture(const std::string& name);

    //-----------------------------------------------------
    // boolean indicating if post effects should be applied
    //-----------------------------------------------------
    bool                        _usingPostEffects;        
    
    //----------------------------
    // The primary d3d back buffer
    //----------------------------
    ISurface*                    _backBufferSurface;

    //-----------------------------
    // The list of active processes
    //-----------------------------
    std::vector<PostEffect*>     _postEffects;

    PresentationPolicy*          _activePolicy;

    PresentationPolicyMap        _presentationPolicies;

    Ibl::IDevice*                 _deviceInterface;
};
}

#endif 
