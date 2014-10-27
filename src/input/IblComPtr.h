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

#ifndef INCLUDED_COM_PTR
#define INCLUDED_COM_PTR

// Lifted from: http://www.sjbrown.co.uk/2002/08/01/the-buggy-demo/
// Many, many many years ago.

#include <IblPlatform.h>

namespace Ibl
{
#define DELETER(name, function) \
    template<class T> \
class Bb##name##Deleter { public: inline static void Free(T*& p) throw() { function } };

DELETER(Operator, delete p;)
DELETER(Array, delete[] p;)
DELETER(Com, p->Release();)
DELETER(Free, free(static_cast<void*>(p));)
DELETER(Close, fclose(p);)
DELETER(Null, delete p;)

template<class T, class Deleter = BbOperatorDeleter<T> >
class SmartPtr
{
protected:
    T* p;

public:
    SmartPtr() throw() : p(0) {}
    
    ~SmartPtr() throw()
    {
        free();
    }

    void free() throw()
    {
        if(p)
        {
            Deleter::Free(p);
            p = 0;
        }
    }

    SmartPtr(T* const& p_) throw() : p(p_) {}

    T* const& operator=(T* const& p_) throw()
    {
        free();
        p = p_;
        return p;
    }

    T* detach() throw()
    {
        T* temp = p;
        p = 0;
        return temp;
    }

    operator T* const&() const throw()
    {
        return p;
    }
    T* const& operator->() const
    {
        if(p == 0)
            throw; 
        return p;
    }
    T** operator&()
    {
        return &p;
    }
};
template<class T>
class ArrayPtr { public: typedef SmartPtr<T, BbArrayDeleter<T> > Type; };

template<class T>
class ComPtr { public: typedef SmartPtr<T, BbComDeleter<T> > Type; };
}

#endif