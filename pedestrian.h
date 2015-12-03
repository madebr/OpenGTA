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
#ifndef OGTA_PEDESTRIAN_H
#define OGTA_PEDESTRIAN_H

#include <SDL.h>
#include "math3d.h"
#include "obox.h"
#include "animation.h"
#include "opengta.h"

namespace OpenGTA {
  struct Projectile {
      Projectile(uint8_t, float, Vector3D, Vector3D, Uint32);
      uint8_t typeId;
      float   rot;
      Vector3D pos;
      Vector3D delta;
      Uint32   endsAtTick;
  };
  class SpriteObject {
    public:
      SpriteObject(const Vector3D & p);
      SpriteObject(const SpriteObject & other);
      struct Animation : public Util::Animation {
        Animation();
        Animation(const Animation & other);
        Animation(Uint16 foff, Uint8 num);
        Animation(Uint16 foff, Uint8 num, float speed);
        Uint16 firstFrameOffset;
        //Uint8  numFrames;
        float  moveSpeed;
      };
      Vector3D pos;
      //Uint8 curFrame;
      Uint16 sprNum;
      Sint16 remap;
      //Animation * animRef;
      Animation anim;
      bool animActive;
      void update(Uint32 ticks);
      Uint32 lastFrameUpdateAt;
      Uint32 lastUpdateAt;
      float rot;
      GraphicsBase::SpriteNumbers::SpriteTypes sprType;
      void setAnimation(Animation & otherAnim);
    protected:
      Uint8 calcCurrentFrame(Uint32 ticks);
      float heightOverTerrain(const Vector3D & v);
    private:
      void copyValues(const SpriteObject & other);
      SpriteObject & operator = (const SpriteObject & other) { return *this; }
  };
  
  class Pedestrian : public SpriteObject, public OBox {
    public:
      struct Controller {
        // turn, move,
        // run/walk/swim/crawl
        // jump, shoot/punch, 
        Sint8 turn;
        Sint8 move;
      };
      Pedestrian(const Vector3D & e, const Vector3D & pos);
      Pedestrian(const Vector3D & e, const Vector3D & pos, const Uint32 & asId);
      Pedestrian(const Pedestrian & other);
      Uint32 pedId;
      Controller* m_control;
      Uint32 animId;
      Vector3D speedForces;
      void switchToAnim(const Uint32 & newId);

      void update(Uint32 ticks);
      void equip(uint8_t eq_id);
      void giveItem(uint8_t id, uint32_t amount);
      void fireWeapon(Uint32 ticks);
    private:
      typedef std::map<uint8_t, uint32_t> InventoryType;
      InventoryType inventory;
      uint8_t activeWeapon;
      bool inGroundContact;
      void tryMove(Vector3D nPos);
      void copyValues(const Pedestrian & other);
      Uint32 weaponReloadedAt;
      //Uint8 calcCurrentFrame(Uint32 ticks);
      Pedestrian & operator = (const Pedestrian & other) { return *this; }
  };

  class Car : public SpriteObject, public OBox {
    public:
      Car(const OpenGTA::Map::ObjectPosition & op);
      Car(const Car & other);
      Uint32 delta;
      Uint32 carId;
      Uint8  type;
    private:
      GraphicsBase::CarInfo & c_info;
      void copyValues(const Car & other);
      Car & operator = (const Car & other) { return *this; }

  };

  class GameObject : public SpriteObject, public OBox {
    public:
      GameObject(const OpenGTA::Map::ObjectPosition & op);
      GameObject(const GameObject & other);
      Uint32 objId;
    private:
      void copyValues(const GameObject & other);
      GameObject & operator = (const GameObject & o) { return *this; }
  };
}

#endif
