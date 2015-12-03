/************************************************************************
* Copyright (c) 2005-2006 tok@openlinux.org.uk                          *
*                                                                       *
* This software is provided as-is, without any express or implied       *
* warranty. In no event will the authors be held liable for any         *
* damages arising from the use of this software.                        *
*                                                                       *
* Permission is granted to anyone to use this software for any purpose, *
* including commercial applications, and to alter it and redistribute   *
* it freely, subject to the following restrictions:                     *
*                                                                       *
* 1. The origin of this software must not be misrepresented; you must   *
* not claim that you wrote the original software. If you use this       *
* software in a product, an acknowledgment in the product documentation *
* would be appreciated but is not required.                             *
*                                                                       *
* 2. Altered source versions must be plainly marked as such, and must   *
* not be misrepresented as being the original software.                 *
*                                                                       *
* 3. This notice may not be removed or altered from any source          *
* distribution.                                                         *
************************************************************************/
#ifndef OGTA_SpriteManager_H
#define OGTA_SpriteManager_H

#include <list>
#include <map>
#include "pedestrian.h"
#include "Singleton.h"

namespace OpenGTA {
  class SpriteManager {
    public:
      SpriteManager();
      ~SpriteManager();
      void drawInRect(SDL_Rect & r);
      void clear();

      void addPed(Pedestrian & ped);
      Pedestrian & getPedById(const Uint32 & id);
      void removePedById(const Uint32 & id);
      
      void addCar(Car & car);
      Car & getCarById(const Uint32 & id);
      void removeCarById(const Uint32 & id);

      void addObject(GameObject & go);
      GameObject & getObjectById(const Uint32 & id);
      void removeObjectById(const Uint32 & id);

      void update(Uint32 ticks);
      SpriteObject::Animation & getAnimationById(const Uint32 & id);
      void registerAnimation(const Uint32 & id, const SpriteObject::Animation & anim);

      inline bool getDrawTexture() { return (drawMode & 1); }
      inline bool getDrawTexBorder() { return (drawMode & 2); }
      inline bool getDrawBBox() { return (drawMode & 4); }
      void setDrawTexture(bool v);
      void setDrawTexBorder(bool v);
      void setDrawBBox(bool v);

      void drawPed(Pedestrian & ped);
      void drawCar(Car & car);
      void drawObject(GameObject & obj);

      void createProjectile(uint8_t typeId, float, Vector3D p, Vector3D d, Uint32 & ticks);
      void drawProjectile(Projectile & p);
      void collideProjectile(Projectile & p);

    protected:
      typedef std::list<Pedestrian> PedListType;
      PedListType activePeds;
      typedef std::list<Car> CarListType;
      CarListType activeCars;
      typedef std::list<GameObject> ObjectListType;
      ObjectListType activeObjects;
      typedef std::list<Projectile> ProjectileListType;
      ProjectileListType activeProjectiles;
      typedef std::map<Uint32, SpriteObject::Animation> AnimLookupType;
      AnimLookupType animations;
    private:
      Uint32 drawMode;
  };

  typedef Loki::SingletonHolder<SpriteManager, Loki::CreateUsingNew, 
    Loki::DefaultLifetime, Loki::SingleThreaded> SpriteManagerHolder; 
}

#endif
