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

#include <IblBrdf.h>
#include <IblShaderMgr.h>
#include <IblIComputeShader.h>
#include <IblTextureMgr.h>
#include <IblITexture.h>
#include <IblIComputeShader.h>
#include <IblLog.h>

namespace Ibl
{
Brdf::Brdf(Ibl::IDevice* device) :
    RenderNode(device),
    _brdfLut(nullptr),
    _brdfLutShader(nullptr),
    _importanceSamplingShaderSpecular (nullptr),
    _importanceSamplingShaderDiffuse (nullptr)
{
}

bool
Brdf::load(const std::string& brdfInclude)
{
    // Load the importance sampling shader and variables.
    if (!_device->shaderMgr()->addShader("IblImportanceSamplingSpecular.fx", brdfInclude, _importanceSamplingShaderSpecular, true, true))
    {
        THROW("Could not add importance sampling shader");
    }
    // Load the importance sampling shader and variables.
    if (!_device->shaderMgr()->addShader("IblImportanceSamplingDiffuse.fx", brdfInclude, _importanceSamplingShaderDiffuse, true, true))
    {
        THROW("Could not add importance sampling shader");
    }

    _device->shaderMgr()->addComputeShaderFromFile("IblBrdf.hlsl", brdfInclude, "CSMain", _brdfLutShader,
        std::map<std::string, std::string>());

    _brdfLut = _device->
        createTexture(&TextureParameters("tempTex",
        Ibl::TwoD,
        Ibl::RenderTarget,
        PF_FLOAT32_RGBA,
        Ibl::Vector3i(256, 256, 1),
        false,
        1, 1, 0, 1, true));

    assert(_brdfLut);

    setName(brdfInclude);

    return true;
}

Brdf::~Brdf()
{
    safedelete(_brdfLut);
}

void
Brdf::compute()
{
    if (_hash != _brdfLutShader->hash())
    {
        _hash = _brdfLutShader->hash();
        try
        {
            // Render
            std::vector<const Ibl::IRenderResource*> views;
            views.push_back(_brdfLut);

            _brdfLutShader->setViews(views);
            _brdfLutShader->bind();
            _brdfLutShader->dispatch(Ibl::Vector3i(256 / 16, 256 / 16, 1));
            _brdfLutShader->unbind();

            _hash = _brdfLutShader->hash();

            // Save
            _brdfLut->save("data/Textures/Procedural/Output.dds");
        }
        catch (const std::exception& ex)
        {
            LOG("Exception while computing BRDF!" << ex.what())
            assert(0);
        }
    }
}

const Ibl::ITexture*
Brdf::brdfLut() const
{
    return _brdfLut;
}

const Ibl::IShader*
Brdf::specularImportanceSamplingShader() const
{
    return _importanceSamplingShaderSpecular;
}

const Ibl::IShader*
Brdf::diffuseImportanceSamplingShader() const
{
    return _importanceSamplingShaderDiffuse;
}


}