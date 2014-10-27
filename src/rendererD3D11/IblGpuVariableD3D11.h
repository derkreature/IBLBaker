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

#ifndef INCLUDED_TECHNIQUE_SAS_VARIABLE
#define INCLUDED_TECHNIQUE_SAS_VARIABLE

#include <IblPlatform.h>
#include <IblShaderParameterValue.h>
#include <IblGpuVariable.h>

namespace Ibl
{
class Hash;

class IEffect;
class ITexture;
class ShaderParameterValue;
class GpuTechnique;
class TextureD3D11;

//---------------------------------
// Manages a directx HLSL Variable
//---------------------------------
class GpuVariableD3D11 : public Ibl::GpuVariable
{
  public:
    GpuVariableD3D11(Ibl::IDevice* device);
    virtual ~GpuVariableD3D11();

    void                        setParameterType (Ibl::ShaderParameter type);
    virtual bool                initialize (Ibl::IEffect* effect, 
                                            const int& index);

    virtual bool                create(){ return true; };
    virtual bool                cache(){ return true; };
    virtual bool                free();

    virtual void                set (const void*, uint32_t size) const;
    virtual void                setMatrix(const float*) const;
    virtual void                setMatrixArray (const float*, uint32_t size) const;
    virtual void                setVectorArray (const float*, uint32_t size) const;    
    virtual void                setVector (const float*) const;
    virtual void                setFloatArray(const float*, uint32_t count) const;
    virtual void                setTexture (const Ibl::ITexture*) const;
    virtual void                setDepthTexture (const Ibl::IDepthSurface*) const;
    virtual void                setResource (const Ibl::IGpuBuffer*) const;
    virtual void                setUnorderedResource(const Ibl::IGpuBuffer*) const;
    virtual void                setStream (const Ibl::IVertexBuffer* vertexBuffer) const;
    virtual void                setStream (const Ibl::IIndexBuffer* indexBuffer) const;

    //-----------------------
    // Gets the semantic name
    //-----------------------
    const std::string&          semantic() const;

    //-----------------------
    // Gets the variable name
    //-----------------------
    const std::string&          name() const;

    //---------------------------
    // Gets the annotation stream
    //---------------------------
    const std::string&          annotation (const std::string&);

    //---------------------------------
    // Gets the handle for the variable
    //---------------------------------
    ID3DX11EffectVariable*      handle() const;

    //--------------------------
    // Gets the shader parameter
    //--------------------------
    Ibl::ShaderParameter    parameterType() const;

    //-------------------------------------
    // Get the default texture if available
    //-------------------------------------
    virtual const Ibl::ITexture*    texture() const;


    Ibl::ShaderParameterValue* shaderParameterValue() const;

    Ibl::IEffect*         effect() const;

    virtual void                unbind() const;

  protected:
    typedef std::map<std::string, std::string>  StringMap;
      
    //---------------------------------------
    // Internal handle to the shader variable
    //---------------------------------------
    ID3DX11EffectVariable*      _handle;

    //---------------------------------------
    // Description structure of the parameter    
    //---------------------------------------
    D3DX11_EFFECT_VARIABLE_DESC _parameterDescription;

    //------------------
    // The variable name
    //------------------
    std::string                 _name;

    //------------------------------
    // Semantic Name of the variable
    //------------------------------
    std::string                 _semantic;

    //-------------------------------------
    // List of annotations for the variable
    //-------------------------------------
    StringMap                   _annotations;
    Ibl::ShaderParameter  _parameterType;
    const TextureD3D11*         _texture;
    Ibl::ShaderParameterValue*    _shaderValue;
    Ibl::IEffect*         _effect;
    
    D3DX11_EFFECT_TYPE_DESC      _variableDesc;
    ID3DX11EffectType*           _effectType;

};
}
#endif
