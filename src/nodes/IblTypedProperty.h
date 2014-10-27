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
#ifndef INCLUDED_TYPED_PROPERTY
#define INCLUDED_TYPED_PROPERTY

#include <IblProperty.h>
#include <IblPlatform.h>
#include <IblTypedProperty.h>
#include <IblLog.h>
#include <IblVector3.h>
#include <IblQuaternion.h>
#include <IblMatrix44.h>
#include <IblRegion.h>
#include <IblVertexStream.h>
#include <IblViewport.h>
#include <IblIDevice.h>

namespace Ibl
{
class ITexture;
class IDepthSurface;
class ISurface;
class IRenderResource;

template <typename T>
class TypedProperty : public Property
{
  public:
    TypedProperty(Node* node, const std::string& name, Node* group = nullptr);
    TypedProperty(Node* node, const std::string& name, TweakFlags* flags);
    virtual ~TypedProperty();

    virtual T&                  get();
    virtual const T&            get() const;
    virtual void                set (const T& value);

  protected:
    T                           _value;

  private:
};

template <typename T>
TypedProperty<T>::TypedProperty (Node* node, 
                                 const std::string& name,
                                 Node* group) :
    Property (node, name, group)
{
}

template <typename T>
TypedProperty<T>::TypedProperty(Node* node,
                                const std::string& name, 
                                TweakFlags* flags) :
    Property(node, name, flags)
{
}

template <typename T>
TypedProperty<T>::~TypedProperty()
{
}

template <typename T>
T&
TypedProperty<T>::get()
{
    if (_group && !_cached)
    {
        _group->cache(this);
    }
    return _value;
}

template <typename T>
const T&
TypedProperty<T>::get() const
{
    if (_group && !_cached)
    {
        _group->cache(this);
    }
    return _value;
}

template <typename T>
void
TypedProperty<T>::set (const T& value)
{
    if (value == _value)
    { 
        _cached = true;
        return;
    }
    else
    { 
        _value = value;
        {
            uncache();
        }
        _cached = true;
    }
}

typedef TypedProperty <std::string>                  StringProperty;    
typedef TypedProperty <bool>                         BoolProperty;
typedef TypedProperty <float>                        FloatProperty;
typedef TypedProperty <int32_t>                      IntProperty;
typedef TypedProperty <uint32_t>                     UIntProperty;
typedef TypedProperty <ITexture*>                    TextureProperty;
typedef TypedProperty <Ibl::Matrix44f>                MatrixProperty;
typedef TypedProperty <Ibl::Quaternionf>              QuaternionProperty;
typedef TypedProperty <Ibl::Vector3f>                 VectorProperty;
typedef TypedProperty <Ibl::Vector4f>                 Vector4fProperty;
typedef TypedProperty <Ibl::PixelFormat>              PixelFormatProperty;
typedef TypedProperty <Ibl::IDevice*>                 DeviceProperty;
typedef TypedProperty <float*>                       FloatPtrProperty;
typedef TypedProperty <uint32_t*>                    UIntPtrProperty;
typedef TypedProperty <Region3f>                        BoundingBoxTypedProperty;
typedef TypedProperty <Ibl::VertexStream*>            VertexStreamProperty;
typedef TypedProperty < std::vector<float> >         FloatArrayProperty;
typedef TypedProperty <Ibl::IDepthSurface*>           DepthSurfaceTypedProperty;
typedef TypedProperty <ITexture*>                    RenderTextureTypedProperty;
typedef TypedProperty <ITexture*>                    Texture2DTextureTypedProperty;
typedef TypedProperty <ITexture*>                    CubeTextureTypedProperty;
typedef TypedProperty <const ITexture*>              ConstTextureProperty;
typedef TypedProperty <Ibl::Viewport>                 ViewportTypedProperty;
typedef TypedProperty <std::vector<Ibl::Matrix44f> >  MatrixArrayProperty;
typedef TypedProperty <std::vector<float> >          FloatArrayProperty;
typedef TypedProperty <std::vector<Ibl::Vector4f>>    Vector4fArrayProperty;
typedef TypedProperty <std::vector<std::string>>     StringArrayProperty;

}

#endif