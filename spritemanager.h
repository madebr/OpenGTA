/************************************************************************
* Copyright (c) 2005-2007 tok@openlinux.org.uk                          *
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
#include "abstract_container.h"
//#include "pedestrian.h"
#include "game_objects.h"
#include "train_system.h"
#include "map_helper.h"

namespace OpenGTA {

#if 0
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
      void drawExplosion(GameObject & obj);

      void drawTrain(TrainSegment & train);
      TrainSegment & getTrainById(const Uint32 & id);

      void createProjectile(uint8_t typeId, float, Vector3D p, Vector3D d, Uint32 & ticks);
      void createExplosion(Vector3D center);
      void drawProjectile(Projectile & p);
      void collideProjectile(Projectile & p);

      typedef std::list<TrainSegment> TrainListType;
    protected:
      typedef std::list<Pedestrian> PedListType;
      PedListType activePeds;
      typedef std::list<Car> CarListType;
      CarListType activeCars;
      typedef std::list<GameObject> ObjectListType;
      ObjectListType activeObjects;
      typedef std::list<Projectile> ProjectileListType;
      ProjectileListType activeProjectiles;
      TrainListType activeTrains;
    public:
      TrainSystem   trainSystem;
    protected:
      typedef std::map<Uint32, SpriteObject::Animation> AnimLookupType;
      AnimLookupType animations;
    private:
      Uint32 drawMode;
  };
#endif
  class SpriteManager : 
    public AbstractContainer<Pedestrian>,
    public AbstractContainer<Car>,
    public AbstractContainer<SpriteObject> { //,
    //public AbstractContainer<TrainSegment> {
      private:
        SpriteManager();
        ~SpriteManager();

      public:
        SpriteManager(const SpriteManager& copy) = delete;
        SpriteManager& operator=(const SpriteManager& copy) = delete;

        static SpriteManager& Instance()
        {
          static SpriteManager instance;
          return instance;
        }

        void drawInRect(SDL_Rect & r);
        void clear();
        void removeDeadStuff();

        template <typename T> T & add(const T & t) {
          return AbstractContainer<T>::doAdd(t);
        }
        template <typename T> size_t getNum() {
          return AbstractContainer<T>::objs.size();
        }
        inline Pedestrian & getPed(uint32_t id) {
          return AbstractContainer<Pedestrian>::doGet(id);
        }
        inline Car & getCar(uint32_t id) {
          return AbstractContainer<Car>::doGet(id);
        }
        inline SpriteObject & getObject(uint32_t id) {
          return AbstractContainer<SpriteObject>::doGet(id);
        }
        /*
        inline TrainSegment & getTrain(uint32_t id) {
          return AbstractContainer<TrainSegment>::doGet(id);
        }*/

        template <typename T> inline std::list<T> & getList() {
          return AbstractContainer<T>::objs;
        }

        inline void removePed(uint32_t id) {
          AbstractContainer<Pedestrian>::doRemove(id);
        }
        inline void removeCar(uint32_t id) {
          AbstractContainer<Car>::doRemove(id);
        }
        void realRemove();

        inline bool getDrawTexture() { return (drawMode & 1); }
        inline bool getDrawTexBorder() { return (drawMode & 2); }
        inline bool getDrawBBox() { return (drawMode & 4); }
        void setDrawTexture(bool v);
        void setDrawTexBorder(bool v);
        void setDrawBBox(bool v);
        void drawBBoxOutline(const OBox &);
        void drawTextureOutline(const float &, const float &);

        void draw(Car &);
        void draw(Pedestrian &);
        void draw(SpriteObject &);
        void draw(Projectile &);

        void drawExplosion(SpriteObject &);

        void update(Uint32 ticks);

        SpriteObject::Animation & getAnimationById(const Uint32 & id);
        void registerAnimation(const Uint32 & id, const SpriteObject::Animation & anim);

        void createExplosion(Vector3D center);
        void createProjectile(uint8_t typeId, float, Vector3D p, Vector3D d, Uint32 & ticks, Uint32 & owner);


      public:
        //TrainSystem   trainSystem;
        Util::SpriteCreationArea creationArea;
      protected:
        typedef std::map<Uint32, SpriteObject::Animation> AnimLookupType;
        AnimLookupType animations;
        typedef std::list<Projectile> ProjectileListType;
        ProjectileListType activeProjectiles;

      private:
        Uint32 drawMode;
        Uint32 lastCreateTick;
    };
}

#endif
