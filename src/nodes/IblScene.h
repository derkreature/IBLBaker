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
#ifndef INCLUDED_BB_SCENE
#define INCLUDED_BB_SCENE

#include <IblPlatform.h>
#include <IblNode.h>
#include <IblRenderNode.h>



namespace Ibl
{
class Entity;
class Mesh;
class Material;
class Camera;
class Brdf;
class IBLProbe;

class Scene : public Ibl::RenderNode
{
  public:
    Scene(Ibl::IDevice* device);
    virtual ~Scene();

    // Load an entity from disk
    Entity *                   load(const std::string& fileName, 
                                    const std::string& userMaterialPathName);

    // Load an entity from disk
    static Entity *            load(Ibl::IDevice* device,
                                    const std::string& fileName);

    void                       destroy(Entity* entity);

    const Camera *             camera() const;
    Camera *                   camera();

    const std::vector<Ibl::Mesh*>& meshesForPass(const std::string& passName) const;

    const std::vector<IBLProbe*>& probes() const;
    IBLProbe*                   addProbe();

    const Brdf*                activeBrdf() const;
    IntProperty*               activeBrdfProperty();

    void                       update();

    bool                       loadBrdfs();

  protected:
    void                       addMesh(Mesh* mesh);
    void                       addToPass(const std::string& passName,
                                         Mesh* mesh);

  private:  
    Camera*                    _camera;
    IntProperty*               _activeBrdfProperty;
    EnumTweakType *            _brdfType;
    typedef std::vector<Brdf*> BrdfCache;
    BrdfCache                  _brdfCache;

    std::set<Entity*>          _entities;
    std::vector<IBLProbe*>     _probes;
    std::set<Material*>        _materials;
    std::map<std::string, std::vector<Ibl::Mesh*> > _meshesByPass;
};

}

#endif