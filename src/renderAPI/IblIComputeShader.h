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
#ifndef INCLUDED_BB_COMPUTESHADER_INTERFACE
#define INCLUDED_BB_COMPUTESHADER_INTERFACE

#include <IblIRenderResource.h>
#include <IblHash.h>

namespace Ibl
{
class GpuTechnique;
class Mesh;
class GpuVariable;
class IEffect;
class PostEffect;
class Light;
class Camera;
class Entity;
class LightList;

class
IComputeShader : public IRenderResource
{
  public:
    IComputeShader (Ibl::IDevice* device);
    virtual ~IComputeShader();

    virtual bool                initializeFromFile (const std::string& shaderFilePathName,
                                                    const std::string& includeFilePathName,
                                                    const std::string& functionName,
                                                    const std::map<std::string, std::string>& defines) = 0;

    virtual bool                initializeFromStream (const std::string& file,
                                                      const std::string& functionName,
                                                      const std::map<std::string, std::string>& defines) = 0;

    virtual bool                bind()  const = 0;
    virtual bool                unbind() const = 0;

    virtual bool                dispatch(const Ibl::Vector3i& groupBounds) const = 0;

    virtual bool                setResources(const std::vector<const Ibl::IRenderResource*> & texture) const = 0;
    virtual bool                setViews(const std::vector<const IRenderResource*>& texture)  const = 0;

    virtual bool                createConstantBuffer(size_t) = 0;
    virtual bool                updateConstantBuffer(void*, size_t) = 0;

    virtual const std::string&  filePathName() const = 0;
    virtual const std::string&  includePathName() const =0;

    const Hash&                 hash() const;

  protected:
    Hash                        _hash;
};
}

#endif