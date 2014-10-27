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

#ifndef INCLUDED_BB_BRDF_LUT
#define INCLUDED_BB_BRDF_LUT

#include <IblPlatform.h>
#include <IblTransformNode.h>
#include <IblIDevice.h>
#include <IblTextureImage.h>
#include <IblHash.h>

namespace Ibl
{
class Entity;
class IDevice;
class IVertexDeclaration;
class IVertexBuffer;
class GpuTechnique;
class VertexStream;
class Mesh;
class Camera;
class IGpuBuffer;
class IShader;
class Scene;
class RenderRequest;
class Material;

class Brdf : public Ibl::RenderNode
{
  public:
    Brdf (Ibl::IDevice* device);
    virtual ~Brdf();

    bool                       load(const std::string& brdfInclude);

    void                       compute();
    const Ibl::ITexture*       brdfLut() const;

    const Ibl::IShader*        specularImportanceSamplingShader() const;
    const Ibl::IShader*        diffuseImportanceSamplingShader() const;

  private:
    Ibl::ITexture*             _brdfLut;

    const Ibl::IComputeShader* _brdfLutShader;
    Ibl::Hash                  _hash;

    // Cubemap importance sampling variables and shader.
    const Ibl::IShader*        _importanceSamplingShaderSpecular;
    const Ibl::IShader*        _importanceSamplingShaderDiffuse;

};
}

#endif
